// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Implementation of a private key for ECSecp256k1.
 */

#include "KeyConverter.h"
#include "contrib/lax_der_privatekey_parsing.h"

#include <impl/JSONSerialiser.h>
#include <impl/Base58Data.h>
#include <impl/RNGAdapter.h>
#include <impl/MetaDataCollectionImpl.h>
#include <impl/utils/EnumCast.h>
#include <impl/utils/Utils.h>
#include <impl/utils/HexToBytes.h>
#include <impl/utils/Hashers.h>

#include <cryptopp/eccrypto.h>
#include <cryptopp/channels.h>
#include <cryptopp/ida.h>
#include <cryptopp/files.h>
#include <cryptopp/oids.h>

#include <secp256k1/include/secp256k1.h>

namespace pt = boost::property_tree;
using namespace nakasendo;
using namespace nakasendo::impl;
using namespace nakasendo::impl::utils;

namespace nakasendo { namespace native
{   

// Initialise default network
template <class Seeder>
std::atomic<MetaDataDefinitions::NetworkType> KeyECSecp256k1<Seeder>::cDefaultNetworkType(MetaDataDefinitions::NetworkType::MAIN_NET);

// Register ourselves with the JSONSerialiser
template <class Seeder>
bool KeyECSecp256k1<Seeder>::KeyECSecp256k1JSONRegistered =
    JSONSerialiser::registerForDeserialise<KeyECSecp256k1<Seeder>>(KeyECSecp256k1<Seeder>{}.jsonObjectType());


template <class Seeder>
KeyECSecp256k1<Seeder>::KeyECSecp256k1()
{   
    // This funky NO-OP is here purely to prevent the linker optimising away our static
    // member KeyECSecp256k1JSONRegistered. Otherwise it sees there are no other users
    // of the static, strips it out of the executable it is linking and we never register
    // with the JSON factory.
    KeyECSecp256k1JSONRegistered = KeyECSecp256k1JSONRegistered;

    init();
    makeNewKey();
}

template <class Seeder>
KeyECSecp256k1<Seeder>::KeyECSecp256k1(const memory::SecureByteVec& seed)
    : m_seeder(seed)
{
    init();
    makeNewKey();
}

template <class Seeder>
KeyECSecp256k1<Seeder>::KeyECSecp256k1(const Secret& secret)
    : Key{secret}
{
    init();
    if (!isValidSecpKey())
    {
        throw std::runtime_error("Private key is not secp256k1 compliant key.");
    }
}

template <class Seeder>
KeyECSecp256k1<Seeder>::KeyECSecp256k1(Secret&& secret)
    : Key{std::move(secret)}
{
    init();
    if (!isValidSecpKey())
    {
        throw std::runtime_error("Private key is not secp256k1 compliant key.");
    }
}

template <class Seeder>
KeyECSecp256k1<Seeder>::KeyECSecp256k1(const KeyFragments& keyFragments)
{
    init();
    setSecret(recover(keyFragments));
    nameSecret();
}

template <class Seeder>
bool KeyECSecp256k1<Seeder>::checkSecKey(const unsigned char *vch) const
{
    return secp256k1_ec_seckey_verify(getEccContext(), vch);
}

template <class Seeder>
memory::SecureByteVec KeyECSecp256k1<Seeder>::getSerializedKey() const
{
    using namespace contrib;
    memory::SecureByteVec vSerializedKey(SERIALIZED_KEY_ECSECP256K1_SIZE);
    size_t serializedKeyLen = SERIALIZED_KEY_ECSECP256K1_SIZE;
    const memory::SecureByteVec& secret = getSecret();
    /// Export the key in DER format.
    if (!ec_privkey_export_der(getEccContext(), (unsigned char*) vSerializedKey.data(), &serializedKeyLen, 
                               (const unsigned char*) secret.data(), SECP256K1_EC_UNCOMPRESSED)) {
        throw std::runtime_error("Serialization has failed.");
    }
    vSerializedKey.resize(serializedKeyLen);
    return vSerializedKey;
}

template <class Seeder>
memory::SecureByteVec KeyECSecp256k1<Seeder>::getDeserializedKey(const memory::SecureByteVec& vSerializedSecret) const
{
    using namespace contrib;
    memory::SecureByteVec vSecretKey(KEY_ECSECP256K1_SIZE);
    /// Import the key in DER format.
    if (!ec_privkey_import_der(getEccContext(), reinterpret_cast<unsigned char*>(vSecretKey.data()),
                               reinterpret_cast<const unsigned char*>(vSerializedSecret.data()), vSerializedSecret.size())) {
        throw std::runtime_error("Recovered key is invalid");
    }
    return vSecretKey;
}

/**
* Re-constructs a key that is previously split into fragments.
* Crypto++ lib operates based data flows. A functionality is abstracted as
* data taken from a source (e.g. ArraySource, or FileSource), goes through
* a transformation (e.g. Shamir's data sharing), which is "attached" to the
* source, and finally goes into a "sink". Sources and Sinks are templates,
* whose concrete types handle different underlying mediums, e.g. in-memory
* string or a disk file. When the pipelines are fully established,
* 'pumping' (e.g. source->PumpAll) triggers the transformation, whose
* results flow into the sinks.
*
* Crypto++'s Shamir's secret sharing also requires the 'ChannelSwitch'
* class, which is used to distribute fragments to their individual 'sink'.
* The 'ChannelSwitch' itself is not part of Shamir's algorithm, but rather
* a mandatory helper for Crypto++.
*
* Extra care is needed with object ownership. Crypto++ has less intuitive
* ownership rules. Different data flow causes different ownerships. The
* rule of thumb seems to be if one object's pointer is passed into another,
* then it's owned by the other object. This may cause confusions, e.g. in
* secret recovery, ChannelSwitch is owned by the sources. Yet in splitting,
* ChannelSwitch is owned by SecretSharing.
* 
* Best to only release a pointer after it's owner has been constructed
* successfully: unless it's a simple 'Attach' (which immediately changes
* ownership), passing raw pointers risks leaks when construction fails.
*/
template <class Seeder>
memory::SecureByteVec KeyECSecp256k1<Seeder>::recover(const KeyFragments& keyFragments)
{
    using namespace CryptoPP;
	if (!keyFragments.size()) {
		throw std::runtime_error("Cannot re-construct from empty key fragment list.");
	}

    /**
    * Step 1: prepare outputs.
    */
    /// Create a buffer to store the result.
    SecureOstringStream ossRecoveredKey;
    /// Shamir's Secret Sharing Algorithm.
    /// The name 'FileSink' is misleading, it should really be 'BufferedSink' as it allows attaching to any ostream.
    SecretRecovery secRecovery(keyFragments.size(), new FileSink(ossRecoveredKey));

    /**
    * Step 2: prepare inputs (setup mechanism of recovery).
    */
    /// Get a copy of all fragments.
    std::vector<memory::SecureByteVec> vKeyFragments;
    for (auto &iter : keyFragments) {
        vKeyFragments.push_back(iter.get().getSecret());
    }
    /// Each fragment needs it's own source.
    vector_member_ptrs<ArraySource> vSources(keyFragments.size());

    try
    {
        SecByteBlock channelId(NULL, 4);
        for (int i=0; i<keyFragments.size(); i++)
        {   /// Create a new source of input bytes.
            vSources[i].reset(new ArraySource(reinterpret_cast<const byte*>(vKeyFragments[i].data()), vKeyFragments[i].size(), false));
            /// Pump data to attached transformation.
            vSources[i]->Pump(4);
            /// Get channel's id.
            vSources[i]->Get(channelId, 4);
            /// Add ChannelSwitch to the end of attachment chain.
            vSources[i]->Attach(new ChannelSwitch(secRecovery, std::string((char*) channelId.begin(), 4)));
        }
        /// Maximpum number of bytes to pump (256 bytes)
        /// Pump returns the number of bytes that remain in the block (i.e., bytes not processed).
        /// It needs to be in a loop for the first element of the vector (self-check for blocking input).
        while (vSources[0]->Pump(256)) 
        {
            for (int i=1; i<vSources.size(); i++) {
                 vSources[i]->Pump(256);
            }
        }

        /**
        * Step 3: Trigger recovery.
        */
        /// Pump all data to attached transformation.
        for (int i=0; i<vSources.size(); i++)
            vSources[i]->PumpAll();
    }
    catch(Exception& e) {
        throw std::runtime_error(std::string{"Error during key's recovery: "} + e.what());
    }

    /// Convert serialized secret from ostringstream into impl::memory::SecureByteVec.
    memory::SecureByteVec vSerializedSecret;
    const auto& rawRecovered = ossRecoveredKey.str();
    std::copy(rawRecovered.begin(), rawRecovered.end(), std::back_inserter(vSerializedSecret));
    /// Return deserialized private key.
    return getDeserializedKey(vSerializedSecret);
}

template <class Seeder>
void KeyECSecp256k1<Seeder>::makeNewKey()
{
    memory::SecureByteVec keydata(KEY_ECSECP256K1_SIZE);
    // It generates a contiguous sequence of 256 bits (32bytes)
    do {
        m_seeder.generate(keydata);
    } while (!checkSecKey(keydata.data()));

    setSecret(keydata);
    nameSecret();
}

template <class Seeder>
PubKeyPtr KeyECSecp256k1<Seeder>::getPubKey() const
{
    if(getCompressionType() == MetaDataDefinitions::CompressionType::COMPRESSED)
    {
        return getPubKey(PubKey::Type::COMPRESSED);
    }
    else
    {
        return getPubKey(PubKey::Type::UNCOMPRESSED);
    }
}
 
template <class Seeder>
PubKeyPtr KeyECSecp256k1<Seeder>::getPubKey(PubKey::Type pkType) const
{
    /// Check if private key is valid.
	if(!isValidSecpKey()) {
        throw std::runtime_error("Private key is not secp256k1 compliant key.");
    }

    /// Create an instance of ECSECP256K1 public key.
    const MetaDataDefinitions::NetworkType& networkType {
        enum_cast<MetaDataDefinitions::NetworkType>(getMetaDataCollection()->getMetaValue(
            enum_cast<std::string>(MetaDataDefinitions::KeyType::NETWORK_TYPE)))
    };
    return std::make_unique<PubKeyECSecp256k1>(networkType, pkType, getSecret());
}

/**
* Splitting the raw private key into a vector of shares, by using Crypto++'s Shamir algorithm. 
* See comments in impl::memory::SecureByteVec KeyECSecp256k1<Seeder>::recover(const KeyFragments& keyFragments) for notes on Crypto++.
*/
template <class Seeder>
std::vector<KeyFragmentPtr> KeyECSecp256k1<Seeder>::split(const int nSplitSharesN, const int nRecoverySharesM) const
{
    using namespace CryptoPP;

    if (nSplitSharesN < 0)
    {
        throw std::runtime_error("Unexpected negative number");
    }

    if ((nRecoverySharesM > nSplitSharesN) || (nRecoverySharesM <= 0))
    {
        throw std::runtime_error("Invalid Number of Recovery Shares");
    }
    /// A vector of fragments (result of split operation).
	std::vector<KeyFragmentPtr> vKeyFragments;

    /**
    * Step 1: prepare inputs.
    */
    /// Get serialized private key (we need to serialize the key before splitting).
    const memory::SecureByteVec vSerializedKey(getSerializedKey());

    /**
    * Step 2: prepare outputs (setup a mechanism of split operation).
    */
    /// Create an output buffer for each fragment.
	std::vector<SecureOstringStream> vShares(nSplitSharesN);
    AutoSeededRandomPool rng;
    ChannelSwitch *channelSwitch = nullptr;
    vector_member_ptrs<FileSink> vSinks(nSplitSharesN);

    try
    {
        /// SecretSharing (Shamir's Secret Sharing Algorithm) which performs a splitting operation.
        SecretSharing *bufferedTransFilter { new SecretSharing(rng, nRecoverySharesM, nSplitSharesN, channelSwitch = new ChannelSwitch) };
        ArraySource source(reinterpret_cast<const byte*>(vSerializedKey.data()), vSerializedKey.size(), false, bufferedTransFilter);
        std::string sChannelId;
        for (int i=0; i<nSplitSharesN; i++)
        {
            /// Combine a sink with a share stream.
            vSinks[i].reset(new FileSink(std::ref(vShares[i])));
            /// Create a named channel.
            sChannelId = WordToString<word32>(i);
            vSinks[i]->Put(reinterpret_cast<const byte*>(sChannelId.data()), 4);
            /// Route input to different and/or multiple channels based on sChannelId
            channelSwitch->AddRoute(sChannelId, *vSinks[i], DEFAULT_CHANNEL);
        }

        /**
        * Step 3: Trigger split.
        */
        /// Pump all data to attached transformation.
        source.PumpAll();
    }
    catch(Exception& e) {
        throw std::runtime_error(std::string{"Error during key's splitting: "} + e.what());
    }

    // Create compressed public key.
    PubKeyPtr pubKey { getPubKey(PubKey::Type::COMPRESSED) };

    /**
    * Step 4: Instantiate key's fragments.
    */
	/// Convert a result into a fragment's type.
    uint8_t nFragmentIdx {1};
    std::transform(vShares.begin(), vShares.end(), std::back_inserter(vKeyFragments), [&](const auto & oss){
        const auto & str { memory::SecureString(oss.str()) };
        /// Instantiate key's fragment.
        return std::make_unique<KeyFragmentECSecp256k1>(Secret{memory::SecureByteVec(str.begin(), str.end())},
                                                        nFragmentIdx++,
                                                        pubKey->getContentAsHexString(),
                                                        this->getMetaDataCollection());
    });

    return vKeyFragments;
}

template <class Seeder>
bool KeyECSecp256k1<Seeder>::isValidSecpKey() const
{
    return checkSecKey(getSecret().data());
}

// Export ourselves in wallet import format (WIF).
template <class Seeder>
memory::SecureString KeyECSecp256k1<Seeder>::exportAsWIF() const
{
    return Base58Data::checkEncode(*this);
}

// Import ourselves from wallet import format (WIF).
template <class Seeder>
void KeyECSecp256k1<Seeder>::importFromWIF(const memory::SecureString& wif)
{
    // Decode WIF
    memory::SecureByteVec rawKey { Base58Data::checkDecode(wif) };

    // Identify version prefix so we know how many bytes to skip at start
    Base58Data::VersionPrefix ver { Base58Data::getVersionOf(rawKey) };
    size_t versionBytesLength { Base58Data::getVersionBytesFor(ver).size() };
    rawKey.erase(rawKey.begin(), rawKey.begin() + versionBytesLength);

    // Check remaining length is as expected
    if(rawKey.size() != KEY_ECSECP256K1_SIZE && rawKey.size() != KEY_ECSECP256K1_SIZE + 1)
    {   
        throw std::runtime_error("Key decoded from WIF has bad length " + std::to_string(rawKey.size()));
    }
 
    // Compressed WIF?
    if(rawKey.size() == KEY_ECSECP256K1_SIZE + 1 && rawKey.back() == 0x01)
    {
        rawKey.pop_back();
        setCompressionType(MetaDataDefinitions::CompressionType::COMPRESSED);
    }
    else
    {
        setCompressionType(MetaDataDefinitions::CompressionType::UNCOMPRESSED);
    }

    // Set network type metadata
    std::string key { enum_cast<std::string>(MetaDataDefinitions::KeyType::NETWORK_TYPE) };
    std::string val { enum_cast<std::string>(Base58Data::getNetworkFor(ver)) };
    setMetaData({key, val});

    // Set our new secret
    setSecret(rawKey);
    nameSecret();
}

// Sign a message with this key.
template <class Seeder>
std::vector<uint8_t> KeyECSecp256k1<Seeder>::sign(const std::vector<uint8_t>& message) const
{
    // We have to sign a 32 byte block, so sign a hash
    uint256 hash { Hash256(message.begin(), message.end()) };

    // Seed signature generator
    memory::SecureByteVec seed(32);
    m_seeder.generate(seed);

    // Do the signing
    memory::SecureByteVec privBytes { getSecret() };
    secp256k1_ecdsa_signature sig {};
    if(!secp256k1_ecdsa_sign(getEccContext(), &sig, hash.begin(), privBytes.data(),
                             secp256k1_nonce_function_rfc6979, seed.data()))
    {
        throw std::runtime_error("secp256k1 signing failed");
    }

    // DER format
    size_t sigLen{72};
    std::vector<uint8_t> sigBytes(sigLen, 0x0);
    secp256k1_ecdsa_signature_serialize_der(getEccContext(), sigBytes.data(), &sigLen, &sig);
    sigBytes.resize(sigLen);

    return sigBytes;
}

// Decrypt the given message
template <class Seeder>
memory::SecureByteVec KeyECSecp256k1<Seeder>::decrypt(const std::vector<uint8_t>& message) const
{
    // Get CryptoPP Decryptor and initialise with our private key
    CryptoPP::ECIES<CryptoPP::ECP>::Decryptor decryptor {};
    CryptoPP::DL_PrivateKey_EC<CryptoPP::ECP>& keyForDecrypting { decryptor.AccessKey() };
    KeyConverter{}.ecpPrvKeyInit(*this, keyForDecrypting);

    // Do the decrypt
    RNGAdapter<Seeder> prng { m_seeder };
    memory::SecureByteVec decrypted(message.size());
    CryptoPP::ArraySink rs { decrypted.data(), decrypted.size() };
    try
    {
        CryptoPP::ArraySource { message.data(), message.size(), true,
            new CryptoPP::PK_DecryptorFilter { prng, decryptor,
                new CryptoPP::Redirector { rs }
            }
        };
    }
    catch(CryptoPP::Exception& e)
    {
        // Rethrow
        throw std::runtime_error(std::string{"Error during decryption: "} + e.what());
    }

    decrypted.resize(rs.TotalPutLength());
    return decrypted;
}

// Deterministically derives a new key from this key using the given message.
template <class Seeder>
KeyPtr KeyECSecp256k1<Seeder>::derive(const std::vector<uint8_t>& message) const
{
    // SHA256(M)
    std::vector<uint8_t> hashM(CryptoPP::SHA256::DIGESTSIZE, 0);
    CryptoPP::SHA256().CalculateDigest(hashM.data(), message.data(), message.size());
    CryptoPP::Integer hashVal { KeyConverter::ecpScalar(hashM.data(), hashM.size()) };

    // New private key = old private key + SHA256(M)
    CryptoPP::Integer oldKeyVal { KeyConverter::ecpScalar(*this) };
    CryptoPP::Integer newKeyVal { oldKeyVal + hashVal };

    // Numbers larger than the group number are not valid private keys,
    // see DL_PrivateKeyImpl::Validate for the source of this check.
    CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP> curve { CryptoPP::ASN1::secp256k1() };
    newKeyVal %= curve.GetSubgroupOrder();

    // Derived key should be for same network as original key
    MetaDataCollectionImplPtr newMeta { std::make_shared<MetaDataCollectionImpl>() };
    MetaDataCollectionConstSPtr ourMeta { getMetaDataCollection() };
    newMeta->copyMetaData(ourMeta, MetaDataDefinitions::KeyType::NETWORK_TYPE);

    // Create and validate the new key
    memory::SecureByteVec bytes(KEY_ECSECP256K1_SIZE, 0);
    newKeyVal.Encode(bytes.data(), bytes.size());
    Secret newKeySecret { bytes, "", newMeta };
    std::unique_ptr<KeyECSecp256k1<Seeder>> derived { std::make_unique<KeyECSecp256k1<Seeder>>(std::move(newKeySecret)) };
    if(!derived->isValidSecpKey())
    {
        throw std::runtime_error("Derived private key is invalid");
    }

    return derived;
}

// Produce a shared secret by using the other party's public key with a given message
// and this private key.
template <class Seeder>
SecretPtr KeyECSecp256k1<Seeder>::sharedSecret(const PubKeyPtr&            pubKey,
                                               const std::vector<uint8_t>& message,
                                               const std::string&          secretName) const
{
    // Derive our and their new keys
    KeyPtr ourNewKey { derive(message) };
    PubKeyPtr theirNewKey { pubKey->derive(message) };
    if(!ourNewKey || !theirNewKey)
    {
        throw std::runtime_error("Failed to derive new keys");
    }

    // Our new private * their new public
    CryptoPP::DL_GroupParameters_EC<CryptoPP::ECP> curve { CryptoPP::ASN1::secp256k1() };
    CryptoPP::Integer ourNewScalar { KeyConverter::ecpScalar(*ourNewKey) };
    CryptoPP::ECPPoint theirPoint { KeyConverter::ecpPoint(*theirNewKey) };
    CryptoPP::ECPPoint secretPoint { curve.GetCurve().ScalarMultiply(theirPoint, ourNewScalar) };

    // Encode & return
    size_t encodedCoordSize { secretPoint.x.MinEncodedSize() };
    memory::SecureByteVec secret(encodedCoordSize * 2, 0);
    secretPoint.x.Encode(secret.data(), encodedCoordSize);
    secretPoint.y.Encode(secret.data() + encodedCoordSize, encodedCoordSize);

    return { std::make_unique<Secret>(secret, secretName) };
}


// Get network we are for.
template <class Seeder>
MetaDataDefinitions::NetworkType KeyECSecp256k1<Seeder>::getNetworkType() const
{
    std::string networkTypeKey { enum_cast<std::string>(MetaDataDefinitions::KeyType::NETWORK_TYPE) };
    std::string networkType { getMetaDataCollection()->getMetaValue(networkTypeKey) };
    return enum_cast<MetaDataDefinitions::NetworkType>(networkType);
}

// Set network we are for.
template <class Seeder>
void KeyECSecp256k1<Seeder>::setNetworkType(MetaDataDefinitions::NetworkType networkType)
{
    std::string networkTypeKey { enum_cast<std::string>(MetaDataDefinitions::KeyType::NETWORK_TYPE) };
    std::string networkTypeStr { enum_cast<std::string>(networkType) };
    setMetaData( {networkTypeKey, networkTypeStr} );

    // We might need to rename our secret after changing network
    nameSecret();
}

// Get the compression type for this key.
template <class Seeder>
MetaDataDefinitions::CompressionType KeyECSecp256k1<Seeder>::getCompressionType() const
{
    std::string compressionTypeKey { enum_cast<std::string>(MetaDataDefinitions::KeyType::COMPRESSION_TYPE) };
    std::string compressionType { getMetaDataCollection()->getMetaValue(compressionTypeKey) };
    return enum_cast<MetaDataDefinitions::CompressionType>(compressionType);
}

// Set compression type for this key
template <class Seeder>
void KeyECSecp256k1<Seeder>::setCompressionType(MetaDataDefinitions::CompressionType compressionType)
{
    std::string compressionTypeKey { enum_cast<std::string>(MetaDataDefinitions::KeyType::COMPRESSION_TYPE) };
    std::string compressionTypeStr { enum_cast<std::string>(compressionType) };
    setMetaData( {compressionTypeKey, compressionTypeStr} );
}

// Set the default Bitcoin network we use for all new keys.
template <class Seeder>
void KeyECSecp256k1<Seeder>::setDefaultNetwork(MetaDataDefinitions::NetworkType network)
{
    cDefaultNetworkType = network;
}


// Get unique ID for this object type.
template <class Seeder>
const std::string KeyECSecp256k1<Seeder>::jsonObjectType() const
{
    return "KeyECSecp256k1";
}

// Export ourselves to JSON.
template <class Seeder>
pt::ptree KeyECSecp256k1<Seeder>::toJson(const SerialisationFormat& fmt) const
{
    // What style to produce?
    if(fmt.mStyle == SerialisationFormat::Style::WIF)
    {
        pt::ptree root {};

        // Wallet import format
        memory::SecureString encodedWIF { Base58Data::checkEncode(*this) };
        root.add("WIF", encodedWIF);

        // Metadata
        root.add_child("MetaData", getMetaDataCollection()->toJson(fmt));
        return root;
    }
    else
    {
        return Secret::toJson(fmt);
    }
}

// Set our state from JSON.
template <class Seeder>
void KeyECSecp256k1<Seeder>::fromJson(const pt::ptree& root, const SerialisationFormat& fmt)
{
    // What format to parse?
    if(fmt.mStyle == SerialisationFormat::Style::WIF)
    {
        // Decode WIF
        const std::string& wif { root.get<std::string>("WIF") };
        memory::SecureString encodedWIF{};
        encodedWIF.insert(encodedWIF.begin(), wif.begin(), wif.end());
        importFromWIF(encodedWIF);

        // Set metadata
        mMetaData->fromJson(root.get_child("MetaData"), fmt);
    }
    else
    {
        Secret::fromJson(root, fmt);
    }
}

// Pick a name for our secret
template <class Seeder>
void KeyECSecp256k1<Seeder>::nameSecret()
{
    if(!empty() && getName().empty())
    {
        // Use public key as string
        PubKeyPtr pubKey { getPubKey(PubKey::Type::COMPRESSED) };
        setName(pubKey->getContentAsHexString());
    }
}

// One time initialisation.
template <class Seeder>
void KeyECSecp256k1<Seeder>::init()
{
    // Set default network for this key if required
    if(!getMetaDataCollection()->keyExists(enum_cast<std::string>(MetaDataDefinitions::KeyType::NETWORK_TYPE)))
    {
        setNetworkType(cDefaultNetworkType);
    }

    // Set compressed/uncompressed for this key if required
    if(!getMetaDataCollection()->keyExists(enum_cast<std::string>(MetaDataDefinitions::KeyType::COMPRESSION_TYPE)))
    {
        // Default to compressed for now. TODO: Do we want this to be configurable?
        setCompressionType(MetaDataDefinitions::CompressionType::COMPRESSED);
    }
}

}}

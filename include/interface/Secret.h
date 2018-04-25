// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * A class for handling Secrets. A secret is a secure sequence of bytes
 * with a name and some associated metadata.
 */

#ifndef _NCHAIN_SDK_SECRET_H_
#define _NCHAIN_SDK_SECRET_H_

#include <string>
#include <memory>
#include <mutex>

#include <impl/memory/SecureVector.h>
#include <impl/memory/SecureArray.h>
#include "JSONSerialisable.h"

#include <impl/MetaDataCollectionImpl.h>

#include <cryptopp/aes.h>

namespace nakasendo
{
/// Forward declaration of Secret pointer type
class Secret;
/// Unique pointer type
using SecretPtr = std::unique_ptr<Secret>;
/// Shared pointer type
using SecretSPtr = std::shared_ptr<Secret>;

/// A class to manage a Secret.
class Secret : public JSONSerialisable
{
  public:

    /// Default constructor
    Secret();

    /**
    * Construct from some secret bytes with an optional name and initial
    * metadata.
    * @param secret A vector of secret bytes.
    * @param name The name for this Secret. If not specified a random name
    * will be generated.
    * @param metaData Optional initial metadata, can be null.
    */
    Secret(const impl::memory::SecureByteVec&             secret,
           const std::string&           name = {},
           const MetaDataCollectionSPtr& metaData = nullptr);

    Secret(const impl::memory::SecureArray<uint8_t>&      secret,
           const std::string&           name = {},
           const MetaDataCollectionSPtr& metaData = nullptr);

    /// Copy constructor
    Secret(const Secret& that);

    /// Move constructor
    Secret(Secret&& that);

    /// Assignemnt
    Secret& operator=(const Secret& that);

    /// Move assignemnt
    Secret& operator=(Secret&& that);

    /// Destructor
    virtual ~Secret() = default;

    /**
    * Get our secret.
    * @return The unencrypted secret contents.
    */
    impl::memory::SecureByteVec getSecret() const;
    void getSecret(impl::memory::SecureByteVec& secret)const;
    void getSecret(impl::memory::SecureArray<uint8_t>& secret) const;

    /**
    * Set our secret.
    * @param secret A new secret value to use.
    */
    void setSecret(const impl::memory::SecureByteVec& secret);
    void setSecret(const impl::memory::SecureArray<uint8_t>& secret);

    /**
    * Get our encrypted secret.
    * @return The unencrypted secret contents. If we haven't been encrypted
    * then this will be empty.
    */
    impl::memory::SecureByteVec getEncryptedSecret() const;
    void getEncryptedSecret(impl::memory::SecureByteVec& secret)const;
    void getEncryptedSecret(impl::memory::SecureArray<uint8_t>& secret) const;

    /**
    * Do we have any secret data?
    * @return True if we don't have any secret data.
    */
    bool empty() const;

    /**
    * Get the name associated with this secret.
    * @return The secrets name.
    */
    const std::string getName() const;

    /**
    * Set The name associated with this secret.
    * @param name A new name for this secret.
    */
    void setName(const std::string& name);

    /**
    * Get the metadata associated with this secret. This returns a read only
    * reference, if you want to change the metadata you have to use the
    * explicit method below so that all changes can be trapped and forwarded
    * to the secret store.
    * @return A const pointer to the metadata for this secret.
    */
    MetaDataCollectionConstSPtr getMetaDataCollection() const { return mMetaData; }

    /**
    * Set some metadata on this Secret.
    * @param meta Some metadata to set.
    */
    void setMetaData(const MetaData& meta);

    /**
    * Encrypt this secret using the given password.
    * @param passwd A password to use when encrypting this secret.
    */
    void encryptSecret(const impl::memory::SecureByteVec& passwd);

    template<typename Iter>
    void encryptSecret(const Iter& begin, const Iter& end){
        static_assert( sizeof(typename Iter::value_type) == sizeof(uint8_t), "Wrong iterator type");

        std::lock_guard<std::mutex> lck { mMtx };
        auto size = std::distance(begin, end);
        // Check password isn't empty
        if(!size){
            throw std::runtime_error("Won't encrypt Secret with empty password");
        }

        decltype(&*begin) lBegin = &*begin;
        encryptSecretNL(lBegin, size);

        mEncryptPasswd.assign(begin, end);
    }

    /**
    * Decrypt this secret using the given password. It must be the
    * same password as was used to encrypt the secret.
    * @param passwd A password to use when decrypting this secret.
    */
    void decryptSecret(const impl::memory::SecureByteVec& passwd);

    template<typename Iter>
    void decryptSecret(Iter begin, Iter end){
        static_assert( sizeof(decltype(*begin)) == sizeof(uint8_t), "Wrong iterator type");

        std::lock_guard<std::mutex> lck { mMtx };
        auto size = std::distance(begin, end);
        // Check we have encrypted data to decrypt
        if(!mEncryptedData.size()){
            throw std::runtime_error("Can't decrypt unencrypted Secret");
        }

        decltype(&*begin) lBegin = &*begin;
        decryptSecretNL(lBegin, size);

        mEncryptPasswd.assign(begin, end);
    }


    /**
    * Is this secret encrypted?
    * @return True if encrypted, false otherwise.
    */
    bool encrypted() const;


    /* JSONSerialisable interface */

    /// Get a unique identifer for this JSONSerialisable type.
    const std::string jsonObjectType() const override;

    /// Construct a boost::property_tree representation of ourselves.
    boost::property_tree::ptree toJson(const SerialisationFormat& fmt) const override;

    /// Set ourselves from the given boost::property_tree.
    void fromJson(const boost::property_tree::ptree& root,
                  const SerialisationFormat& fmt) override;

  protected:

    /// Metadata about this secret.
    MetaDataCollectionSPtr mMetaData {};

  private:

    /// Encrypt (without taking lock)
    void encryptSecretNL(const uint8_t* passwd, const size_t size);
    /// Decrypt (without taking lock)
    void decryptSecretNL(const uint8_t* passwd, const size_t size);

    /// Setup AES key
    CryptoPP::SecByteBlock setupAESKey(const uint8_t* passwd, const size_t size);

    /// Pick a random name for this Secret.
    void pickRandomName();


    /// Mutex for locking
    mutable std::mutex mMtx {};

    /// Encryption types we use
    enum class EncryptionType
    {
        UNKNOWN = 0,
        CRYPTOPP_AES
    };

    /// The secret name
    std::string mName {};

    /// The secret value.
    impl::memory::SecureArray<uint8_t> mSecretData {};

    /// Our encrypted value
    impl::memory::SecureByteVec mEncryptedData {};
    impl::memory::SecureArray<uint8_t> mEncryptPasswd {};

    /// Flag to indicate we have registered with the JSON serialiser factory.
    /// Initialising this static actually triggers the registration.
    static bool SecretJSONRegistered;

};
}

#endif

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Implementation of a private key for ECSecp256k1.
 */

#ifndef _NCHAIN_SDK_KEY_ECSECP256K1_H_
#define _NCHAIN_SDK_KEY_ECSECP256K1_H_

#include <interface/RandomRangeGenerator.h>
#include "PubKeyECSecp256k1.h"
#include "KeyFragmentECSecp256k1.h"
#include "EccontextSecp256k1.h"

#include <interface/Key.h>
#include <impl/MetaDataDefinitions.h>

//#include <impl/memory/SecureVector.h>
//#include <impl/memory/SecureString.h>

#include <vector>
#include <atomic>

namespace nakasendo
{
    namespace native
    {
        /// Implementation for the Key class.
        template <class Seeder=RandutilsSeed>
        class KeyECSecp256k1 : public Key,
                               public EccontextSecp256k1
        {
            /// A fixed-size of the key (32 bytes).
            static constexpr impl::memory::SecureByteVec::size_type KEY_ECSECP256K1_SIZE = 32;
            /// A fixed-size of the serialized key (279 bytes).
            static constexpr impl::memory::SecureByteVec::size_type SERIALIZED_KEY_ECSECP256K1_SIZE = 279;

        public:
            /// Default constructor
            KeyECSecp256k1(); 

            /**
            * Construct a random key using a provided seed.
            * @param seed A seed for the random number generator.
            */
            KeyECSecp256k1(const impl::memory::SecureByteVec& seed);

            /**
            * Construct from an existing secret.
            * @param secret A secret to use as our private key.
            */
            KeyECSecp256k1(const Secret& secret);
            KeyECSecp256k1(Secret&& secret);

            /**
            * Recover the key from fragments.
            * @param keyFragments A vector of key fragments.
            */
            KeyECSecp256k1(const KeyFragments& keyFragments);

        private:
            /**
            * Check if a sequence of bytes meets secp256k1 requirements of a key.
            * @param vch A pointer to raw sequence of 256 bits.
            */
			bool checkSecKey(const unsigned char *vch) const;

            /**
            * Get a serialized private key (export in DER format), with all parameters included (279 bytes).
            * @return Serialized private key.
            */
            impl::memory::SecureByteVec getSerializedKey() const;

            /**
            * Get a deserialized (import in DER format) private key.
            * @return Deserialized private key.
            */
            impl::memory::SecureByteVec getDeserializedKey(const impl::memory::SecureByteVec& vSerializedSecret) const;

			/**
			* Recover the key from given fragments.
			* @param keyFragments A vector of key's fragments.
            * @return A vector of the recovered key.
			*/
            impl::memory::SecureByteVec recover(const KeyFragments& keyFragments);

            /**
            * Pick a name for our secret.
            */
            void nameSecret();

            /**
            * One time initialisation.
            */
            void init();

            /**
            * Generate a new private key.
            */
            void makeNewKey();

            /// A source of pseudo-random sequence of data (mutable because we don't
            /// want consuming entropy to mean our methods have to be non-const)
            mutable Seeder m_seeder;

            /// Flag to indicate we have registered with the JSON serialiser factory.
            /// Initialising this static actually triggers the registration.
            static bool KeyECSecp256k1JSONRegistered;

            /// The default ECSECP256K1 network all new keys are made for.
            static std::atomic<impl::MetaDataDefinitions::NetworkType> cDefaultNetworkType;

        public:
            // Key interface
            /**
            * Derive public key from private key.
            */
            virtual PubKeyPtr getPubKey() const override;
            virtual PubKeyPtr getPubKey(PubKey::Type pkType) const override;

			/**
			* Splits the Key into the requested number of fragments.
			* @param nSplitSharesN The absolute number of parts that the generated
			*     key should be split into.
			* @param nRecoverySharesM The minimum number of parts that need to be brought
			*     together in order to recovery the Key.
			* @return vector of created KeyFragment instances. If the split fails,
			*     throws an exception.
			*/
            virtual std::vector<KeyFragmentPtr> split(const int nSplitSharesN, const int nRecoverySharesM) const override;

            /**
            * Export ourselves in wallet import format (WIF).
            */
            impl::memory::SecureString exportAsWIF() const override;

            /**
            * Import ourselves from wallet import format (WIF).
            */
            void importFromWIF(const impl::memory::SecureString& wif) override;

            /**
            * Sign a message with this key.
            */
            std::vector<uint8_t> sign(const std::vector<uint8_t>& message) const override;

            /**
            * Decrypt a message previously encrypted by our public key.
            * @param message An encrypted message.
            * @return The decrypted message.
            */
            impl::memory::SecureByteVec decrypt(const std::vector<uint8_t>& message) const override;

            /**
            * Deterministically derives a new key from this key using the given
            * message.
            * @param message The message to be used in deterministically deriving
            * the new key.
            * @return A derived new key.
            */
            KeyPtr derive(const std::vector<uint8_t>& message) const override;

            /**
            * Produce a shared secret by using the other party's public key with
            * a given message and this private key. The result will be identical
            * to one produced by the other party, i.e. calculating with their
            * private key, our public key, and the same message.
            * @param pubKey The other party's public key.
            * @param msg The shared message.
            * @param secretName The name to give the derived shared secret. If not
            * specified, the SDK will pick a random name.
            * @return A shared secret.
            */
            SecretPtr sharedSecret(const PubKeyPtr&           pubKey,
                                   const std::vector<uint8_t>& message,
                                   const std::string&          secretName) const override;

            /**
            * Check if created key is valid in terms of secp256k1 specification.
            */
            bool isValidSecpKey() const;

            /**
            * Get network we are for.
            */
            impl::MetaDataDefinitions::NetworkType getNetworkType() const;

            /**
            * Set network we are for.
            */
            void setNetworkType(impl::MetaDataDefinitions::NetworkType networkType);

            /**
            * Get the compression type for this key.
            */
            impl::MetaDataDefinitions::CompressionType getCompressionType() const;

            /**
            * Set the compression type for this key.
            */
            void setCompressionType(impl::MetaDataDefinitions::CompressionType compressionType);

            /**
            * Set the default ECSECP256K1 network we use for all new keys.
            */
            static void setDefaultNetwork(impl::MetaDataDefinitions::NetworkType network);


            // JSONSerialisable interface

            /**
            * Get unique ID for this object type.
            * @return A unique string identifer for objects of this type.
            */
            const std::string jsonObjectType() const override;

            /**
            * Export ourselves to JSON.
            * @return A JSON reprenstation of ourselves.
            */
            boost::property_tree::ptree toJson(const SerialisationFormat& fmt) const override;

            /**
            * Set our state from JSON.
            * @param root A JSON property tree to set ourselves from.
            */
            void fromJson(const boost::property_tree::ptree& root,
                          const SerialisationFormat& fmt) override;

        };

    }
}

#include "KeyECSecp256k1T.h"

#endif // _NCHAIN_SDK_KEY_ECSECP256K1_H_

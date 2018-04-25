// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/**
 * Implementation of a public key for ECSecp256k1.
 */

#ifndef _NCHAIN_SDK_PUB_KEY_ECSECP256K1_H_
#define _NCHAIN_SDK_PUB_KEY_ECSECP256K1_H_

#include "EccontextSecp256k1.h"

#include <impl/memory/SecureVector.h>
#include <impl/utils/UInt.h>
#include <interface/PubKey.h>
#include <impl/MetaDataDefinitions.h>

#include <secp256k1/include/secp256k1.h>
#include <array>
#include <mutex>

namespace nakasendo 
{
    namespace native 
    {
        /// Implementation of serialized public key (including compressed and uncompressed version).
        class PubKeyECSecp256k1 : public PubKey,
                                  public EccontextSecp256k1
        {
            /// Size of X & Y components of public keys
            static constexpr size_t PUB_KEY_XY_COMPONENT_SIZE { 32 };
            /// Uncompressed public key size: Prefix(1byte) + Coordinates(64bytes = x(32bytes) + y(32bytes))
            static constexpr size_t UNCOMPRESSED_PUB_KEY_SIZE { 1 + (PUB_KEY_XY_COMPONENT_SIZE * 2) };
            /// Compressed public key size: Prefix(1byte) + Coordinates(32bytes = x(32bytes)) 
            static constexpr size_t COMPRESSED_PUB_KEY_SIZE { 1 + PUB_KEY_XY_COMPONENT_SIZE };
            /// A prefix for compressed key (0x02 - y is even, 0x03 - y is odd)
            static constexpr std::array<uint8_t, 2> COMPRESSED_PUB_KEY_PREFIX {0x02, 0x03};
            /// A prefix for uncompressed key (0x04 - pubkey is uncompressed, 0x06 - pubkey is hybrid even, 0x07 - pubkey is hybrid odd)
            static constexpr std::array<uint8_t, 3> UNCOMPRESSED_PUB_KEY_PREFIX {0x04, 0x06, 0x07};
 
        public:
            /// Create a public key from a given secret key (a valid public key in terms of secp256k1 specification).
            PubKeyECSecp256k1(const impl::MetaDataDefinitions::NetworkType& networkType,
                              PubKey::Type                                  pkType,
                              const impl::memory::SecureByteVec&                              secKey);

            /// Create public key and set it from the given raw bytes.
            PubKeyECSecp256k1(const std::vector<uint8_t>&                  bytes,
                              const impl::MetaDataDefinitions::NetworkType networkType =
                                    impl::MetaDataDefinitions::NetworkType::MAIN_NET);

        private:
            /// Make public key.
            void makePublicKey(PubKey::Type pkType, const impl::memory::SecureByteVec& secKey);
            /// Source coding
            void sourceCoding(PubKey::Type pkType);
            /// Check if the public key is valid.
            bool isValid() const;
            /// Serialize secp public key
            std::vector<uint8_t> serializeSecpPubKey(PubKey::Type pkType, const secp256k1_pubkey& secpPubKey) const;
            /// Get hash of the key.
            impl::utils::uint160 getHash160() const;

            /// Network type.
            const impl::MetaDataDefinitions::NetworkType m_networkType;
            /// Serialized public key.
            std::vector<uint8_t> m_vSerializedPubKey {};
            /// A mutex for thread safety modifications.
            mutable std::mutex m_mtx {};

        public:
            /// Public key interface.

            /**
            * Check if the public key is compressed. 
            */
            virtual bool isCompressed() const override;

            /**
            * Decompress the public key if it's compressed.
            */
            virtual void decompress() override;

            /**
            * Compress the public key if it's uncompressed.
            */
            virtual void compress() override;

            /**
            * Get a vector of public key.
            * @return A vector containing raw public key.
            */
            virtual std::vector<uint8_t> getContent() const override;

            /**
            * Get a public key as a hex string.
            * @return A hexadecimal string representing of public key.
            */
            virtual std::string getContentAsHexString() const override;

            /**
            * Derive a blockchain address from the public key.
            * @return A pointer to the blockchain address derived from the public key.
            */
            virtual BlockChainAddressPtr getBlockChainAddress() const override;

            /**
            * Verify a signature of a message.
            * @param message The message the signature should be for.
            * @param signature The signature.
            * @return True if we can verify the signature.
            */
            virtual bool verify(const std::vector<uint8_t>& message,
                                const std::vector<uint8_t>& signature) const override;

            /**
            * Encrypt the given message so it may only be decrypted by our
            * corresponding private key.
            * @param message The message to encrypt.
            * @return An encrypted message.
            */
            std::vector<uint8_t> encrypt(const impl::memory::SecureByteVec& message) const override;

            /**
            * Deterministically derives a new key from this key using the given
            * message.
            * @param message The message to be used in deterministically deriving
            * the new key.
            * @return A derived new key.
            */
            virtual PubKeyPtr derive(const std::vector<uint8_t>& message) const override;

        };

    } // end of namespace native
} // end of namespace nakasendo

#endif // _NCHAIN_SDK_PUB_KEY_ECSECP256K1_H_

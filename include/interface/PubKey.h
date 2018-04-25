// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/**
* Encapsulate a logical public key used within the SDK
*/

#ifndef _NCHAIN_SDK_PUB_KEY_H_
#define _NCHAIN_SDK_PUB_KEY_H_

#include <impl/memory/SecureVector.h>
#include "BlockChainAddress.h"

namespace nakasendo
{
    /// Forward declaration of pointer type
    class PubKey;
    /// Unique pointer type
    using PubKeyPtr = std::unique_ptr<PubKey>;
    /// Shared pointer type
    using PubKeySPtr = std::shared_ptr<PubKey>;

    /**
    * This class encapsulates a logical public key used within the SDK.
    */
    class PubKey
    {
    public:
        /// Public key can be compressed or uncompressed.
        enum class Type { COMPRESSED, UNCOMPRESSED };
        /// Defualt destructor
        virtual ~PubKey() = default;

    public:
        /**
        * Check if the public key is compressed.
        */
        virtual bool isCompressed() const = 0;

        /**
        * Decompress the public key if it's compressed.
        */
        virtual void decompress() = 0;

        /**
        * Compress the public key if it's uncompressed.
        */
        virtual void compress() = 0;

        /**
        * Get a vector of public key.
        * @return A vector containing raw public key.
        */
        virtual std::vector<uint8_t> getContent() const = 0;

        /**
        * Get a public key as a hex string.
        * @return A hexadecimal string representing of public key.
        */
        virtual std::string getContentAsHexString() const = 0;

        /**
        * Derive a blockchain address from the public key.
        * @return A pointer to the blockchain address derived from the public key.
        */
        virtual BlockChainAddressPtr getBlockChainAddress() const = 0;

        /**
        * Verify a signature of a message.
        * @param message The message the signature should be for.
        * @param signature The signature.
        * @return True if we can verify the signature.
        */
        virtual bool verify(const std::vector<uint8_t>& message,
                            const std::vector<uint8_t>& signature) const = 0;

        /**
        * Encrypt the given message so it may only be decrypted by our
        * corresponding private key.
        * @param message The message to encrypt.
        * @return An encrypted message.
        */
        virtual std::vector<uint8_t> encrypt(const impl::memory::SecureByteVec& message) const = 0;

        /**
        * Deterministically derives a new key from this key using the given
        * message.
        * @param message The message to be used in deterministically deriving
        * the new key.
        * @return A derived new key.
        */
        virtual PubKeyPtr derive(const std::vector<uint8_t>& message) const = 0;

    };

} // end of namespace nakasendo

#endif // _NCHAIN_SDK_PUB_KEY_H_

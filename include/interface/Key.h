// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Encapsulate a logical private key used within the SDK.
 */

#ifndef _NCHAIN_SDK_KEY_H_
#define _NCHAIN_SDK_KEY_H_

#include "ContextObject.h"
#include "MetaDataCollection.h"
#include "Types.h"
#include "PubKey.h"
#include "KeyFragment.h"
#include "Secret.h"

#include <impl/memory/SecureString.h>

#include <vector>
#include <memory>

namespace nakasendo
{
    /// Forward declaration of Key pointer type
    class Key;
    /// Unique pointer type
    using KeyPtr = std::unique_ptr<Key>;
    /// Shared pointer type
    using KeySPtr = std::shared_ptr<Key>;

    /**
    * This class encapsulates a logical key used within the SDK.
    */
    class Key : public Secret
    {
    public:

        /// Default constructor.
        Key() = default;

        /// Construct from an existing Secret.
        Key(const Secret& secret) : Secret{secret} {}

        /// Move construct from an existing Secret.
        Key(Secret&& secret) : Secret{std::move(secret)} {}

        /// Default destructor
        virtual ~Key() = default;

        /**
        * Derive public key from private key.
        * @return A pointer to a new public key.
        */
        virtual PubKeyPtr getPubKey() const = 0;

        /**
        * Derive public key of the requested type from private key.
        * @param pkType Generate a COMPRESSED or UNCOMPRESSED public key.
        * @return A pointer to a new public key.
        */
        virtual PubKeyPtr getPubKey(PubKey::Type pkType) const = 0;

        /**
        * Splits the Key into the requested number of fragments.
        * @param nSplitSharesN The absolute number of parts that the generated
        *     key should be split into.
        * @param nRecoverySharesM The minimum number of parts that need to be brought
        *     together in order to recovery the Key.
        * @return vector of created KeyFragment instances. If the split fails,
        *     throws an exception.
        */
        virtual std::vector<KeyFragmentPtr> split(const int nSplitSharesN=3, const int nRecoverySharesM=2) const = 0;

        /**
        * Export ourselves in wallet import format (WIF).
        * @return A WIF encoded string.
        */
        virtual impl::memory::SecureString exportAsWIF() const = 0;

        /**
        * Import ourselves from wallet import format (WIF).
        * @param wif A WIF encoded string.
        */
        virtual void importFromWIF(const impl::memory::SecureString& wif) = 0;

        /**
        * Sign a message with this key.
        * @param message the message to sign as a byte array.
        * @return The signed message.
        */
        virtual std::vector<uint8_t> sign(const std::vector<uint8_t>& message) const = 0;

        /**
        * Decrypt a message previously encrypted by our public key.
        * @param message An encrypted message.
        * @return The decrypted message.
        */
        virtual impl::memory::SecureByteVec decrypt(const std::vector<uint8_t>& message) const = 0;

        /**
        * Deterministically derives a new key from this key using the given
        * message.
        * @param message The message to be used in deterministically deriving
        * the new key.
        * @return A derived new key.
        */
        virtual KeyPtr derive(const std::vector<uint8_t>& message) const = 0;

        /**
        * Produce a shared secret by using the other party's public key with
        * a given message and this private key. The result will be identical
        * to one produced by the other party, i.e. calculating with their
        * private key, our public key, and the same message.
        * @param pubKey The other party's public key.
        * @param message The shared message.
        * @param secretName The name to give the derived shared secret. If not
        * specified, the SDK will pick a random name.
        * @return A shared secret.
        */
        virtual SecretPtr sharedSecret(const PubKeyPtr&            pubKey,
                                       const std::vector<uint8_t>& message,
                                       const std::string&          secretName = {}) const = 0;

    };

}

#endif // _NCHAIN_SDK_KEY_H_


// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/**
* Encapsulate a logical key fragment used within the SDK
*/

#ifndef _NCHAIN_SDK_KEY_FRAGMENT_H_
#define _NCHAIN_SDK_KEY_FRAGMENT_H_

#include "Secret.h"
#include <interface/Types.h>

namespace nakasendo
{
    /// Forward declaration of KeyFragment
    class KeyFragment;
    /// Unique pointer type
    using KeyFragmentPtr = std::unique_ptr<KeyFragment>;
    /// Shared pointer type
    using KeyFragmentSPtr = std::shared_ptr<KeyFragment>;
    /// A list of KeyFragments
    using KeyFragments = std::vector<std::reference_wrapper<KeyFragment>>; // non-owner

    /// Encapsulate a logical key fragment used within the SDK
    class KeyFragment : public Secret
    {
    public:
        /// Default constructor.
        KeyFragment() = default;

        /// Move construct from an existing Secret.
        KeyFragment(Secret&& secret) : Secret{std::move(secret)} {}

        /// Default destructor
        virtual ~KeyFragment() = default;

    public:

        /**
        * Get public key as string.
        * @return Public key as hex string.
        */
        virtual std::string getPubKeyAsHexString() const = 0;

    };

} // end of namespace nakasendo

#endif //_NCHAIN_SDK_KEY_FRAGMENT_H_

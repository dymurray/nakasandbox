// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREALLOCATIONACTION_H
#define SECUREALLOCATIONACTION_H

#include <cstddef>

namespace nakasendo { namespace impl { namespace memory {
        /**
         * @brief The SecureAllocatorAction struct is a set of generic callback actions that are
         * being called from various stages of secure memory management classes
         */
        struct SecureAllocatorAction
        {
            template<typename... Args>
            static size_t checkSize(Args...) noexcept;

            template<typename... Args>
            static bool execOnAlloc(Args...);

            template<typename... Args>
            static bool execOnDeAlloc(Args...);

            template<typename... Args>
            static bool execOnError(Args...);

            template<typename... Args>
            static bool execOnUnlock(Args...);
        };
} } }
#endif // SECUREALLOCATIONACTION_H

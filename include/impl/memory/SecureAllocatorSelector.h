// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREALLOCATORSELECTOR_H
#define SECUREALLOCATORSELECTOR_H

namespace nakasendo {
    namespace impl {

        enum class SecureAllocatorSelector{
            NO_ACTION = 10,
            DEBUG,
            RELEASE
        };
    }
}


#endif // SECUREALLOCATORSELECTOR_H

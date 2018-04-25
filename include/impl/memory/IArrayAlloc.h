// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef IARRAYALLOC_H
#define IARRAYALLOC_H

#include <cstddef>

namespace nakasendo{ namespace impl{ namespace memory{

class IArrayAlloc{
public:
    virtual void* byteAlloc(size_t n) = 0;
    virtual void* byteAlloc(size_t n, void* allocated) = 0;
    virtual void byteFree(void* ptr) = 0;
    virtual void byteFree(void* ptr, void* allocated) = 0;
    virtual size_t size(void* ptr) const noexcept = 0;
    virtual void getException() = 0;

    virtual ~IArrayAlloc(){}
};
} } }
#endif // SECUREUNIQUEPTR_H

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREARRAYALLOC_H
#define SECUREARRAYALLOC_H

#include <impl/memory/IArrayAlloc.h>

namespace nakasendo{ namespace impl{ namespace memory{

class SecureArrayAlloc : public IArrayAlloc {
    // ArrayAlloc interface
public:
    void *byteAlloc(size_t n) override;
    void *byteAlloc(size_t n, void *allocated) override;
    void byteFree(void *ptr) override;
    void byteFree(void *ptr, void *allocated) override;
    size_t size(void* ptr) const noexcept override;
    void getException() override;
};

} } }
#endif // SECUREARRAYALLOC_H

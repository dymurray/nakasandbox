// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef ARRAYALLOC_H
#define ARRAYALLOC_H

#include <impl/memory/IArrayAlloc.h>

namespace nakasendo{ namespace impl{ namespace memory{

class ArrayAlloc : public IArrayAlloc{
    // IArrayAlloc interface
public:
    ArrayAlloc() = default;

    ArrayAlloc(const ArrayAlloc& other): mSize(other.mSize){}
    ArrayAlloc(ArrayAlloc&& other): mSize(other.mSize){ other.mSize = 0; }

    ArrayAlloc& operator=(const ArrayAlloc& other);
    ArrayAlloc& operator=(ArrayAlloc&& other);

    virtual ~ArrayAlloc(){}

    void *byteAlloc(size_t n) override;
    void *byteAlloc(size_t n, void *allocated) override;
    void byteFree(void *ptr) override;
    void byteFree(void *ptr, void *allocated) override;
    inline size_t size(void* ptr = nullptr) const noexcept override{
        return mSize;
    }
    void getException() override;

private:
    size_t mSize = 0;
};

} } }
#endif // ARRAYALLOC_H

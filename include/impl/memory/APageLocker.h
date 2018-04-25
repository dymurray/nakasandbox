// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef ALOCKEDPAGEMANAGER_H
#define ALOCKEDPAGEMANAGER_H

#include <impl/utils/Error.h>

#include <cstdint>
#include <string>

namespace nakasendo { namespace impl { namespace memory {

class APageLocker
{
public:
    virtual ~APageLocker() {}
    /** Allocate and lock memory pages.
     * If len is not a multiple of the system page size, it is rounded up.
     * Returns 0 in case of allocation failure.
     *
     * If locking the memory pages could not be accomplished it will still
     * return the memory, however the lockingSuccess flag will be false.
     * lockingSuccess is undefined if the allocation fails.
     */
    virtual utils::Error lockPage(void* ptrInPage) = 0;

    /** Unlock and free memory pages.
     * Clear the memory before unlocking.
     */
    virtual utils::Error freePage(void* ptrPage) = 0;

    /** Get the total limit on the amount of memory that may be locked by this
     * process, in bytes. Return size_t max if there is no limit or the limit
     * is unknown. Return 0 if no memory can be locked at all.
     */
    virtual size_t getLimit() const = 0;
    virtual utils::Error setLimit(size_t expectedLimit) = 0;

    /** Get the page size in bytes
     */
    size_t pageSize()const { return mPageSize; }

    /**
     * Get the start absolute address of the page
     */
    uintptr_t pageStart(const void* ptr) const{
        const uintptr_t baseAddr = reinterpret_cast<const uintptr_t>(ptr);
        return baseAddr & mPageMask;
    }

    /**
     * Get the end absolute address of the page
     */
    uintptr_t pageEnd(const void* ptr, const size_t len) const{
        const uintptr_t baseAddr = reinterpret_cast<const uintptr_t>(ptr);
        return (baseAddr + len - 1) & mPageMask;
    }

protected:
    size_t mPageMask = 0;
    size_t mPageSize = 0;
};

} } }
#endif // ALOCKEDPAGEMANAGER_H

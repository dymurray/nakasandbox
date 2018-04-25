// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef PAGELOCKER_H
#define PAGELOCKER_H

#ifdef WIN32
  #define DllExport   __declspec( dllexport )  
#else
  #define DllExport  
#endif // WIN32

#include <impl/utils/FNV1aHash.h>
#include <impl/utils/Error.h>
#include <impl/utils/Status.h>
#include <impl/containers/ThreadSafeHash.h>

#include <impl/memory/APageLocker.h>

#include <thread>
#include <mutex>
#include <set>

bool DllExport setLockerLimit(size_t limit);

namespace nakasendo { namespace impl { namespace memory {

/**
 * @brief The PageLockerManager class is responsible for locking pages in memory in order to avoid their possible
 * offload to a swap file. It keeps a thread safe counter of memory pages blocked togther with informaotion about
 * how many times the memory page was requested to remain in memory
 */
class PageLockerManager
{
    /**
     * @brief mStatusBase Unique #Status message code base
     */
    static constexpr uint32_t mStatusBase = utils::Hash::fnv1a32("PageLockerManager");
public:
    static constexpr utils::Status SUCCESS_LOCKED      { mStatusBase,       "Page lock succeeded" };
    static constexpr utils::Status SUCCESS_UNLOCKED    { mStatusBase + 1,   "Page unlock succeeded" };
    static constexpr utils::Status COUNTER_INCREMENTED { mStatusBase + 2,   "Page is already locked, counter incremented" };
    static constexpr utils::Status COUNTER_DECREMENTED { mStatusBase + 3,   "Page is already locked, counter decremented" };
    static constexpr utils::Status NO_LOCK_REQUIRED    { mStatusBase + 4,   "No page lock required" };
    static constexpr utils::Status NO_UNLOCK_REQUIRED  { mStatusBase + 5,   "No page unlock required" };
    static constexpr utils::Status NO_PAGE_ALLOCATOR        { mStatusBase + 6,   "No page allocator found" };

public:
    /**
     * @brief Instance Running instance of #PageLockerManager. Implements singleton pattern
     * @return Pointer to #PageLockerManager instance
     */
    static PageLockerManager* Instance()
    {
        std::call_once(PageLockerManager::mInitFlag, PageLockerManager::CreateInstance);
        return PageLockerManager::mInstance;
    }

    /**
     * @brief lockRange Lock previously  allocated memory range
     * @param ptr Range start
     * @param size Range size
     * @return True if success
     */
    bool lockRange(void* ptr, const size_t size);

    /**
     * @brief unLockRange Unlock previously  locked memory range
     * @param ptr Range start
     * @param size Range size
     * @return True if success
     */
    bool unLockRange(void *ptr, const size_t size);

    /**
     * @brief lockRangeEx Extended version of #lockRange. Returns map of memory pages locked and a #Status message for each page
     * @param ptr Range start
     * @param size Range size
     * @return Map of memory pages locked and a #Status message for each page
     */
    std::map<uintptr_t, utils::Status> lockRangeEx(void* ptr, const size_t size);

    /**
     * @brief unLockRangeEx Extended version of #unLockRange. Returns map of memory pages locked and a #Status message for each page
     * @param ptr Range start
     * @param size Range size
     * @return Map of memory pages locked and a #Status message for each page
     */
    std::map<uintptr_t, utils::Status> unLockRangeEx(void *ptr, const size_t size);

    /**
     * @brief lastError Last system error message for the calling thread
     * @return Error message or empty string if there was no error
     */
    utils::Error lastError() const noexcept;

    /**
     * @brief lockedPages Number of locked memory pages
     * @return Number of locked pages
     */
    size_t lockedPages() const noexcept;

    /**
     * @brief lockedBlocks
     * @return
     */
    size_t lockedBlocks() const noexcept;

    /**
     * @brief lockedBytes Total size of locked memory in bytes aggregating data from all locked memory pages
     * @return Size of locked memory in bytes
     */
    size_t lockedBytes() const;

    /**
     * @brief getLimit Upper limit of the memory which OS allows to lock. Depends on OS and process permissions
     * @return Size in locable memory bytes
     */
    size_t getLimit() const;

    /**
     * @brief getPageSize Virtual memory page size
     * @return Memory page size
     */
    size_t getPageSize()const;

    /**
     * @brief setLimit Set upper limit of the memory which OS allows to lock. Depends on OS and process permissions
     * @return True if operation was successful
     */
    bool setLimit( size_t limit );

    /**
     * @brief isLocked Tests if memory location belongs to a locked memory block
     * @param ptr Pointer to memory location
     * @return Size of the allocated memory memory block if success otherwise 0
     */
    size_t isLocked(const void* ptr)const noexcept;

private:
    explicit PageLockerManager(std::unique_ptr<APageLocker> allocator);

    static void CreateInstance();                                           /**< Create a new PageLocker specialized to the OS */
    static bool LockingFailed();                                            /**< Called when locking fails, warn the user here */

    bool tryLock(void* ptr,
                   const size_t size ,
                   std::vector<uintptr_t> &pages);

    utils::Status registerLockRange(const uintptr_t page);                    /**< Internal range locker helper method */
    utils::Status registerUnlockRange(const uintptr_t page);                  /**< Internal range unlocker helper method */


    utils::Status setLastError(utils::Error error) const;

	 static PageLockerManager* mInstance;                                      /**< Global instance of PageLockerManager */
	 static std::once_flag mInitFlag;                                          /**< Initialisation flag */

    std::unique_ptr<APageLocker> mPageLocker;                                 /**< Pointer to concrete allocator */

    using LockedMemPages =
      containers::ThreadSafeHash<uintptr_t, size_t,
                                 containers::ThreadSafeHashTypes::HITS_COUNTER>;   /**< Type of locked pages counter */

    LockedMemPages mLockedPages;                                               /**< Instance of locked pages counter */


    using LockedMemBlocks =
      containers::ThreadSafeHash<uintptr_t, size_t,
                                 containers::ThreadSafeHashTypes::GENERAL>;
    LockedMemBlocks mLockedBlocks;

    using ThreadError = containers::ThreadSafeHash<std::thread::id, utils::Error>;
    mutable ThreadError mLastError;                                            /**< Strings containing the last error - one per thread */

    std::function<bool(uintptr_t/*page*/)>  mActionOnLocked;                   /**< Called when page is locked */
    std::function<bool(uintptr_t/*page*/)>  mActionOnUnlocked;                 /**< Called when page is unlocked */

    std::mutex mLock;
};

} } }
#endif // PAGELOCKER_H

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef ARRAY_H
#define ARRAY_H

#include <impl/memory/TArray.h>
#include <impl/memory/ArrayAlloc.h>

namespace nakasendo{ namespace impl{ namespace memory{

template<typename T>
using Array = TArray<T, ArrayAlloc>;

/**
 * @brief makeSecureUnique Unique pointer that owns and manages memory block allocated by #mAlloc.byteAlloc
 * @param size number of elements of type T
 * @return unique_ptr of type T to #Array of #size elements
 * @throw any exception from #Array
 */
template <typename T>
using ArrayUniquePtr = std::unique_ptr<Array<T>>;
template< class T>
ArrayUniquePtr<T> makeSecureUnique( size_t size ){
    return std::unique_ptr<Array<T>>(new Array<T>(size));
}

/**
 * @brief makeSecureUnique Unique pointer that owns and manages memory block allocated by #mAlloc.byteAlloc placement version
 * @param size number of elements of type T
 * @param allocMem pointer to previoulsy allocate memeory block
 * @return unique_ptr of type T to #Array of #size elements
 * @throw any exception from #Array
 */
template <typename T>
using ArrayUniquePtr = std::unique_ptr<Array<T>>;
template< class T>
ArrayUniquePtr<T> makeSecureUnique( size_t size, T* allocMem ){
    return std::unique_ptr<Array<T>>(new Array<T>(size, allocMem));
}

/**
 * @brief makeSecureShared Shared pointer that owns and manages memory block allocated by #mAlloc.byteAlloc
 * @param size number of elements of type T
 * @return shared_ptr of type T to #Array of #size elements
 * @throw any exception from #Array
 */
template <typename T>
using ArraySharedPtr = std::shared_ptr<Array<T>>;
template< class T>
ArraySharedPtr<T> makeSecureShared( size_t size ){
    return std::shared_ptr<Array<T>>(new Array<T>(size));
}

/**
 * @brief makeSecureShared Shared pointer that owns and manages memory block allocated by #mAlloc.byteAlloc placement version
 * @param size number of elements of type T
 * @param allocMem pointer to previoulsy allocate memeory block
 * @return shared_ptr of type T to #Array of #size elements
 * @throw any exception from #Array
 */
template <typename T>
using ArraySharedPtr = std::shared_ptr<Array<T>>;
template< class T>
ArraySharedPtr<T> makeSecureShared( size_t size, T* allocMem ){
    return std::shared_ptr<Array<T>>(new Array<T>(size, allocMem));
}

/**
 * Alias for weak_ptr of #Array
 */
template <typename T>
using ArrayWeakPtr = std::weak_ptr<Array<T>>;
} } }
#endif // ARRAY_H

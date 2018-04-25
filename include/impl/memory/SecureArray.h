// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREARRAY_H
#define SECUREARRAY_H

#include <impl/memory/TArray.h>
#include <impl/memory/SecureArrayAlloc.h>

namespace nakasendo{ namespace impl{ namespace memory{

template<typename T>
using SecureArray = TArray<T, SecureArrayAlloc>;

/**
 * @brief makeSecureUnique Unique pointer that owns and manages memory block allocated by #mAlloc.byteAlloc
 * @param size number of elements of type T
 * @return unique_ptr of type T to #SecureArray of #size elements
 * @throw any exception from #SecureArray
 */
template <typename T>
using SecureArrayUniquePtr = std::unique_ptr<SecureArray<T>>;
template< class T>
SecureArrayUniquePtr<T> makeSecureUnique( size_t size ){
    return std::unique_ptr<SecureArray<T>>(new SecureArray<T>(size));
}

/**
 * @brief makeSecureUnique Unique pointer that owns and manages memory block allocated by #mAlloc.byteAlloc placement version
 * @param size number of elements of type T
 * @param allocMem pointer to previoulsy allocate memeory block
 * @return unique_ptr of type T to #SecureArray of #size elements
 * @throw any exception from #SecureArray
 */
template <typename T>
using SecureArrayUniquePtr = std::unique_ptr<SecureArray<T>>;
template< class T>
SecureArrayUniquePtr<T> makeSecureUnique( size_t size, T* allocMem ){
    return std::unique_ptr<SecureArray<T>>(new SecureArray<T>(size, allocMem));
}

/**
 * @brief makeSecureShared Shared pointer that owns and manages memory block allocated by #mAlloc.byteAlloc
 * @param size number of elements of type T
 * @return shared_ptr of type T to #SecureArray of #size elements
 * @throw any exception from #SecureArray
 */
template <typename T>
using SecureArraySharedPtr = std::shared_ptr<SecureArray<T>>;
template< class T>
SecureArraySharedPtr<T> makeSecureShared( size_t size ){
    return std::shared_ptr<SecureArray<T>>(new SecureArray<T>(size));
}

/**
 * @brief makeSecureShared Shared pointer that owns and manages memory block allocated by #mAlloc.byteAlloc placement version
 * @param size number of elements of type T
 * @param allocMem pointer to previoulsy allocate memeory block
 * @return shared_ptr of type T to #SecureArray of #size elements
 * @throw any exception from #SecureArray
 */
template <typename T>
using SecureArraySharedPtr = std::shared_ptr<SecureArray<T>>;
template< class T>
SecureArraySharedPtr<T> makeSecureShared( size_t size, T* allocMem ){
    return std::shared_ptr<SecureArray<T>>(new SecureArray<T>(size, allocMem));
}

/**
 * Alias for weak_ptr of #SecureArray
 */
template <typename T>
using SecureArrayWeakPtr = std::weak_ptr<SecureArray<T>>;
} } }
#endif // SECUREARRAY_H

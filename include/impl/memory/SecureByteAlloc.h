// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREBYTEALLOC_H
#define SECUREBYTEALLOC_H

#include <cstdlib>
#include <memory>

namespace nakasendo{ namespace impl{ namespace memory{

/**
 * @brief byteAlloc Allocates requested number of bytes as a conventional memory
 * @param n number of bytes to allocate
 * @return non-null pointer to suitably aligned memory of size at least n
 * @throw any exception that could be thrown by operator new
 */
extern void* byteAlloc(size_t n);

/**
 * @brief byteFree Deallocates storage previously allocated by a matching #byteAlloc(size_t n)
 * @param ptr pointer to a memory block to deallocate or a null pointer
 */
extern void byteFree(void* ptr) noexcept;

/**
 * @brief byteAlloc Allocates requested number of bytes from preacllocated conventional memory block
 * @param n number of bytes to allocate
 * @param stage pointer to a memory area to allocate bytes at
 * @return non-null pointer to suitably aligned memory of size at least n
 * @throw any exception that could be thrown by operator new
 */
extern void *byteAlloc(size_t n, void* stage);

/**
 * @brief byteFree Deallocates storage previously allocated by a matching #byteAlloc(size_t n, void* ptr)
 * @param ptr pointer to a memory block to deallocate or a null pointer
 * @param stage pointer to a memory area where bytes are allocated
 */
extern void byteFree(void* volatile ptr, void* stage) noexcept;

/**
 * @brief secureByteAlloc Allocates memory block and register it with #PageLockerManager
 * as non swappable in order to avoid memory block being placed in a swap file
 * @param n Required Memory block size in bytes
 * @return Pointer to allocated memory block
 * @throw bad_alloc() if memory allocation fails or secure_bad_alloc() if locking memory block fails
 */
extern void *secureByteAlloc(size_t n);

/**
 * @brief secureByteAlloc Placement allocator .Allocates memory block and register it with #PageLockerManager
 * as non swappable in order to avoid memory block being placed in a swap file
 * @param n Required Memory block size in bytes
 * @param stage Required Pointer to previously allocated memory block. If size of memory block pointed by #ptr is
 * less then #n then behavior undefined
 * @return Pointer to allocated memory block
 * @throw bad_alloc() if ptr is null or secure_bad_alloc() if locking memory block fails
 */
extern void *secureByteAlloc(size_t n, void* stage);

/**
 * @brief secureByteFree Releases memory block previously allocated by #secureByteAlloc.
 * The memory content is wiped before the release
 * @param ptr Pointer to a memory block
 * @param n Block size
 */
extern void secureByteFree( void* volatile ptr) noexcept;

/**
 * @brief secureByteFree Releases memory block previously allocated by placement #secureByteAlloc.
 * The memory content is wiped before the release
 * @param ptr Pointer to a memory block
 * @param stage Pointer to a preallocated memory block
 * @param n Block size
 */
extern void secureByteFree(void* volatile ptr, void* stage) noexcept;

/**
 * @brief toUnsecure Converts memory allocated by #secureByteAlloc() to a conventional memory which
 * can be freed by a corresponding #byteFree
 * @param ptr Pointer to a memory block
 */
extern void convertToUnsecure(void* ptr) noexcept;

/**
 * @brief secureCheckSize
 * @param ptr
 * @return
 */
extern size_t secureCheckSize(void* ptr) noexcept;

} } }
#endif // SECUREUNIQUEPTR_H

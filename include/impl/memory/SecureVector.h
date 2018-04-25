// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREVECTOR_H
#define SECUREVECTOR_H

#include <impl/memory/SecureAllocator.h>

#include <vector>

namespace nakasendo{ namespace impl{ namespace memory{

template<typename T>
using SecureVec = std::vector<T, SecureAllocator<T>>;

using SecureByteVec = SecureVec<uint8_t>;
using CSecureByteVec = const SecureVec<uint8_t>;
using SecureUIntVec = SecureVec<uint32_t>;
using CSecureUIntVec = const SecureVec<uint32_t>;

//template<typename T>
//using SecureVecIter = typename std::vector<T, SecureAllocator<T>>::iterator;

using SecureByteVecIter = SecureByteVec::iterator;
using CSecureByteVecIter = CSecureByteVec::iterator;
using SecureUIntVecIter = SecureUIntVec::iterator;

template <typename T>
using is_vector = std::is_same<T, std::vector< typename T::value_type,
                                               typename T::allocator_type > >;

} } }

#endif // SECUREVECTOR_H

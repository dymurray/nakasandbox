// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.


#ifndef _NCHAIN_SDK_TYPES_H_
#define _NCHAIN_SDK_TYPES_H_

#include <vector>
#include <string>

#include <impl/memory/SecureArray.h>
#include <impl/memory/SecureAllocator.h>

//#include <support/allocators/secure.h>

namespace nakasendo
{

//using SecureVec = std::vector<uint8_t, impl::memory::SecureAllocator<uint8_t>>;
//using SecureVecUINT32 = std::vector<uint32_t, impl::memory::SecureAllocator<uint32_t>>;
//using SecureVecIter = SecureVec::iterator;
using SecureOstringStream = std::basic_ostringstream<char, std::char_traits<char>, impl::memory::SecureAllocator<char>>;

template <typename T>
using is_vector = std::is_same<T, std::vector< typename T::value_type,
                                               typename T::allocator_type > >;
}

#endif

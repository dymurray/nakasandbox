// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECURESTRING_H
#define SECURESTRING_H

#include <impl/memory/SecureAllocator.h>

#include <string>

namespace nakasendo{ namespace impl{ namespace memory{

template<typename T>
using SecureBasicString = std::basic_string<T, std::char_traits<T>, SecureAllocator<T>>;

using SecureString = std::basic_string<char, std::char_traits<char>, SecureAllocator<char>>;

} } }

#endif // SECURESTRING_H

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Generic utility functions that are small enough not to warrant
 * their own dedicated files.
 */

#ifndef _NCHAIN_SDK_UTILS_H_
#define _NCHAIN_SDK_UTILS_H_

#include <type_traits>

namespace nakasendo { namespace impl { namespace utils
{

/// Convert an enum class value to its underlying type (for example, to print it)
template <typename Enumeration>
auto EnumAsUnderlying(Enumeration value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

}}}

#endif

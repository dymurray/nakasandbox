// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Long (256 and 160 bit) unsigned int class.
 */

#ifndef _NCHAIN_SDK_UINT_H_
#define _NCHAIN_SDK_UINT_H_

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <string.h>

namespace nakasendo { namespace impl { namespace utils {

/// Long (256 and 160 bit) unsigned int class.
template<unsigned int Bits>
class UIntArray
{
  public:
    /// Constructor
    UIntArray(const std::vector<uint8_t>& bytes)
    {
        // Check sizes
        if(bytes.size() != sizeof(mBytes))
        {   
            throw std::invalid_argument("Bad size for UintArray " + std::to_string(bytes.size()));
        }

        memcpy(mBytes, bytes.data(), sizeof(mBytes));
    }

    /// Get iterator to start
    const uint8_t* begin() const { return &mBytes[0]; }

    /// Get size
    size_t size() const { return sizeof(mBytes); }

    /// Return as a vector
    std::vector<uint8_t> toVector() const
    {
        return std::vector<uint8_t> {begin(), begin() + size()};
    }

    /// Equality
    bool operator==(const UIntArray& that) const
    {
        return (sizeof(mBytes) == sizeof(that.mBytes)) &&
               (memcmp(mBytes, that.mBytes, sizeof(mBytes)) == 0);
    }

    /// Non-equality
    bool operator!=(const UIntArray& that) const
    {
        return !(*this == that);
    }

  private:

    /// Underlying array
    uint8_t mBytes[Bits/8] {};

};

/// uint256 type
using uint256 = UIntArray<256>;

/// uint160 type
using uint160 = UIntArray<160>;

}}}

#endif

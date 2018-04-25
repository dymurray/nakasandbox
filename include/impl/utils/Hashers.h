// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Useful hashing functions.
 */

#ifndef _NCHAIN_SDK_BITCOIN_CASH_HASHERS_H_
#define _NCHAIN_SDK_BITCOIN_CASH_HASHERS_H_

#include "UInt.h"

#include <iterator>

#include <cryptopp/sha.h>
#include <cryptopp/ripemd.h>

namespace nakasendo { namespace impl { namespace utils
{

/// Bitcoin cash style double SHA-256
template<typename T>
uint256 Hash256(const T begin, const T end)
{
    // Make sure we never dereference an end() iterator
    auto dist = std::distance(begin, end);
    const uint8_t* byte { dist? reinterpret_cast<const uint8_t*>(&begin[0]) : nullptr };

    // Hash once
    std::vector<uint8_t> hash1(CryptoPP::SHA256::DIGESTSIZE, 0);
    CryptoPP::SHA256().CalculateDigest(hash1.data(), byte, dist);

    // Hash twice
    std::vector<uint8_t> hash2(CryptoPP::SHA256::DIGESTSIZE, 0);
    CryptoPP::SHA256().CalculateDigest(hash2.data(), hash1.data(), hash1.size());

    // Return result
    return uint256 {hash2};
}

/// Bitcoin cash style SHA-256 + RIPEMD-160
template<typename T>
uint160 Hash160(const T begin, const T end)
{
    // Make sure we never dereference an end() iterator
    auto dist = std::distance(begin, end);
    const uint8_t* byte { dist? reinterpret_cast<const uint8_t*>(&begin[0]) : nullptr };

    // Hash with SHA-256
    std::vector<uint8_t> hash1(CryptoPP::SHA256::DIGESTSIZE, 0);
    CryptoPP::SHA256().CalculateDigest(hash1.data(), byte, dist);

    // Hash with RIPEMD-160
    std::vector<uint8_t> hash2(CryptoPP::RIPEMD160::DIGESTSIZE, 0);
    CryptoPP::RIPEMD160().CalculateDigest(hash2.data(), hash1.data(), hash1.size());

    // Return result
    return uint160 {hash2};
}

}}}

#endif

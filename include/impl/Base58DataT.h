// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Handle the encoding and decoding of Base58 and Base58Check data.
 */

#include <impl/utils/Hashers.h>

namespace nakasendo { namespace impl 
{

namespace
{
    const char* Base58Alphabet { "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz" };
}

// Encode our raw data as a Base58 string.
// Functionality lifted from Bitcoin ABC.
template <class V, class S>
auto Base58Data::encode(const typename std::enable_if<memory::is_vector<V>::value, V>::type& data)
        -> typename std::enable_if<std::is_same<S, std::string>::value || std::is_same<S, memory::SecureString>::value, S>::type
{
    const uint8_t* pbegin { data.data() };
    const uint8_t* pend { pbegin + data.size() };

    // Skip & count leading zeroes.
    int zeroes {0};
    int length {0};
    while(pbegin != pend && *pbegin == 0)
    {
        ++pbegin;
        ++zeroes;
    }

    // Allocate enough space in big-endian base58 representation.
    // log(256) / log(58), rounded up.
    int size { static_cast<int>((pend - pbegin) * 138 / 100 + 1) };
    std::vector<uint8_t> b58(size);
    // Process the bytes.
    while(pbegin != pend)
    {
        int carry { *pbegin };
        int i {0};
        // Apply "b58 = b58 * 256 + ch".
        for(std::vector<uint8_t>::reverse_iterator it = b58.rbegin();
                (carry != 0 || i < length) && (it != b58.rend()); it++, i++)
        {
            carry += 256 * (*it);
            *it = carry % 58;
            carry /= 58;
        }

        length = i;
        ++pbegin;
    }

    // Skip leading zeroes in base58 result.
    std::vector<uint8_t>::iterator it { b58.begin() + (size - length) };
    while(it != b58.end() && *it == 0)
    {
        ++it;
    }

    // Translate the result into a string.
    S str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, '1');
    while(it != b58.end())
    {
        str += Base58Alphabet[*(it++)];
    }
    
    return str;
}

// Encode our raw data and version bytes in Base58Checked format.
template <class V, class S>
auto Base58Data::checkEncode(const typename std::enable_if<memory::is_vector<V>::value, V>::type& data)
        -> typename std::enable_if<std::is_same<S, std::string>::value || std::is_same<S, memory::SecureString>::value, S>::type
{
    // Add 4-byte hash check to the end
    V allData { data };
    utils::uint256 hash { utils::Hash256(data.begin(), data.end()) };
    allData.insert(allData.end(), (uint8_t*)&hash, (uint8_t*)&hash + 4);
    return encode<V, S>(allData);
}

} // end of namespace impl
} // end of namespace nakasendo

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Helper class for converting hex strings and characters to a byte
 * representation.
 */

#ifndef NCHAIN_SDK_STRING_TO_BYTES_H_
#define NCHAIN_SDK_STRING_TO_BYTES_H_

#include <vector>
#include <string>

namespace nakasendo { namespace impl { namespace utils
{

/// Class for converting hex strings and characters to a byte
/// representation and back again.
class HexToBytes
{
  public:

    /**
    * Convert a hex string to a big endian byte stream.
    * @param str A string to convert from hex to bytes.
    * @return A vector of bytes.
    */
    static std::vector<uint8_t> bigEndian(const std::string& str);

    /**
    * Convert a hex string to a little endian byte stream.
    * @param str A string to convert from hex to bytes.
    * @return A vector of bytes.
    */
    static std::vector<uint8_t> littleEndian(const std::string& str);

    /**
    * Convert a single hex character.
    * @param c A hex character to convert.
    * @return The byte value of the character.
    */
    static inline uint8_t toByte(char c);

    /**
    * Convert a big endian byte stream to a hex string.
    * @param bytes A vector of bytes to convert.
    * @return A hex coded string.
    */
    static std::string bigEndian(const std::vector<uint8_t>& bytes);

    /**
    * Convert a little endian byte stream to a hex string.
    * @param bytes A vector of bytes to convert.
    * @return A hex coded string.
    */
    static std::string littleEndian(const std::vector<uint8_t>& bytes);

    /**
    * Convert a nibble to a char.
    * @param nibble A nibble to convert.
    * @return A hex character.
    */
    static inline char toChar(uint8_t nibble);

};

}}}
#endif 

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Handle the encoding and decoding of Base58 and Base58Check data.
 */

#ifndef _NCHAIN_SDK_BASE_58_DATA_H_
#define _NCHAIN_SDK_BASE_58_DATA_H_

#include <vector>
#include <map>
#include <tuple>
#include <array>

#include "MetaDataDefinitions.h"

#include <impl/memory/SecureString.h>
#include <impl/memory/SecureVector.h>

namespace nakasendo
{
    class Key;
}

namespace nakasendo { namespace impl
{

/// A class to wrap Base58/Base58Check encoding and decoding.
class Base58Data
{
  public:

    /// Well known version prefixes.
    ///
    /// NOTE! If you change these here, don't forget to also update
    /// the static map VersionPrefixMap.
    enum class VersionPrefix
    {
        PUBLIC_KEY_ADDRESS,
        SCRIPT_ADDRESS,
        PRIVATE_KEY,
 
        TESTNET_PUBLIC_KEY_ADDRESS,
        TESTNET_SCRIPT_ADDRESS,
        TESTNET_PRIVATE_KEY,
 
        REGTEST_PUBLIC_KEY_ADDRESS,
        REGTEST_SCRIPT_ADDRESS,
        REGTEST_PRIVATE_KEY,
    };


    /**
    * Encode the given raw data in Base58 format.
    * @param data The data to encode.
    * @return A Base58 encoded representation of the raw data.
    */
    template <class V = memory::SecureByteVec, class S = memory::SecureString>
    static auto encode(const typename std::enable_if<memory::is_vector<V>::value, V>::type& data)
                    -> typename std::enable_if<std::is_same<S, std::string>::value || std::is_same<S, memory::SecureString>::value, S>::type;

    /**
    * Decode encoded string from Base58 format.
    * @param encoded Base58 encoded string to decode.
    * @return Raw decoded data.
    */
    static memory::SecureByteVec decode(const memory::SecureString& encoded);

    /**
    * Encode the given raw data in Base58Check format.
    * @param data The data to encode.
    * @return A Base58Check encoded representation of the raw data.
    */
    template <class V = memory::SecureByteVec, class S = memory::SecureString>
    static auto checkEncode(const typename std::enable_if<memory::is_vector<V>::value, V>::type& data)
                    -> typename std::enable_if<std::is_same<S, std::string>::value || std::is_same<S, memory::SecureString>::value, S>::type;

    /**
    * Encode the given private key in Base58Check format. Used for
    * creating wallet import format (WIF) strings.
    * @param key A private key.
    * @return A Base58Check encoded WIF representation of the key.
    */
    static memory::SecureString checkEncode(const Key& key);

    /**
    * Decode encoded string from Base58Check format.
    * @param encoded Base58Check encoded string to decode.
    * @return Raw decoded data.
    */
    static memory::SecureByteVec checkDecode(const memory::SecureString& encoded);


    /**
    * Utility method to get the appropriate version byte(s) for the requested
    * well know version prefix type.
    * @param ver A well know version prefix from the VersionPrefix enumeration.
    * @return The version bytes for the requested type.
    */
    static const std::vector<uint8_t>& getVersionBytesFor(VersionPrefix ver);

    /**
    * Utility method to get the well known version prefix type for some
    * Base58Check encoded data. Throws if it can't identify the data.
    * @parm data Base58Check encoded data with version prefix bytes at the start.
    * @return The VersionPrefix for the data.
    */
    static VersionPrefix getVersionOf(const memory::SecureByteVec& data);

    /**
    * Utility method to get the network type corresponding to a version prefix.
    * @param ver A VersionPrefix enumeration.
    * @return A NetworkType enumeration for the given version prefix.
    */
    static MetaDataDefinitions::NetworkType getNetworkFor(VersionPrefix ver);

  private:

    /// Static map of VersionPrefixes to version bytes and NetworkType
    using VersionPrefixMap = std::map<VersionPrefix, std::tuple<std::vector<uint8_t>, MetaDataDefinitions::NetworkType>>;
    static const VersionPrefixMap mVersionPrefixMap;

};

}}

#include "Base58DataT.h"

#endif

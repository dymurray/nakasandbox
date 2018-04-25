// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Some well known and useful MetaData definitions.
 */

#ifndef _NCHAIN_SDK_META_DATA_DEFINITIONS_H_
#define _NCHAIN_SDK_META_DATA_DEFINITIONS_H_

#include "utils/EnumCast.h"

#include <interface/MetaData.h>

namespace nakasendo { namespace impl
{

/// Useful internal metadata types.
///
/// The metadata definitions consist of a list of metadata key types
/// and for each key, a list of expected values. If you add a new key
/// to the KeyType enum, then you should also create a new enumeration
/// listing the expected values for that new key type.
///
/// NOTE: Don't forget to update the enum_table_t for KeyType after
/// modifying KeyType!
struct MetaDataDefinitions
{

    /// Metadata key types
    enum class KeyType
    {   
        UNKNOWN,
        NETWORK_TYPE,
        PUBLIC_KEY,
        COMPRESSION_TYPE
    };

    /// Metadata value types for key = NETWORK_TYPE
    enum class NetworkType
    {   
        UNKNOWN,
        MAIN_NET,
        TEST_NET,
        REGTEST
    };

    /// Metadata value types for key = COMPRESSION_TYPE
    enum class CompressionType
    {
        UNKNOWN,
        COMPRESSED,
        UNCOMPRESSED
    };
};

/// Enable enum_cast for KeyType
inline const utils::enumTableT<MetaDataDefinitions::KeyType>&
    enumTable(MetaDataDefinitions::KeyType)
{   
    static utils::enumTableT<MetaDataDefinitions::KeyType> table
    {   
        {MetaDataDefinitions::KeyType::UNKNOWN,          "Unknown"},
        {MetaDataDefinitions::KeyType::NETWORK_TYPE,     "NetworkType"},
        {MetaDataDefinitions::KeyType::PUBLIC_KEY,       "PublicKey"},
        {MetaDataDefinitions::KeyType::COMPRESSION_TYPE, "CompressionType"}
    };
    return table;
}

/// Enable enum_cast for NetworkType
inline const utils::enumTableT<MetaDataDefinitions::NetworkType>&
    enumTable(MetaDataDefinitions::NetworkType)
{
    static utils::enumTableT<MetaDataDefinitions::NetworkType> table
    {
        {MetaDataDefinitions::NetworkType::UNKNOWN,     "Unknown"},
        {MetaDataDefinitions::NetworkType::MAIN_NET,    "MainNet"},
        {MetaDataDefinitions::NetworkType::TEST_NET,    "TestNet"},
        {MetaDataDefinitions::NetworkType::REGTEST,     "RegTest"}
    };
    return table;
}

/// Enable enum_cast for CompressionType
inline const utils::enumTableT<MetaDataDefinitions::CompressionType>&
    enumTable(MetaDataDefinitions::CompressionType)
{
    static utils::enumTableT<MetaDataDefinitions::CompressionType> table
    {
        {MetaDataDefinitions::CompressionType::UNKNOWN,      "Unknown"},
        {MetaDataDefinitions::CompressionType::COMPRESSED,   "Compressed"},
        {MetaDataDefinitions::CompressionType::UNCOMPRESSED, "Uncompressed"},
    };
    return table;
}

}}

#endif

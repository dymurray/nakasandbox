// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Meta data to be associated with a supporting object. For maximum
 * flexibility meta data is simply a key,value pair, where both are
 * strings.
 */

#ifndef _NCHAIN_SDK_META_DATA_H_
#define _NCHAIN_SDK_META_DATA_H_

#include <string>
#include <utility>

namespace nakasendo
{

    /// Metadata key type
    using MetaDataKey = std::string;

    /// Metadata value type
    using MetaDataValue = std::string;

    /// A single item of metadata
    using MetaData = std::pair<MetaDataKey, MetaDataValue>;

}

#endif

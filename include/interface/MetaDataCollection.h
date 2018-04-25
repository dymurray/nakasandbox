// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Interface to a collection of metadata. Provides some utility methods on
 * top of what map gives us and enforces thread safety.
 */


#ifndef _NCHAIN_SDK_META_DATA_COLLECTION_H_
#define _NCHAIN_SDK_META_DATA_COLLECTION_H_

#include "MetaData.h"
#include "JSONSerialisable.h"

#include <unordered_map>
#include <memory>

namespace nakasendo
{

/// Forward declaration of MetaDataCollection pointer type
class MetaDataCollection;
/// Shared pointer type
using MetaDataCollectionSPtr = std::shared_ptr<MetaDataCollection>;
/// Shared pointer to a const collection
using MetaDataCollectionConstSPtr = std::shared_ptr<const MetaDataCollection>;

/// Interface to a collection of metadata.
class MetaDataCollection : public JSONSerialisable
{ 
  public:

    /// Our underlying storage type.
    using MetaMap = std::unordered_map<MetaDataKey, MetaDataValue>;

    /**
    * Check if the given key exists in the collection.
    * @param key The key to look for.
    * @return True if an entry for the given key was found.
    */
    virtual bool keyExists(const MetaDataKey& key) const = 0;

    /**
    * Lookup the metadata value for some key. Throws if not found.
    * @param key The metadata key to lookup.
    * @return The metadata value for the given key.
    */
    virtual const MetaDataValue& getMetaValue(const MetaDataKey& key) const = 0;

    /**
    * Set the value of a piece of metadata.
    * @param meta Some metadata to set.
    */
    virtual void setMetaData(const MetaData& meta) = 0;

    /**
    * Get a copy of all our metadata.
    * @return A copy of all our metadata.
    */
    virtual MetaMap getAllMetaData() const = 0;
};

} // end of namespace nakasendo

#endif // _NCHAIN_SDK_META_DATA_COLLECTION_H_

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Implementation of a collection of metadata. Provides some utility methods on
 * top of what map gives us and enforces thread safety.
 */

#ifndef _NCHAIN_SDK_META_DATA_COLLECTION_IMPL_H_
#define _NCHAIN_SDK_META_DATA_COLLECTION_IMPL_H_

#include "utils/EnumCast.h"

#include <interface/MetaDataCollection.h>
#include <mutex>

namespace nakasendo { namespace impl
{

/// Implements a MetaDataCollection.
class MetaDataCollectionImpl : public MetaDataCollection
{
  public:

    /// Default constructor
    MetaDataCollectionImpl() = default;
    /// Copy constructor
    MetaDataCollectionImpl(const MetaDataCollectionImpl& that);
    /// Move constructor
    MetaDataCollectionImpl(MetaDataCollectionImpl&& that);

    /// Assignment
    MetaDataCollectionImpl& operator=(const MetaDataCollectionImpl& that);
    /// Move assignment
    MetaDataCollectionImpl& operator=(MetaDataCollectionImpl&& that);

    /**
    * Check if the given key exists in the collection.
    * @param key The key to look for.
    * @return True if an entry for the given key was found.
    */
    bool keyExists(const MetaDataKey& key) const override;

    /**
    * Lookup the metadata value for some key. Throws if not found.
    * @param key The metadata key to lookup.
    * @return The metadata value for the given key.
    */
    const MetaDataValue& getMetaValue(const MetaDataKey& key) const override;

    /**
    * Set the value of a piece of metadata.
    * @param meta The metadata to set.
    */
    void setMetaData(const MetaData& meta) override;

    /**
    * Get a copy of all our metadata.
    * @return A copy of all our metadata.
    */
    MetaMap getAllMetaData() const override;

    /**
    * Copy some item of metadata.
    * @param src The MetaDataCollection to copy from.
    * @param key The key for the item to copy.
    */
    template<typename MetaType>
    void copyMetaData(MetaDataCollectionConstSPtr& src, MetaType key)
    {
        const std::string& keyName { utils::enum_cast<std::string>(key) };
        setMetaData( { keyName, src->getMetaValue(keyName) } );
    }


    // JSONSerialisable interface

    /// Get a unique identifer for this JSONSerialisable type.
    const std::string jsonObjectType() const override;

    /// Construct a boost::property_tree representation of ourselves.
    boost::property_tree::ptree toJson(const SerialisationFormat& fmt) const override;

    /// Set ourselves from the given boost::property_tree.
    void fromJson(const boost::property_tree::ptree& root,
                  const SerialisationFormat& fmt) override;

  private:

    /// Mutex for thread safety
    mutable std::mutex mMtx {};

    /// A map of metadata keys to values
    MetaMap mMetaData {};

};

/// Pointer type
using MetaDataCollectionImplPtr = std::shared_ptr<MetaDataCollectionImpl>;

}}

#endif

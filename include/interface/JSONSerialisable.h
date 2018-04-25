// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Interface definition for objects which are JSON serialisable/deserialisable.
 */

#ifndef _NCHAIN_SDK_JSON_SERIALISABLE_H_
#define _NCHAIN_SDK_JSON_SERIALISABLE_H_

#include "SerialisationFormat.h"

#include <string>
#include <memory>
#include <boost/property_tree/ptree.hpp>

namespace nakasendo
{

/// Interface for JSON serialisation/deserialisation.
class JSONSerialisable
{
  public:

    virtual ~JSONSerialisable(){}
    /**
    * Get a unique identifer for this JSONSerialisable type.
    * @return A unique identifer of objects of this JSONSerialisable type.
    */
    virtual const std::string jsonObjectType() const = 0;

    /**
    * Construct a boost::property_tree representation of ourselves.
    * @param fmt Describes the format to serialise in.
    * @return A boost::property_tree representation of ourselves.
    */
    virtual boost::property_tree::ptree toJson(const SerialisationFormat& fmt) const = 0;

    /**
    * Set ourselves from the given boost::property_tree.
    * @param root The JSON boost::property_tree that describes our requried
    *        state.
    * @param fmt Describes the format to deserialise from.
    */
    virtual void fromJson(const boost::property_tree::ptree& root,
                          const SerialisationFormat& fmt) = 0;
};

/// JSONSerialisable pointer type.
using JSONSerialisablePtr = std::shared_ptr<JSONSerialisable>;

}

#endif


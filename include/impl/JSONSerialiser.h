// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * A class to perform JSON serialisation and deserialisation.
 *
 * JSON serialisation can be performed for any JSONSerialisable
 * object to any stream. Similarly, deserialisation can be performed
 * for any default constructable JSONSerialisable class that has
 * previously registered itself with the factory portion of
 * JSONSerialiser.
 *
 * The format of JSON serialised objects is currently very simple:
 * {
 *   "ObjType": "...",
 *   "Obj":
 *   {
 *     Object specific fields...
 *   }
 * }
 */

#ifndef _NCHAIN_SDK_JSON_SERIALISER_H_
#define _NCHAIN_SDK_JSON_SERIALISER_H_

#include <interface/JSONSerialisable.h>

#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <type_traits>
#include <mutex>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace nakasendo { namespace impl
{

/// Class to perform JSON serialisation and deserialisation.
class JSONSerialiser final
{
  public:

    /**
    * Serialise the given sequence of JSONSerialisablePtr objects to a stream.
    * @param seq The sequence of JSONSerialisable objects to serialise.
    * @param str The stream to serialise to.
    * @param fmt A formatter to describe the required serialisation.
    * @return The modified stream.
    */
    template<typename Sequence>
    static auto serialise(const Sequence&            seq,
                          std::ostream&              str,
                          const SerialisationFormat& fmt = {})
        -> typename std::enable_if<!std::is_base_of<JSONSerialisable, Sequence>::value, std::ostream&>::type
    {
        boost::property_tree::ptree objListJson {};
        for(const JSONSerialisablePtr& obj : seq)
        {
            if(obj)
            {
                boost::property_tree::ptree objJson { serialiseCommon(*obj, fmt) };
                objListJson.push_back(std::make_pair("", objJson));
            }
        }

        try
        {
            boost::property_tree::ptree root {};
            root.add_child("ObjList", objListJson);
            boost::property_tree::write_json(str, root);
        }
        catch(boost::property_tree::ptree_error& e)
        {
            throw std::runtime_error("Failed to serialise JSONSerialisable sequence: "
                + std::string{e.what()});
        }

        return str;
    }

    /**
    * Serialise the given JSONSerialisable object to a stream.
    * @param obj The JSONSerialisable object to serialise.
    * @param str The stream to serialise to.
    * @param fmt A formatter to describe the required serialisation.
    * @return The modified stream.
    */
    static std::ostream& serialise(const JSONSerialisable&    obj,
                                   std::ostream&              str,
                                   const SerialisationFormat& fmt = {});

    /**
    * Serialise the given JSONSerialisable object to a string.
    * @param obj The JSONSerialisable object to serialise.
    * @param fmt A formatter to describe the required serialisation.
    * @return The object serialised as a string.
    */
    static std::string serialise(const JSONSerialisable&    obj,
                                 const SerialisationFormat& fmt = {});


    /**
    * Register a JSONSerialisable class is deserialisable by our factory.
    * The class type being registered must be JSONSerialisable and default
    * constructable.
    * @param typeName A string that identifies the type of the registering
    *        class. When the factory sees this string as the object identifier
    *        in serialised JSON it will attempt to initialise an instance of
    *        the registering type with the JSON ptree.
    * @return A boolean indicating success or failure.
    */
    template<typename JSONType>
    static auto registerForDeserialise(const std::string& typeName)
        -> typename std::enable_if<std::is_base_of<JSONSerialisable, JSONType>::value &&
                                   std::is_default_constructible<JSONType>::value,
                                   bool>::type
    {
        std::shared_ptr<Maker<JSONType>> maker { std::make_shared<Maker<JSONType>>() };
        std::lock_guard<std::mutex> lck { mMutex() };
        mMakers()[typeName] = std::static_pointer_cast<MakerBase>(maker);
        return true;
    }

    /// Overload of registerForDeserialise for detecting attempts to register
    /// non-JSONSerialisable classes.
    template<typename JSONType>
    static auto registerForDeserialise(const std::string& typeName)
        -> typename std::enable_if<!std::is_base_of<JSONSerialisable, JSONType>::value, bool>::type
    {
        static_assert(sizeof(JSONType) < 0, "JSONType is not JSONSerialisable");
        return false;
    }

    /// Overload of registerForDeserialise for detecting attempts to register
    /// non-default constructable classes.
    template<typename JSONType>
    static auto registerForDeserialise(const std::string& typeName)
        -> typename std::enable_if<!std::is_default_constructible<JSONType>::value, bool>::type
    {
        static_assert(sizeof(JSONType) < 0, "JSONType is not default constructable");
        return false;
    }


    /**
    * Is the given object type registered with our factory?
    * @param typeName The name of the type to check for.
    * @return True if registered, false otherwise.
    */
    static bool isRegistered(const std::string& typeName);

    /**
    * Deserialise and construct a JSONSerialisable object from the given stream.
    * @param str An input stream to read from.
    * @return A pointer to the newly created object.
    */
    static JSONSerialisablePtr deserialise(std::istream& str);

    /**
    * Deserialise and construct a JSONSerialisable object from the given string.
    * @param str An string containing a serialised object.
    * @return A pointer to the newly created object.
    */
    static JSONSerialisablePtr deserialise(std::string& str);

    /**
    * Deserialise and construct a list of JSONSerialisable objects from the
    * given stream.
    * @param str An input stream to read from.
    * @return A vector of pointers to newly created objects.
    */
    static std::vector<JSONSerialisablePtr> deserialiseList(std::istream& str);

  private:

    /**
    * Factory method to deserialise and initialise a JSONSerialisable object
    * from a JSON ptree.
    * @param root The root of the JSON property tree to initialise the object from.
    * @return A pointer to the newly created object.
    */
    static JSONSerialisablePtr make(const boost::property_tree::ptree& root);

    /**
    * Factory method to deserialise and initialise a list of objects from a JSON ptree.
    * @param root The root of the JSON property tree to initialise the list from.
    * @return A vector of pointers to newly created objects.
    */
    static std::vector<JSONSerialisablePtr> makeList(const boost::property_tree::ptree& root);


    /// Serialise the common parts of an object
    static boost::property_tree::ptree serialiseCommon(const JSONSerialisable&    obj,
                                                       const SerialisationFormat& fmt);

    /// Maker for a JSONSerialisable
    class MakerBase
    {
      public:
        virtual JSONSerialisablePtr operator()() = 0;
    };
    using MakerPtr = std::shared_ptr<MakerBase>;

    template<typename JSONType>
    class Maker : public MakerBase
    {
      public:
        JSONSerialisablePtr operator()() override
        {
            // JSONType Must be default constructable (enforced at registration time)
            return std::make_shared<JSONType>();
        }
    };

    /// Mutex for thread safety
    static std::mutex& mMutex()
    {
        static std::mutex mtx{};
        return mtx;
    }

    /// Map of deserialisable objects to Maker
    using MakerMap = std::map<std::string, MakerPtr>;
    static MakerMap& mMakers()
    {
        static MakerMap makers{};
        return makers;
    }

};

/// JSONSerialisable output operator for convenience.
std::ostream& operator<<(std::ostream& str, const JSONSerialisable& obj);

}}

#endif


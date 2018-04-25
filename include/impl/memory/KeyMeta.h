// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef KEYMETA_H
#define KEYMETA_H

#include <interface/MetaData.h>
#include <string>

namespace nakasendo {

namespace impl {

class KeyMeta{
public:
    KeyMeta() = default;
    KeyMeta(const MetaData& metaData);
    KeyMeta(const MetaDataKey& metaKey, const MetaDataValue& metaValue);

    const std::string& name() const;
    size_t size() const;
    void setName(const std::string& name);

    std::string toStr()const;

private:
    void removePattern(const char pattern, std::string& value);

    std::string mName;
    MetaData mMetaData;
};
}
}


#endif // KEYMETA_H

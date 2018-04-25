// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef KEYEXPORT_H
#define KEYEXPORT_H

#include <impl/utils/Status.h>
#include <impl/memory/SecureAllocatorAction.h>
#include <impl/memory/SecureByteAlloc.h>
#include <impl/memory/SecureArray.h>
#include <impl/memory/KeyMeta.h>

#include <string>
#include <memory>

namespace nakasendo { namespace impl { namespace memory {

/**
* @brief The KeyExport class
* @details This class is a delivery container for a Key entity
* It consists of secure memory block with Key content, it size,
* metadata structure and an error message. Class data is readonly
* it can be populated by KeyStorage class only
*/
template<typename T>
class KeyExport{
    friend class KeyStorage;
public:
    KeyExport() = default;
    KeyExport(const KeyExport& ) = delete;
    KeyExport& operator==(const KeyExport& ) = delete;


    KeyExport(KeyExport&& other):
        mMeta(std::move(other.mMeta)),
        mpData(std::move(other.mpData)),
        mDataSize(other.mDataSize),
        mStatusMsg(std::move(other.mStatusMsg))
    { other.mDataSize = 0; }


    KeyExport& operator==( KeyExport&& other) {
        if( this == &other )
            return *this;

        mMeta = std::move(other.mMeta);
        mpData = std::move(other.mpData);
        mDataSize = other.mDataSize; other.mDataSize = 0;
        mStatusMsg = std::move(other.mStatusMsg);

        return *this;
    }


    ~KeyExport(){}

    /**
     * @brief name Return key name. Name is element of
     * meta data structure
     * @return Key name
     */
    const std::string& name() const {
        return mMeta.name();
    }

    /**
     * @brief size Size of secure memory block
     * @return Mem block size
     */
    size_t size() const {
        return mDataSize;
    }

    /**
     * @brief data Pointer to allocated memory block
     * @return Pointer to secure mem block
     */
    const T* data()const {
        return mpData.data();
    }

    /**
     * @brief error Error message that could be populated
     * by KeyStorage class
     * @return Error message
     */
    const utils::Status& status() const {
        return *mStatusMsg;
    }

private:
    KeyMeta mMeta;                      // + SOME META data
    SecureArray<T> mpData;               /**< Secure data block */
    size_t mDataSize = 0;               /**< Secure data block size */

    const utils::Status* mStatusMsg;              /**< Error message */
};

} } }
#endif // KEYEXPORT_H

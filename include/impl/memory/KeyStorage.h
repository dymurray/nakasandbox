// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREARENA_H
#define SECUREARENA_H

#include <cstdint>
#include <vector>
#include <string>
#include <limits>

#include <impl/utils/FNV1aHash.h>
#include <impl/utils/Status.h>
#include <impl/memory/SecureAllocator.h>
#include <impl/memory/KeyExport.h>


namespace nakasendo { namespace impl { namespace memory {

/**
 * @brief The KeyStorage class. This class provides a secure storage service
 * It employes two different approaches on Windows and Linux.
 * On Windows this class itulises the system provided API for securing the data in user space memory.
 * On linux  this class utilises services provided by kernel keyring service. This serive is used to store
 * binary blocks of user data in the kernel area.
 */
class KeyStorage
{
    /**
     * @brief mStatusBase Unique #Status message code base
     */
    static constexpr uint32_t mStatusBase = utils::Hash::fnv1a32("KeyStorage");
public:
    /* Key storage Status */
    static constexpr utils::Status ERR_MAX_SIZE { mStatusBase,       "Payload is too big" };
    static constexpr utils::Status ERR_ZERO_SIZE{ mStatusBase + 1,   "Payload has zero size" };
    static constexpr utils::Status ERR_PADDING  { mStatusBase + 2,   "Wrong padding" };
    static constexpr utils::Status ERR_MEMORY   { mStatusBase + 3,   "Could not allocate secure memory block" };
    static constexpr utils::Status ERR_INIT     { mStatusBase + 4,   "Already initialised" };
    static constexpr utils::Status ERR_NON_INIT { mStatusBase + 5,   "Not initialised" };
    static constexpr utils::Status ERR_DECRYPT  { mStatusBase + 6,   "Decryption failed" };
    static constexpr utils::Status ERR_ENCRYPT  { mStatusBase + 7,   "Encryption failed" };
public:

    /**
     * @brief The Limits struct Provides information about system limits on number of keys and bytes.
     */
    struct Limits{
        std::string toStr()const {
            return "Keys: " + std::to_string(numOfKeys) +
                    "; Bytes: " + std::to_string(numOfBytes);
        }

        size_t numOfKeys = 0;
        size_t numOfBytes = 0;
    };

    /**
     * @brief KeyStorage Default constructor
     */
    KeyStorage(){}

    /**
     * @brief KeyStorage Deleted copy constructor
     */
    KeyStorage(const KeyStorage&) = delete;

    /**
     * @brief KeyStorage Move constructor
     */
    KeyStorage(KeyStorage&&);


    /**
     * @brief KeyStorage Deleted copy assignment operator
     */
    KeyStorage& operator=(const KeyStorage&) = delete;

    /**
     * @brief operator = move assignment operator
     * @return KeyStorage instance
     */
    KeyStorage& operator=(KeyStorage&&);

    /**
      Destructor
      */
    ~KeyStorage();

    /**
     * @brief limits Queries system wide information about allocated keys and bytes used.
     * @param max If true then requests max limits for user, otherwise reports number of
     *  keys and bytes already used
     * @return Limits structure
     */
    static Limits limits(bool max = true);

    /**
     * @brief write writes payload and its name into secured storage
     * @param name payload name
     * @param clearData payload in a plain data
     * @param size payload size
     * @return True if successful
     */
    const utils::Status& write(const KeyMeta& meta, SecureArray<uint8_t> &&clearText);

    /**
     * @brief read Returns key data decrypted in key export format
     * @return smart pointer to key structure
     */
    KeyExport<uint8_t> read() const;

    /**
     * @brief remove Delete payload from secure storeage and release resources
     * @return True if successful
     */
    utils::Status remove();

    uintptr_t id()const;
    size_t size() const;

    std::string debug() const;

private:
    /**
     * @brief getError Returns human readable description of the last error.
     * @return String description
     */
    std::string getError() const;

    bool validateSize(size_t size) const;

    /* Storage protocol details */
    static constexpr uint8_t PADDING_LENGTH = sizeof(uint8_t);
    static constexpr uint8_t SALT_LENGTH =    sizeof(uintptr_t);
    static constexpr uint8_t META_LENGTH =    sizeof(size_t);
    static constexpr uint8_t PAYLOAD_LENGTH = sizeof(size_t);
    static constexpr uint8_t HEADER_LENGTH = PADDING_LENGTH + SALT_LENGTH + META_LENGTH + PAYLOAD_LENGTH;

    static constexpr short maxSize = std::numeric_limits<short>::max();

    uintptr_t mStage = 0;                       /**< on Linux: serial number of the secure storage, Windows: pointer to secure memory area */
    size_t mStageSize = 0;                      /**< size of unecrypted data */
    bool mActive = false;                       /**< Treu if class instance is not initialised */
};

} } }
#endif // SECUREARENA_H

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef STATUSMESSAGES_H
#define STATUSMESSAGES_H

#include <impl/utils/FNV1aHash.h>

#include <cstdint>
#include <iostream>

namespace nakasendo { namespace impl { namespace utils {

constexpr auto StatusHash(const char* const value){
    return utils::Hash::fnv1a32(value);
}
using StatusType = std::remove_reference<decltype(StatusHash(""))>::type;

/**
 * @brief The Status struct is designed to be initialised as a constexpr
 */
struct Status{
    const StatusType id;                    /**< Status id which should be unique */
    const char *desc;                       /**< Readable description */

    /**
     * @brief operator == Compare two statuses
     * @param rhs Another instance of #Status
     * @return if the corresponding comparison holds, false otherwise
     */
    bool operator==(const Status& rhs) const noexcept{
        return id == rhs.id;
    }

    /**
     * @brief operator != Compare two statuses
     * @param rhs Another instance of #Status
     * @return if the corresponding comparison holds, false otherwise
     */
    bool operator!=(const Status& rhs) const noexcept{
         return id != rhs.id;
    }

    /**
     * @brief operator << performs stream output on #Status
     * @param out output stream
     * @param data instance of #Status to be inserted
     * @return  reference to output stream #out
     * @throw output stream related exceptions
     */
    friend std::ostream& operator<<(std::ostream& out, const Status& data){
        out << data.desc;
        return out;
    }
};

/* General */
constexpr Status SUCCESS {    0, "Success" };
constexpr Status FAILURE {    1, "Failure was detected at some point" };

} } }
#endif // STATUSMESSAGES_H

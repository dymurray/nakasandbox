// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef UTILS_ERROR_H
#define UTILS_ERROR_H

#include <cstdint>
#include <iostream>

namespace nakasendo { namespace impl { namespace utils {

/**
 * @brief The Error struct provide platform independent access to the last system error for a thread from which
 * #lastError static function was called. It is possible to check if an error actually happend by clling #isError method
 * @struct Error contains error code converted to int and human readable error description converted to std::string
 */
struct Error {
  int id = 0;                           /**< Error id which might be provided by platform and this library */
  std::string desc;                     /**< Error description which could be provided by platform and this library*/

  /**
   * @brief isError Checks if the current #Error instance is an error
   * @return True if struct #Error instance is not actually an error
   */
  inline bool isError()const noexcept { return id != 0; }

  /**
   * @brief lastError
   * @param errorMessageID
   * @return Instance of #Error struct containing information about the last error as reported by a platform
   */
  template<typename T>
  static Error lastError(T errorMessageID) noexcept;

};

} } }
#endif // STATUSMESSAGES_H

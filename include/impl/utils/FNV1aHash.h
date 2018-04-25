// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef COMPILE_TIME_HASH_HPP
#define COMPILE_TIME_HASH_HPP

#include <cstdint>


namespace nakasendo { namespace impl { namespace utils {
          // code license: public domain or equivalent
          //http://www.isthe.com/chongo/tech/comp/fnv/index.html

        /**
         * Fowler–Noll–Vo hash function FNV-1a hash calculation implemented as a constexpr
         */
        namespace {
            constexpr uint32_t cVal32 = 0x811c9dc5;
            constexpr uint32_t cPrime32 = 0x1000193;
            constexpr uint64_t cVal64 = 0xcbf29ce484222325;
            constexpr uint64_t cPrime64 = 0x100000001b3;
        }

        class Hash{
        public:
            /**
             * @brief fnv1a32 32 bit hash calculated as FNV-1a
             * @param str Source string
             * @param value Initial 32 bit value
             * @return 32 bit hash value for a string
             */
            inline static constexpr uint32_t fnv1a32(const char* const str, const uint32_t value = cVal32) noexcept {
                return (str[0] == '\0') ? value : fnv1a32(&str[1], static_cast<uint32_t>((value ^ str[0]) *
                        static_cast<uint64_t>(cPrime32)));
            }

            /**
             * @brief fnv1a64 64 bit hash calculated as FNV-1a
             * @param str Source string
             * @param value Initial 64 bit value
             * @return 64 bit hash value for a string
             */
            inline static constexpr uint64_t fnv1a64(const char* const str, const uint64_t value = cVal64) noexcept {
                return (str[0] == '\0') ? value : fnv1a64(&str[1], static_cast<uint64_t>((value ^ str[0]) * cPrime64));
            }
        };
}}}
#endif // COUNTER_HPP

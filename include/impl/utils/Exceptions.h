// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef NCHAIN_EXCEPTIONS_H
#define NCHAIN_EXCEPTIONS_H

#include <string>

namespace nakasendo { namespace impl { namespace utils
{
        class secure_bad_alloc : public std::bad_alloc
        {
        public:
            secure_bad_alloc(std::string msg): mMessage(msg)  { }

            virtual ~secure_bad_alloc() {}

            const char* what() const throw() /*gcc 6.3 requires throw()*/ override {
                return mMessage.c_str();
            }

        private:
            std::string mMessage;
        };

        class secure_bad_dealloc : public std::bad_alloc
        {
        public:
            secure_bad_dealloc(std::string msg): mMessage(msg)  { }

            virtual ~secure_bad_dealloc() {}

            const char* what() const throw() /*gcc 6.3 requires throw()*/ override {
                return mMessage.c_str();
            }

        private:
            std::string mMessage;
        };

        class secure_bad_convert : public std::bad_alloc
        {
        public:
            secure_bad_convert(std::string msg): mMessage(msg)  { }

            virtual ~secure_bad_convert() {}

            const char* what() const throw() /*gcc 6.3 requires throw()*/ override {
                return mMessage.c_str();
            }

        private:
            std::string mMessage;
        };
}}}

#endif // SECUREALLOCATOR_H


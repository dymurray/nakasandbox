// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef CONTEXT_H
#define CONTEXT_H

#include <mutex>
#include <functional>
#include <set>
#include <memory>

#include <interface/ContextObject.h>
#include <impl/ContextAction.h>

namespace nakasendo {
    namespace impl {

        class Context{
        public:
            static Context* init(std::unique_ptr<AContextAction> &&contextAction);

            void add(const ContextObject& pContextObj);
            void remove(const ContextObject& pContextObj);

            size_t size() const;

            virtual ~Context();

        private:
            Context(std::unique_ptr<AContextAction>&& contextAction);

            Context() = delete;
            Context(const Context& ) = delete;
            Context(Context&& ) = delete;
            Context& operator=(const Context& ) = delete;
            Context& operator=(Context&& ) = delete;


            static Context* mInstance;

            std::mutex mMutex;
            std::set<std::reference_wrapper<const ContextObject>, LessContextObject> mRegister;

            std::unique_ptr<AContextAction> mContextAction;

        };

    }
}

#endif // CONTEXT_H

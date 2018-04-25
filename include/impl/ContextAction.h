// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef CONTEXTACTION_H
#define CONTEXTACTION_H

#include <map>
#include <tuple>
#include <type_traits>
#include <mutex>

#include <iostream>

namespace{
    template <typename F, typename Tuple, bool Done, int Total, int... N>
    struct callImpl
    {
        static void call(F f, Tuple && t){
            callImpl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
        }
    };

    template <typename F, typename Tuple, int Total, int... N>
    struct callImpl<F, Tuple, true, Total, N...>
    {
        static void call(F f, Tuple && t){
            f(std::get<N>(std::forward<Tuple>(t))...);
        }
    };

    // User call
    template <typename F, typename Tuple>
    void call(F f, Tuple && t)
    {
       typedef typename std::decay<Tuple>::type ttype;
       callImpl<F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(f, std::forward<Tuple>(t));
    }
}


namespace nakasendo {
    namespace impl {

        class AContextAction{
        public:
            virtual ~AContextAction() = default;
            virtual bool runAction(const std::string& name) = 0;
        };

        template<typename Functor, typename... FunctorArgs>
        class ContextAction : public AContextAction
        {
        public:
            ContextAction(){}

            void addAction(const std::string& name, Functor actionFunc, FunctorArgs&&... args){
                std::lock_guard<std::mutex> locker(mMutex);
                mActions.emplace(name, FunctorArgsPack( actionFunc, args... ));
            }

            bool removeAction(const std::string& name){
                std::lock_guard<std::mutex> locker(mMutex);

                auto found = mActions.find(name);
                if( found != mActions.end() ){
                    mActions.erase(found);
                    return true;
                }

                return false;
            }

            bool runAction(const std::string& name) override{
                std::lock_guard<std::mutex> locker(mMutex);

                auto found = mActions.find(name);
                if( found != mActions.end() ){
                    FunctorArgsPack& currAction = found->second;
                    call(currAction.mFunctor, currAction.mArgs);
                    return true;
                }
                return false;
            }

        private:
            struct FunctorArgsPack{
                FunctorArgsPack(Functor functor, FunctorArgs&&... args ):
                    mFunctor(functor),
                    mArgs(std::forward_as_tuple(args...))
                {}

                Functor mFunctor;
                std::tuple<FunctorArgs...> mArgs;
            };

            std::mutex mMutex;
            std::map<std::string, FunctorArgsPack> mActions;

        };
    }
}



#endif // CONTEXTACTION_H

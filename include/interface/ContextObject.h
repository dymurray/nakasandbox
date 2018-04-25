// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef CONTEXTOBJECT_H
#define CONTEXTOBJECT_H

#include <functional>

namespace nakasendo{
    namespace impl {
        class Context;
    }
}


namespace nakasendo {

    /// A wrapper for a Context
    class ContextObject
    {
    public:
        /// Destructor
        virtual ~ContextObject(){}

    protected:
        /// Pointer to the context we wrap
        nakasendo::impl::Context*                       mContext;
    };

    /// Hash functor for ContextObject
    class HashContextObject{
    public:
        /// Functor operator
        size_t operator() ( const std::reference_wrapper<const ContextObject> ctx )const{
            const ContextObject& lCtx = ctx;
            return std::hash<const ContextObject*>{}(&lCtx);
        }
    };

    /// Comparator for ContextObject
    class LessContextObject{
    public:
        /// Functor operator
        bool operator() ( const std::reference_wrapper<const ContextObject> lhs,
                          const std::reference_wrapper<const ContextObject> rhs )const{
            const ContextObject& slhs = lhs, &srhs = rhs;
            return &slhs < &srhs;
        }
    };

}


#endif // CONTEXTOBJECT_H

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef SECUREALLOCATOR_H
#define SECUREALLOCATOR_H

#include <impl/utils/Exceptions.h>
#include <impl/memory/SecureAllocatorAction.h>
#include <impl/memory/SecureByteAlloc.h>

#include <limits>

namespace nakasendo{ namespace impl{ namespace memory{

         /**
         @class SecureAllocator
         @details Allocator that locks its contents from being paged
         out of memory and clears its contents before deletion. Derived
         from STL allocator class
        */

        template <typename T>
        class SecureAllocator
        {
        public:
            typedef T value_type;
            typedef value_type* pointer;
            typedef const value_type* const_pointer;
            typedef value_type& reference;
            typedef const value_type& const_reference;
            typedef std::size_t size_type;
            typedef std::ptrdiff_t difference_type;

            template <typename U>
            struct rebind {
                typedef SecureAllocator<U> other;
            };

            /**
             * @brief SecureAllocator - default constructor
             */
            inline SecureAllocator() noexcept{}
            /**
             * @brief SecureAllocator Copy constructor. Constructs the default allocator.
             * Since the default allocator is stateless, the constructors
             * have no visible effect.
             * @param allocator	-another allocator to construct with
             */
            inline SecureAllocator(const SecureAllocator&)noexcept{}
            inline SecureAllocator& operator=(const SecureAllocator&) noexcept { return *this; }

            /**
             * @brief SecureAllocator Templated copy constructor. Constructs the default allocator.
             * Since the default allocator is stateless, the constructors
             * have no visible effect.
             * @param allocator	-another allocator to construct with
             */
            template <typename U>
            inline SecureAllocator(const SecureAllocator<U>&) noexcept{}

            inline SecureAllocator(SecureAllocator&&) noexcept {}
            inline SecureAllocator& operator=(SecureAllocator&&) noexcept { return *this; }

            /**
              @brief Destructor
             */
            inline ~SecureAllocator() {}

            pointer address (reference value) const{
                return (&value);
            }

            const_pointer address (const_reference value) const{
                return (&value);
            }

            /**
             * @brief allocate method is is reimplementation of allocate from STL allocator
             * it
             * @details additionaly prevents memory region from being written into swap file
             * @param n - the number of objects to allocate storage for
             * @param hint - pointer to a nearby memory location
             * @return Pointer to the first byte of a memory block suitably aligned and
             * sufficient to hold an array of n objects of type T
             * @throw bad_alloc or secure_bad_alloc exceptions
             */
            pointer allocate(std::size_t n, const void *hint = 0){
                ((void)(hint));

                pointer p = nullptr;
                p = reinterpret_cast<pointer>( secureByteAlloc(n*sizeof(T)) );/* may throw here */
                return p;
            }

            /**
             * @brief deallocate Deallocates the storage referenced by the pointer p, which
             * must be a pointer obtained by an earlier call to allocate().
             * The argument n must be equal to the first argument of the call to allocate()
             * that originally produced p; otherwise, the behavior is undefined.
             * @details additionaly wipes or zeroizes the memory block before deallocation,
             * allows memory region from being written into swap file
             * @param p - pointer obtained from allocate()
             * @param n	- number of objects earlier passed to allocate()
             */
            void deallocate(pointer p, std::size_t n) noexcept{
                ((void)(n));
                try{
                    secureByteFree(p);/* size is known to secureByteFree, can throw */
                }catch(...){
                    //TODO logging here
                }
            }

            /**
             * @brief construct Constructs an object of type T in allocated uninitialized
             * storage pointed to by p, like using placement-new
             * @param p -  pointer to allocated uninitialized storage
             * @param args... the constructor arguments to use
             * @throw bad_alloc or consructor exception
             */
            template< typename U, typename... Args >
            void construct( U* p, Args&&... args ){
                void* securedPtr = secureByteAlloc(sizeof(U), p);      /* Can throw */
                try{
                    ::new (securedPtr) U(std::forward<Args>(args)...); /* U() constructor can throw */
                }catch(std::exception& ){
                    secureByteFree(securedPtr, securedPtr);
                    throw;
                }
            }

            /**
             * @brief destroy Destroys an U constructed with variadic arguments :: placement
             * @param p - pointer to the object that is going to be destroyed
             */                    
            template<typename U>
            void destroy(U* p) noexcept{
                if(p) p->~U();
                ::operator delete(p,p);
                try{
                    secureByteFree(p, p);       /*can throw */
                }catch(...){
                    //TODO logging here
                }
            }

            /**
             * @brief max_size Returns the maximum theoretically possible value of n,
             * for which the call allocate(n, 0) could succeed.
             * @return The maximum supported allocation size
             */
            constexpr size_type max_size() const { return ( std::numeric_limits<size_type>::max()/sizeof(T)); }

            friend bool operator==(SecureAllocator const&, SecureAllocator const&) noexcept { return true; }
            friend bool operator!=(SecureAllocator const& lhs, SecureAllocator const& rhs) noexcept { return !operator==(lhs, rhs); }
        };
} } }

#endif // SECUREALLOCATOR_H

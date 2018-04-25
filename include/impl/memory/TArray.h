// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef TARRAY_H
#define TARRAY_H

#include <impl/memory/SecureByteAlloc.h>
#include <impl/utils/Exceptions.h>
#include <impl/memory/IArrayAlloc.h>

#include <cstdlib>
#include <memory>
#include <vector>
#include <set>
#include <cstring>

namespace nakasendo{ namespace impl{ namespace memory{

/**
 * @brief The TArray class is a manager for memory blocks allocated by #mAlloc.byteAlloc
 * operations
 */
template<typename T, typename Alloc,
         typename std::enable_if<std::is_trivial<T>::value && std::is_base_of<IArrayAlloc, Alloc>::value,int>::type = 0>
class TArray{
public:
    TArray() noexcept = default;

    /**
     * @brief TArray copy constructor
     * @param other Another SecureArr instance
     * @throw any exception from calls thrown by #mAlloc.byteAlloc and secure_bad_alloc
     */
    TArray(const TArray& other){
        auto size = checkNumOfElements(other); /* can throw */

        if( !size )                             /* other is empty, construction finishes here */
            return;

        mPointer = (T*) mAlloc.byteAlloc(sizeof(T) * size );
        memcpy(mPointer, other.mPointer, sizeof(T) * size);
        isPlacement = false;
    }

    /**
     * @brief operator = copy assignment operator
     * @param other another SecureArr instance
     * @return this instance
     * @throw any exception from calls included in #clear that might include some user defined functions and secure_bad_alloc
     */
    TArray& operator=(const TArray& other){
        if( mPointer == other.mPointer )
            return *this;

        auto size = checkNumOfElements(other); /* can throw */

        clear();

        if( !size)                             /* other is empty, construction finishes here, just cleared everything */
            return *this;

        mPointer = (T*)mAlloc.byteAlloc( sizeof(T) * size );
        memcpy(mPointer, other.mPointer, sizeof(T) *size);
        isPlacement = false;
        return *this;
    }

    /**
     * @brief TArray move constructor
     * @param other Another SecureArr instance
     */
    TArray(TArray&& other){   /* considered noexcept because works only with trivial values */
        checkNumOfElements(other);      /* can throw, if not correct TArray object */

        mAlloc = std::move(other.mAlloc);
        mPointer = other.mPointer; other.mPointer = nullptr;
        isPlacement = other.isPlacement;        
    }

    /**
     * @brief operator = move assignment operator
     * @param other another SecureArr instance
     * @return this instance
     * @throw any exception from calls included in #clear that might include some user defined functions
     */
    TArray& operator=(TArray&& other){
        if( mPointer == other.mPointer )
            return *this;

        checkNumOfElements(other); /* can throw, if not correct TArray object */

        clear();    /* may throw */

        mAlloc = std::move(other.mAlloc);
        mPointer = other.mPointer; other.mPointer = nullptr;
        isPlacement = other.isPlacement;

        return *this;
    }

    /**
     * @brief TArray constructor construct TArray of a given size
     * @param size required number of elements of type T
     * @throw any exception from #mAlloc.byteAlloc
     */
    TArray(size_t size){
        mPointer = (T*)mAlloc.byteAlloc( sizeof(T) * size );
    }

    /**
     * @brief TArray
     * @param begin
     * @param end
     * @throw
     */
    template<typename Iter>
    TArray(Iter begin, Iter end){
        static_assert( sizeof(decltype(*begin)) == sizeof(T), "Wrong iterator type");
        insert(begin, end);
    }


    /**
     * @brief TArray placement constructor
     * @param size required number of elements of type T
     * @param allocated previously allocated memory block
     * @throw any exception from #mAlloc.byteAlloc
     */
    TArray(size_t size, T* allocated){
        mPointer = (T*)mAlloc.byteAlloc( sizeof(T) * size, allocated );
        isPlacement = true;
    }

    /**
     * Destructor
     */
    ~TArray(){
        try{
            clear();
        }catch(...){
            //TODO some serious logging here
        }
    }

    template<typename Iter>
    void assign(Iter begin, Iter end){
        static_assert( sizeof(decltype(*begin)) == sizeof(T), "Wrong iterator type");
        clear();
        insert(begin, end);
    }

    template<typename Iter>
    void insert(Iter begin, Iter end, size_t after){
        static_assert( sizeof(decltype(*begin)) == sizeof(T), "Wrong iterator type");
        std::vector<size_t> splitVec = {after};
        auto splitResult = split(splitVec.begin(), splitVec.end());
        if( splitResult.size() == 1 )
            splitResult[0] += TArray<T, Alloc>(begin, end);
        else
            splitResult[0] += (TArray<T, Alloc>(begin, end) + splitResult[1]);

        this->swap(splitResult[0]);
    }

    template<typename Iter>
    std::vector<TArray<T, Alloc>> split( Iter begin, Iter end){
        static_assert( std::is_same<typename Iter::value_type, size_t>::value, "Wrong iterator type");
        std::set<typename Iter::value_type> limits;

        for( auto iter = begin; iter != end; ++iter )
            limits.insert(*iter);

        std::vector<TArray<T, Alloc>> result;
        if( !limits.size() || *limits.begin() >= size() || size() == 0 ){
            result.emplace_back(*this);
            return result;
        }

        limits.insert(0);
        limits.insert(size());

        const T* src = data();
        T* dest = nullptr;

        for(auto currIter = limits.begin(); currIter != std::prev(limits.end()); ++currIter ){
                auto prevLimit = *currIter;
                auto currLimit = *std::next(currIter);
                if(currLimit > size())
                    break;

                result.emplace_back( currLimit - prevLimit);
                dest = result.rbegin()->data();
                std::copy(src + prevLimit, src + currLimit, dest );
        }

        return result;
    }
    /**
     * @brief data pointer of type T to a data array
     * @return  pointer to the data stored in the byte array
     */
    const T* data() const noexcept{
        return mPointer;
    }

    T* data() noexcept{
        return mPointer;
    }

    /**
     * @brief size Returns the number of elements in the container
     * @return The number of elements in the container
     */
    size_t size() const noexcept{
        return mAlloc.size(mPointer) /* size in bytes */ / sizeof(T);
    }

    /**
     * @brief swap #TArray class with another instance of the same class
     * @param other another instance of #TArray class
     */
    void swap(TArray& other){
        if( mPointer == other.mPointer )
            return;

        auto* tmp = other.mPointer;
        other.mPointer = mPointer;
        mPointer = tmp;

        auto tmpPlacement = other.isPlacement;
        other.isPlacement = isPlacement;
        isPlacement = tmpPlacement;

        auto tmpAlloc = other.mAlloc;
        other.mAlloc = mAlloc;
        mAlloc = tmpAlloc;
    }

    /**
     * @brief clear Delete data and release all resources
     * @throw all exceptions thrown by #secureByteFree
     */
    void clear(){
        if (isPlacement)  /* selects correct  free operation */
            mAlloc.byteFree(reinterpret_cast<void*>(mPointer), reinterpret_cast<void*>(mPointer));
        else
            mAlloc.byteFree( reinterpret_cast<void*>(mPointer));

        mPointer = nullptr;
        isPlacement = false;
    }

    TArray& operator +=(const TArray& other){
        auto size = this->size();
        auto otherSize = checkNumOfElements(other);
        if( !otherSize )
            return *this;

        Alloc tmpAlloc;
        T* tmpLocation = (T*)tmpAlloc.byteAlloc( sizeof(T) * (size + otherSize) );
        memcpy( tmpLocation, mPointer, size * sizeof(T));
        memcpy( tmpLocation + size, other.mPointer, otherSize * sizeof(T));
        this->clear();
        mPointer = tmpLocation;
        mAlloc = tmpAlloc;
        isPlacement = false;

        return *this;
    }

    friend TArray operator +(const TArray& lhs, const TArray& rhs){
        auto lhsSize = lhs.size();
        auto rhsSize = rhs.size();

        TArray tmpLocation( lhsSize + rhsSize );
        memcpy( tmpLocation.mPointer, lhs.mPointer, lhsSize * sizeof(T));
        memcpy( tmpLocation.mPointer + lhsSize, rhs.mPointer, rhsSize * sizeof(T));

        return std::move( tmpLocation );
    }

    friend bool operator ==(const TArray& lhs, const TArray& rhs){
        auto lhsSize = lhs.size();
        auto rhsSize = rhs.size();
        if ( lhsSize != rhsSize )
            return false;

        return memcmp( lhs.mPointer, rhs.mPointer, lhsSize) == 0;
    }

    friend bool operator !=(const TArray& lhs, const TArray& rhs){
        return !(lhs == rhs);
    }

private:
    size_t checkNumOfElements( const TArray& other){
        auto size = other.size();
        if( (!other.mPointer && size) || (other.mPointer && !size) ){
                mAlloc.getException();          /* Memory block is not registered with PageLocker or other problems */
        }

        return size;
    }

    template<typename Iter>
    void insert(Iter begin, Iter end){
        static_assert( sizeof(decltype(*begin)) == sizeof(T), "Wrong iterator type");

        auto size = std::distance(begin, end);
        mPointer = (T*)mAlloc.byteAlloc(  sizeof( T ) * size );

        T* lBegin = mPointer;
        std::copy(begin, end, lBegin);
    }

    T* mPointer = nullptr;                      /**< pointer to an allocated memory block */
    bool isPlacement = false;                   /**< placement indicator, true if memory was allocated bu placement */
    Alloc mAlloc;
};

} } }

#endif // TARRAY_H

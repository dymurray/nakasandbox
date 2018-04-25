// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef THREADSAFEHASH_H
#define THREADSAFEHASH_H

#include <impl/utils/FNV1aHash.h>
#include <impl/utils/Status.h>

#include <vector>
#include <memory>
#include <mutex>
#include <functional>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <atomic>
#include <unordered_map>

namespace nakasendo { namespace impl { namespace containers {

enum class ThreadSafeHashTypes{
    GENERAL,
    HITS_COUNTER
};

template<typename Key, typename Value, ThreadSafeHashTypes Type = ThreadSafeHashTypes::GENERAL, typename Action = bool(), typename Hash = std::hash<Key> >
/**
 * @brief The ThreadSafeHash class is thread-safe hash. A hits counter is build as a extention of this class.
 * A target is represented by the Key, whilst Value shows how many time the target was hit. Every target has
 * a counter which increases when target is hit.
 * The target could be also “unhit” which will lead to counter decremention. When counter reaches value below
 * certain threshold the target is removed from the collection. The class is implemented as a thread-safe
 * hash map.
 */
class ThreadSafeHash
{
    static constexpr short mcPrimeNumber = 191;
    static constexpr uint32_t mcStatusBase = utils::Hash::fnv1a32("HitsCounter");
public:
    static constexpr utils::Status APPEND  =   { mcStatusBase,       "New Item was appeneded" };
    static constexpr utils::Status INCREMENT = { mcStatusBase + 1,   "Item exists. Counter incremented" };
    static constexpr utils::Status REMOVE  =   { mcStatusBase + 2,   "Counter is empty. Item was removed" };
    static constexpr utils::Status DECREMENT = { mcStatusBase + 3,   "Item exists. Counter decremented" };
    static constexpr utils::Status UPDATE =    { mcStatusBase + 4,   "Item value was updated. Counter not changed" };
    static constexpr utils::Status NOT_FOUND = { mcStatusBase + 5,   "Item not found" };
public:
    typedef Key KeyType;
    typedef Value MappedType;
    typedef Hash HashType;

    /**
     * @brief HitsCounter Copy and Move operation are delete as the main usage is in singleton patterns
     * @param other
     */
    ThreadSafeHash( const ThreadSafeHash& other) = delete;
    ThreadSafeHash( ThreadSafeHash&& other) = delete;

    ThreadSafeHash& operator=( const ThreadSafeHash& other) = delete;
    ThreadSafeHash& operator=( ThreadSafeHash&& other) = delete;

    /**
     * @brief HitsCounter constructor
     * @param numBuckets Hash container buckets
     * @param pHasher Hash function
     * @throw any exception that std::vector constructor and new operator can throw
     */
    ThreadSafeHash( unsigned int numBuckets = mcPrimeNumber, const Hash& pHasher = Hash() ):
        mBuckets(numBuckets),
        mHasher(pHasher)
    {
        for( unsigned int i = 0; i < numBuckets; ++i)
            mBuckets[i].reset(new BucketType);

        mUniqueHitsNum.store(0);
        mTotalHitsNum.store(0);
    }

    /**
     * @brief find Value by Key
     * @param key search parameter
     * @param value set to the found value
     * @return True if value found
     * @throw If the algorithm fails to allocate memory, std::bad_alloc is thrown
     */
    bool find(const Key& key, Value& value) const noexcept{
        return getBucket(key).find(key, value);
    }

    /**
     * @brief appendOrUpdate appends new Key - Value pair or updates value if key found in the container
     * @param key search parameter
     * @param value new value
     */
    template<typename U = Value, ThreadSafeHashTypes C = Type>
    typename std::enable_if<C == ThreadSafeHashTypes::GENERAL, utils::Status>::type
    tryAppendOrUpdate( Key key, Value value, Action fAction = [](){ return true; }) noexcept
    {
        return getBucket(key).update(std::forward<Key>(key),
                                     std::forward<Value>(value),
                                      mUniqueHitsNum,
                                      mTotalHitsNum,
                                      fAction);
    }

    /**
     * @brief tryRemove Remove Key if found
     * @param key search parameter
     * @return  True if key was removed False if not
     */
    template<typename U = Value, ThreadSafeHashTypes C = Type>
    typename std::enable_if<C == ThreadSafeHashTypes::GENERAL, utils::Status>::type
    tryRemove(const Key& key, Action fAction = [](){ return true; }) noexcept
    {
        return getBucket(key).remove(key, mUniqueHitsNum, mTotalHitsNum, fAction);
    }

    /**
     * @brief appendOrIncrement Increment vaue if key is found. If not then adds key - value pair where
     * default value is 1. This method is defined only for unsigned types implementing operator++()
     * @param key search parameter
     * @return void //TODO enchanced return type
     */
    template<typename U = Value,  ThreadSafeHashTypes C = Type>
    typename std::enable_if<std::is_integral<U>::value && C == ThreadSafeHashTypes::HITS_COUNTER, utils::Status>::type
    tryAppendOrIncrement(Key key, std::function<Action> fAction = [](){ return true; }) noexcept
    {
        return getBucket(key).increment( std::forward<Key>(key), mUniqueHitsNum, mTotalHitsNum, fAction);
    }

    /**
     * @brief tryRemoveOrDecrement Decrement vaue if key is found. If value is equal to a certain threshold
     * remove pair. Default value is 1. This method is defined only for unsigned types implementing operator--()
     * @param key search parameter
     * @return True if value was removed or modified
     */
    template<typename U = Value,  ThreadSafeHashTypes C = Type>
    typename std::enable_if<std::is_integral<U>::value && C == ThreadSafeHashTypes::HITS_COUNTER, utils::Status>::type
    tryRemoveOrDecrement(const Key& key, std::function<Action>  fAction = [](){ return true; }) noexcept
    {
        return getBucket(key).decrement(key, mUniqueHitsNum, mTotalHitsNum, fAction);
    }


    /**
     * @brief getMap Return container content as an std::map
     * @return std::map
     */
    std::map<Key,Value> getMap() const
    {
        std::vector<std::unique_lock<boost::shared_mutex> > locks;

        for(unsigned i = 0; i < mBuckets.size(); ++i)
            locks.push_back(std::unique_lock<boost::shared_mutex>( mBuckets[i]->mutex ));


        std::map<Key,Value> res;
        for(unsigned i=0;i<mBuckets.size();++i)
        {
            for(typename BucketType::BucketIterator it=mBuckets[i]->bucketData.begin();
                it!=mBuckets[i]->bucketData.end(); ++it)
            {
                res.insert(*it);
            }
        }
        return res;
    }

    /**
     * @brief size Return number of unique hits
     * @return unique hits
     */
    size_t getUniqueHits() const noexcept
    {
        return mUniqueHitsNum.load();
    }

    /**
     * @brief size Return number of total hits including every repetetive hits
     * @return number of hits
     */
    size_t getTotalHits() const noexcept
    {
        return mTotalHitsNum.load();
    }

private:
    /**
     * @brief The BucketType class is implementation of a hash map bucket
     */
    class BucketType
    {
        friend std::map<Key,Value> ThreadSafeHash::getMap() const;
    public:
        /**
         * @brief find #key - #value pair
         * @param key key element to search for
         * @param value value element to search for
         * @return True if required pair exists, otherwise false
         */
        bool find(const Key& key, Value& value) noexcept
        {
            boost::shared_lock<boost::shared_mutex> lk(mutex, boost::defer_lock);
            try{
                lk.lock();
            }catch(std::exception& ){
                return false;  //TODO reporting
            }

            BucketIterator const found = bucketData.find(key);
            if( found != bucketData.end() ){
                value = found->second;
                return true;
            }
            else
                return false;
        }

        /**
         * @brief remove Removes the record identified by #key
         * @param key element to search for
         * @param value new value
         * @param uniqueHitsNum Number of unique hits excluding collisions
         * @param totalCounts Number of unique hits including collisions
         * @param action User defined function without arguments returning bool
         * @return status of the operation result
         */
        utils::Status remove(const Key& key,
                      std::atomic<size_t>& uniqueHitsNum,
                      std::atomic<size_t>& totalCounts,
                      std::function<Action> actionOnRemove) noexcept
        {
            std::unique_lock<boost::shared_mutex> lk(mutex, std::defer_lock);
            try{
                lk.lock();
            }catch(std::exception& ){
                return utils::FAILURE;  //TODO reporting
            }

            BucketIterator found_entry = bucketData.find( key );
            if( found_entry != bucketData.end()){
                bool actionResult = false;
                try{
                    actionResult = actionOnRemove();
                    if( actionResult ){
                        bucketData.erase(found_entry);
                        --uniqueHitsNum;
                        --totalCounts;
                        return REMOVE;
                    }else{
                        return utils::FAILURE;
                    }
                }catch(std::exception& ){
                    //TODO reporting
                    return utils::FAILURE;
                }catch( ... ){
                    //TODO reporting
                    return utils::FAILURE;
                }

            }else{
                return NOT_FOUND;
            }
        }


        /**
         * @brief update Updates the record identified by #key
         * @param key element to search for
         * @param value new value
         * @param uniqueHitsNum Number of unique hits excluding collisions
         * @param totalCounts Number of unique hits including collisions
         * @param action User defined function without arguments returning bool
         * @return status of the operation result
         */
        utils::Status update(Key&& key,
                      Value&& value,
                      std::atomic<size_t>& uniqueHitsNum,
                      std::atomic<size_t>& totalCounts,
                      std::function<Action> actionOnUpdate) noexcept
        {
            std::unique_lock<boost::shared_mutex> lk(mutex, std::defer_lock);
            try{
                lk.lock();
            }catch(std::exception& ){
                return utils::FAILURE;  //TODO reporting
            }

            BucketIterator found_entry = bucketData.find( key );
            if( found_entry == bucketData.end()){                   /* not found */
                bool actionResult = false;
                try{
                    actionResult = actionOnUpdate();                /* user action */
                    if( actionResult ){                             /* success */
                        bucketData.emplace( key, value );           /* add */
                        ++uniqueHitsNum;
                        ++totalCounts;
                        return APPEND;
                    }else{
                        return utils::FAILURE;                      /* user action failed */
                    }
                }
                catch(std::exception& ){                            /* user action threw */
                    //TODO reporting
                    return utils::FAILURE;
                }catch( ... ){                                      /* user action threw */
                    //TODO reporting
                    return utils::FAILURE;
                }
            }
            else{                                                   /* found */
                found_entry->second = value;
                return UPDATE;
            }
            return utils::FAILURE;
        }

        /**
         * @brief increment Increments the value associated with a #key. Additionaly, updates the hash counters
         * #uniqueHitsNum and #totalCounts to reflect actual results
         * @param key value of the element to search for
         * @param uniqueHitsNum Number of unique hits excluding collisions
         * @param totalCounts Number of unique hits including collisions
         * @param action User defined function without arguments returning bool
         * @return status of the operation result
         */
        template<typename U = Value>
        typename std::enable_if<std::is_integral<U>::value && !std::numeric_limits<U>::is_signed, utils::Status>::type
        increment(const Key& key,
                  std::atomic<size_t>& uniqueHitsNum,
                  std::atomic<size_t>& totalCounts,
                  std::function<Action> actionOnItemAdded) noexcept
        {
            static_assert(!std::numeric_limits<U>::is_signed, "Increment implemented only for unsigned integer values");

            std::unique_lock<boost::shared_mutex> lk(mutex, std::defer_lock);
            try{
                lk.lock();
            }catch(std::exception& ){
                return utils::FAILURE;  //TODO reporting
            }

            BucketIterator found_entry = bucketData.find( key );
            if( found_entry == bucketData.end()){
                bool actionResult = false;
                try{
                    actionResult = actionOnItemAdded();
                    if( actionResult ){
                        bucketData.emplace( key, 1 );
                        ++uniqueHitsNum;
                        ++totalCounts;
                        return APPEND;
                    }else {
                        return utils::FAILURE;
                    }
                }catch(std::exception& ){
                    //TODO reporting
                    return utils::FAILURE;
                }catch( ... ){
                    //TODO reporting
                    return utils::FAILURE;
                }
            }
            else{
                ++found_entry->second;
                ++totalCounts;
                return INCREMENT;
            }
        }


        /**
         * @brief decrement Decrements the value associated with a #key. Additionaly, updates the hash counters
         * #uniqueHitsNum and #totalCounts to reflect actual results
         * @param key value of the element to search for
         * @param uniqueHitsNum Number of unique hits excluding collisions
         * @param totalCounts Number of unique hits including collisions
         * @param action User defined function without arguments returning bool
         * @return status of the operation result
         */
        template<typename U = Value>
        typename std::enable_if<std::is_integral<U>::value && !std::numeric_limits<U>::is_signed, utils::Status>::type
        decrement(const Key& key,
                  std::atomic<size_t>& uniqueHitsNum,
                  std::atomic<size_t>& totalCounts,
                  std::function<Action> actionOnItemRemoved) noexcept
        {
            static_assert(!std::numeric_limits<U>::is_signed, "Decrement implemented only for unsigned integer values");

            std::unique_lock<boost::shared_mutex> lk(mutex, std::defer_lock);
            try{
                lk.lock();
            }catch(std::exception& ){
                return utils::FAILURE;  //TODO reporting
            }

            BucketIterator found_entry = bucketData.find( key );
            if( found_entry != bucketData.end()){
                --found_entry->second;
                --totalCounts;

                if(!found_entry->second){
                    bool actionResult = false;
                    try{
                        actionResult = actionOnItemRemoved();
                        if( actionResult ){
                            bucketData.erase(found_entry);
                            --uniqueHitsNum;
                            return REMOVE;
                        }
                    }catch(std::exception& ){
                    //TODO reporting
                    }catch( ... ){                          /* because user function can throw anything */
                        //TODO reporting
                    }
                    ++totalCounts;
                    ++found_entry->second;
                    return utils::FAILURE;
                }
                return DECREMENT;
            }
            return NOT_FOUND;
        }

    private:
        typedef std::pair<Key,Value> BucketValue;
        typedef std::unordered_map<Key,Value> BucketData;
        typedef typename BucketData::iterator BucketIterator;
        typedef typename BucketData::const_iterator CBucketIterator;

        /**
         * @brief find Value within a given bucket
         * @param key search parameter
         * @return Iterator to found Key - Value pair or end() iterator
         * @throw If the algorithm fails to allocate memory, std::bad_alloc is thrown
         */
        BucketIterator find(const Key& key) noexcept
        {
            BucketIterator res = bucketData.end();
            try{
                res =  std::find_if( bucketData.begin(),
                                     bucketData.end(),
                                     [&]( const BucketValue& item ){ return item.first == key; }
                );
            }catch(std::exception&){
                //TODO reporting
            }catch( ... ){
                //TODO reporting
            }

            return res;
        }

        mutable boost::shared_mutex mutex;
        BucketData bucketData;

    };

    /**
     * @brief getBucket Return bucket where the key is located
     * @param key search parameter
     * @return  Reference to bucket
     */
    BucketType& getBucket(const Key& key) const noexcept
    {
        std::size_t const bucketIndex = mHasher(key) % mBuckets.size();
        return *mBuckets[bucketIndex];  /* guaranteed, bucket was init in constructor */
    }

    std::vector<std::unique_ptr<BucketType> > mBuckets;          /**< Vector of buckets */
    Hash mHasher;

    std::atomic<size_t> mUniqueHitsNum;                          /**< Number of unique hits excluding collisions */
    std::atomic<size_t> mTotalHitsNum;                           /**< Number of total hits including collisions (aka size) */
};

template <typename Key, typename Value, ThreadSafeHashTypes Type, typename Action, typename Hash>
constexpr utils::Status ThreadSafeHash<Key, Value, Type, Action,  Hash >::APPEND;

template <typename Key, typename Value, ThreadSafeHashTypes Type, typename Action, typename Hash>
constexpr utils::Status ThreadSafeHash<Key, Value, Type, Action,  Hash >::INCREMENT;

template <typename Key, typename Value, ThreadSafeHashTypes Type, typename Action, typename Hash>
constexpr utils::Status ThreadSafeHash<Key, Value, Type, Action,  Hash >::REMOVE;

template <typename Key, typename Value, ThreadSafeHashTypes Type, typename Action, typename Hash>
constexpr utils::Status ThreadSafeHash<Key, Value, Type, Action,  Hash >::DECREMENT;

template <typename Key, typename Value, ThreadSafeHashTypes Type, typename Action, typename Hash>
constexpr utils::Status ThreadSafeHash<Key, Value, Type, Action,  Hash >::UPDATE;

template <typename Key, typename Value, ThreadSafeHashTypes Type, typename Action, typename Hash>
constexpr utils::Status ThreadSafeHash<Key, Value, Type, Action,  Hash >::NOT_FOUND;


} } }//namespaces
#endif // THREADSAFEHASH_H

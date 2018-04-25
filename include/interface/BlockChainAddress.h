// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/**
* Encapsulate a logical blockchain address used within the SDK
*/

#ifndef _NCHAIN_SDK_BLOCK_CHAIN_ADDRESS_H_
#define _NCHAIN_SDK_BLOCK_CHAIN_ADDRESS_H_

#include "Types.h"

namespace nakasendo
{
    /// Forward declaration of BlockChainAddress pointer type
    class BlockChainAddress;
    /// Unique pointer type
    using BlockChainAddressPtr = std::unique_ptr<BlockChainAddress>;
    /// Shared pointer type
    using BlockChainAddressSPtr = std::shared_ptr<BlockChainAddress>;

    /// Encapsulate a logical blockchain address used within the SDK
    class BlockChainAddress
    {
    public:
        /// Default destructor
        virtual ~BlockChainAddress() = default;

    public:

        /**
        * Get a blockchain address.
        * @return A string containing blockchain address.
        */
        virtual std::string getAddress() const = 0;
    };
} // end of namespace nakasendo

#endif //_NCHAIN_SDK_BLOCK_CHAIN_ADDRESS_H_

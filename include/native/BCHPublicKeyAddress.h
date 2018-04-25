// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/**
 * Implementation of a blockchain address for BCH public key addresses.
 */
#ifndef _NCHAIN_SDK_BLOCK_CHAIN_ADDRESS_BCH_PUBLIC_KEY_H_
#define _NCHAIN_SDK_BLOCK_CHAIN_ADDRESS_BCH_PUBLIC_KEY_H_

#include <interface/BlockChainAddress.h>
#include <impl/MetaDataDefinitions.h>
#include <impl/utils/UInt.h>

namespace nakasendo
{
    namespace native
    {
        class BCHPublicKeyAddress : public BlockChainAddress
        {
            /// Public key hash size.
            static constexpr size_t ECSECP256K1_PUBLIC_KEY_HASH_SIZE_IN_BYTES = 20;
            /// Version prefix.
            static constexpr size_t ECSECP256K1_ADDRESS_VERSION_PREFIX_SIZE_IN_BYTES = 1;

        public:
            /// Create a blockchain address based on a hash of the public key.
            BCHPublicKeyAddress(const impl::MetaDataDefinitions::NetworkType& networkType, const impl::utils::uint160& hash160);
            /// Copy constructor
            BCHPublicKeyAddress(const BCHPublicKeyAddress&) = default;
            /// Move constructor. 
            BCHPublicKeyAddress(BCHPublicKeyAddress&&) = default;
            /// Default destructor.
            virtual ~BCHPublicKeyAddress() = default;

        private:
            /// Get version prefix for the network.
            uint8_t getVersionPrefix() const;

            /// Network type
            const impl::MetaDataDefinitions::NetworkType m_networkType;
            /// A hash of the public key.
            const impl::utils::uint160 m_pubKeyHash160;

        public:
			/**
        	* Get a ECSECP256K1 blockchain address.
        	* @return A string containing blockchain address.
        	*/
        	virtual std::string getAddress() const override;
        };
	} // end of namespace native
} // end of namespace nakasendo

#endif // _NCHAIN_SDK_BLOCK_CHAIN_ADDRESS_BCH_PUBLIC_KEY_H_

// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * An adapter class to make our seedable RNGs usable in a CryptoPP context.
 */

#ifndef _NCHAIN_SDK_RNG_ADAPTER_H_
#define _NCHAIN_SDK_RNG_ADAPTER_H_

#include <impl/memory/SecureVector.h>
#include <cryptopp/rng.h>

namespace nakasendo { namespace impl
{

// A CryptoPP compatible RNG that works with our Seeder.
template<class RNG>
class RNGAdapter : public CryptoPP::RandomNumberGenerator
{ 
  public:

    /// Constructor
    RNGAdapter(RNG& rng) : mRNG{rng} {}

    /// Override GenerateBlock from CryptoPP::RandomNumberGenerator
    void GenerateBlock(byte* output, size_t size) override
    {
        impl::memory::SecureByteVec randBytes(size);
        mRNG.generate(randBytes);

        size_t numToCopy { randBytes.size() };
        if(size < numToCopy)
        {
            numToCopy = size;
        }
        memcpy(output, randBytes.data(), numToCopy);
    }

  private:

    /// The RNG we adapt
    RNG& mRNG;
};

}}

#endif

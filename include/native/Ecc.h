// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#ifndef _NCHAIN_SDK_ECC_H_
#define _NCHAIN_SDK_ECC_H_

#include <secp256k1/include/secp256k1.h>

namespace nakasendo 
{
	namespace native {

        /*
        *   Singelton class for ECC secp256k1 support.
        */
        class EccContext 
        {
            secp256k1_context *secp256k1_context_both = nullptr;

        private:
            EccContext(){}

        public:
            EccContext(const EccContext&) = delete;
            void operator=(const EccContext&) = delete;

            /*
            *   Create an instance of ECC.
            */
			static EccContext& getInstance()
			{
				static EccContext instance;
				return instance;
			}

            /*
            *   Initialize the elliptic curve support. May not be called twice without calling ECC_Stop first.
            */
            void ECC_Start();

            /*
            *   Deinitialize the elliptic curve support. No-op if ECC_Start wasn't called first.
            */
            void ECC_Stop();

            /*
            *   Get a pointer to the context.
            */
            const secp256k1_context* GetECContext() const;
 
            /// Check that required EC support is available at runtime.
            //bool ECC_InitSanityCheck(void);
        };
    }
}

#endif //_NCHAIN_SDK_ECC_H_  

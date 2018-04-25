// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * A utility class for converting keys to eliptic curve points.
 */

#ifndef _NCHAIN_SDK_KEY_CONVERTER_H_
#define _NCHAIN_SDK_KEY_CONVERTER_H_

#include <cryptopp/ecp.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/osrng.h>

namespace nakasendo 
{
    class Key;
    class PubKey;

    namespace impl
    {
        /// Class to help in converting keys to ECP points.
        class KeyConverter
        {
        public:

            /**
            * Default constructor.
            */
            KeyConverter() = default;

            /**
            * Get a CryptoPP public key for one of our public keys.
            * @param pubKey One of our public keys.
            * @return A CryptoPP Elliptic Curve Discrete Log (DL) public key.
            */
            CryptoPP::DL_PublicKey_EC<CryptoPP::ECP> ecpPubKey(const PubKey& pubKey);

            /**
            * Get raw scalar for the given byte array.
            * @param data Pointer to the start of the data to convert.
            * @param size Number of bytes to convert.
            * @return Scalar value for the converted bytes.
            */
            static CryptoPP::Integer ecpScalar(const uint8_t* data, size_t size);

            /**
            * Get raw scalar for the given private key.
            * @param key A private key.
            * @return Scalar value of the private key.
            */
            static CryptoPP::Integer ecpScalar(const Key& key);

            /**
            * Initialise a CryptoPP eliptic curve private key from one of our
            * private keys.
            * @param key Our private key.
            * @param out The CryptoPP eliptic curve private key to initialise.
            */
            void ecpPrvKeyInit(const Key& key, CryptoPP::DL_PrivateKey_EC<CryptoPP::ECP>& out);

            /**
            * Get a CryptoPP ECPPoint for the given public key.
            * @param pubKey A public key.
            * @return An ECPPoint.
            */
            static CryptoPP::ECPPoint ecpPoint(const PubKey& pubKey);

        private:

            /// A random number generator
            CryptoPP::AutoSeededRandomPool mRng{};

        };
    }
}

#endif // _NCHAIN_SDK_KEY_CONVERTER_H_

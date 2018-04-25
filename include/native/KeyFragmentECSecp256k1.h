// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/**
 * Implementation of a key fragment for ECSecp256k1.
 */
#ifndef _NCHAIN_SDK_KEY_FRAGMENT_ECSECP256K1_H_
#define _NCHAIN_SDK_KEY_FRAGMENT_ECSECP256K1_H_

#include <interface/KeyFragment.h>

namespace nakasendo
{
    namespace native
    {
        class KeyFragmentECSecp256k1 : public KeyFragment
        {
            /// Secret's name (with a given prefix) needs to be unique.
            static const std::string PREFIX;
            static const std::string SEPARATOR;

        public:

            /// Create key's fragment.
            KeyFragmentECSecp256k1();
            KeyFragmentECSecp256k1(Secret&& secret,
                                   const int nFragmenIdx,
                                   const std::string& sPubKey,
                                   const MetaDataCollectionConstSPtr& metaData);
            /// Copy constructor
            KeyFragmentECSecp256k1(const KeyFragmentECSecp256k1&) = delete;
            /// Move constructor. 
            KeyFragmentECSecp256k1(KeyFragmentECSecp256k1&&) = delete;
            /// Default destructor.
            virtual ~KeyFragmentECSecp256k1() = default;

        private:
            std::string getPubKeyFromSecretName() const;

            /// Flag to indicate we have registered with the JSON serialiser factory.
            /// Initialising this static actually triggers the registration.
            static bool KeyFragmentECSecp256k1JSONRegistered;

        public:

           /**
            * Get public key as string.
            * @return Public key as hex string.
            */
            virtual std::string getPubKeyAsHexString() const override;


            // JSONSerialisable interface

            /**
            * Get unique ID for this object type.
            * @return A unique string identifer for objects of this type.
            */
            const std::string jsonObjectType() const override;

        };

        //using KeyFragmentECSecp256k1Ptr = std::shared_ptr<KeyFragmentECSecp256k1>;
	} // end of namespace native
} // end of namespace nakasendo

#endif // _NCHAIN_SDK_KEY_FRAGMENT_ECSECP256K1_H_

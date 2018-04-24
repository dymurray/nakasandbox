// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

#include <iostream>
#include <vector>

#include <interface/Key.h>
#include <interface/PubKey.h>
#include <native/KeyECSecp256k1.h>

using namespace nakasendo;
using namespace nakasendo::native;

int main()
{
    /* Sign and verify a message */

    // Get an arbitrary message for signing
    // (The contents are unimportant for this example)
    std::string msg = "dylan terrence nakamoto";

    std::vector<uint8_t> message(msg.begin(), msg.end());
    message.push_back('\0');
    // Get public and private keys
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };
    PubKeyPtr pubKey { key->getPubKey() };

    KeyPtr newKey = key->derive(message);
    PubKeyPtr newPubKey = pubKey->derive(message);

    // Sign the message using our private key
    std::vector<uint8_t> signature { key->sign(message) };

    // Verify the signed message using the corresponding public key
    bool signatureOk { pubKey->verify(message, signature) };

    assert(newPubKey->getContent() == newKey->getPubKey()->getContent());

    if(signatureOk)
      std::cerr << "Signature verified ok" << std::endl;
    }

}
//int main()
//{
//    /* Sign and verify a message */
//
//    // Get an arbitrary message for signing
//    // (The contents are unimportant for this example)
//    std::vector<uint8_t> message(16);
//
//    // Get public and private keys
//    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };
//    PubKeyPtr pubKey { key->getPubKey() };
//
//    // Sign the message using our private key
//    std::vector<uint8_t> signature { key->sign(message) };
//
//    // Verify the signed message using the corresponding public key
//    bool signatureOk { pubKey->verify(message, signature) };
//
//    if(signatureOk)
//        std::cerr << "Signature verified ok" << std::endl;
//}


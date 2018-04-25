---
title: Keys
---

The SDK provides several classes related to asymmetric cryptographic key handling, namely `Key`, `PubKey` and `KeyFragment`.

## The `Key` and `PubKey` Interfaces

The primary interface to the cryptographic key functionality is the `Key` class. This is an abstract interface for the private key half of the public/private key pair and contains methods for (amongst other things) message decryption, message signing and retrieval of the corresponding public key.

The public key itself is represented by the `PubKey` class. This is again an abstract interface which contains methods for things such as message encryption, signature verification and blockchain address generation.

The abstract Key and PubKey classes have corresponding concrete implementations in the `KeyECSecp256k1` and `PubKeyECSecp256k1` classes. As the names suggests, these are implementations of a private and public keys compatible with the secp256k1 elliptic curve.

A new random private key can be created as follows:


**C++**

```c++
    // Create a new secp256k1 Key and initialise it
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };
```

**Java**

```Java
    // Create a new secp256k1 Key and initialise it
    Key key = KeyECSecp256k1.create();
```
The corresponding public key to the private key can be obtained via the `getPubKey()` method:


**C++**

```c++
    // Obtain the public key corresponding to an existing private key
    PubKeyPtr pubKey { key->getPubKey() };
```

**Java**

```Java
    // Obtain the public key corresponding to an existing private key
    PubKey pubKey = key.getPubKey();
```

A public key can also be created from a sequence of raw bytes, if that public key has been obtained from some source outside of the SDK (for example; from having been received from a remote instance over a network connection):


**C++**

```c++
    // Obtain the raw bytes of a public key from somewhere
    std::vector<uint8_t> rawPubKey { readRawPublicKey() };

    // Construct a secp256k1 public key from the raw bytes
    PubKeyPtr pubKey { std::make_unique<PubKeyECSecp256k1>(rawPubKey) };
```

**Java**

```Java
     // Obtain the raw bytes of a public key from somewhere
     byte[] content = pubKey.getContent();

     // Construct a secp256k1 public key from the raw bytes
     PubKey derivePubKey = pubKey.derive ( content );
```

### Message Signing and Verification

A private key can be used to sign an arbitrary message such that anyone who has the corresponding public key can verify the identity of the signer. This is performed using the `sign()` method on the private key and the `verify()` method on the public key:


**C++**

```c++
    // Obtain the message we want to sign from somewhere
    std::vector<uint8_t> message { getMessageForSigning() };

    // Get public and private keys
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };
    PubKeyPtr pubKey { key->getPubKey() };

    // Sign the message using our private key
    std::vector<uint8_t> signature { key->sign(message) };

    // Verify the signed message using the corresponding public key
    bool signatureOk { pubKey->verify(message, signature) };
```

**Java**

```Java
    // Obtain the message we want to sign from somewhere
    final String sMessage = "A message to process";
    byte[] message = sMessage.getBytes();

    // Get public and private keys
    Key key = KeyECSecp256k1.create();
    PubKey pubKey = key.getPubKey();

    // Sign the message using our private key
    byte[] signature = key.sign(message);

    // Verify the signed message using the corresponding public key
    pub.verify(message, signature));
```

### Message Encryption and Decryption

A public key can be used to encrypt a message (or any arbitrary sequence of bytes) such that only the corresponding private key can later be used to decrypt that same message. This is done using the `encrypt()` and `decrypt()` methods:


**C++**

```c++
    // Obtain the message to encrypt from somewhere
    SecureByteVec message { getMessageForEncrypting() };

    // Get public and private keys
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };
    PubKeyPtr pubKey { key->getPubKey() };

    // Encrypt the message using the public key
    std::vector<uint8_t> encrypted { pubKey->encrypt(message) };

    // Decrypt the encrypted message using the private key
    SecureByteVec decrypted { key->decrypt(encrypted) };
```

**Java**

```Java
    // Obtain the message to encrypt from somewhere
    final String sMessage = "A message to process";
    byte[] message = sMessage.getBytes();

    // Get public and private keys
    Key key = KeyECSecp256k1.create();
    PubKey pubKey = key.getPubKey();

    // Encrypt the message using the public key
    byte[] encrypted = pubKey.encrypt(message);

    // Decrypt the encrypted message using the private key
    byte[] decrypted = key.decrypt(encrypted);
```

### New Key Derivation

Both the private and public keys contain functionality to derive a new private or public key from some arbitrary message. This functionality forms part of the common secret derivation feature of the SDK discussed [here](http://KeySplittingAndSharing.md), but is also exposed as a method available for use on its own. Using the `derive()` method of a private or public key then it is possible to deterministically derive a new instance of a private or public key by providing an arbitrary seed message. For example:


**C++**

```c++
    // Generate some arbitrary message to use in new key derivation
    std::vector<uint8_t> message { getMessageFromSomewhere() };

    // Get initial public and private keys
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };
    PubKeyPtr pubKey { key->getPubKey() };

    // Deterministically derive new public and private keys using the message
    KeyPtr newKey { key->derive(message) };
    PubKeyPtr newPubKey { pubKey->derive(message) };
```

**Java**

```Java
    // Generate some arbitrary message to use in new key derivation
    final String sMessage = "A message to process";
    byte[] message = sMessage.getBytes();

    // Get initial public and private keys
    Key key = KeyECSecp256k1.create();
    PubKey pubKey = key.getPubKey();

    // Deterministically derive new public and private keys using the message
    Key newKey = key.derive(message);
    PubKey newPubKey = pubKey.derive(message);
```

The new public key derived above will be exactly the same as one obtained directly from the new derived private key, i.e. they contain exactly the same raw byte values:


**C++**

```c++
    // Derived public key is equal to one obtained directly from the private key
    assert(newPubKey->getContent() == newKey->getPubKey()->getContent());
```

**Java**

```Java
    Assert.assertArrayEquals(newKey.getPubKey().getContent(), newPubKey.getContent());
```

### Shared Secret Derivation

Building on top of the new key derivation functionality described above, the private key class also contains the `sharedSecret()` method for generating a common secret between two independent instances of the SDK. One use of this would be the in the key splitting and secure distribution of those shares, as described [here](http://KeySplittingAndSharing.md). To use this method you need an arbitrary shared message that both sides will use, and the other parties public key. A shared secret can then simply be constructed as follows:


**C++**

```c++
    // Create a shared secret using a common message and the other parties public key
    SecretPtr secret { myKey->sharedSecret(theirPubKey, commonMessage) };
```

**Java**

```Java
    // Create a shared secret using a common message and the other parties public key
    Secret secret = myKey.sharedSecret(theirPubKey, commonMessage);
```

### Key Import/Export In Wallet Import Format (WIF)

Wallet import format is a standard human readable text based encoding for private keys that makes it easy to extract private keys, pass them on and import them into another application.

**NOTE:** WIF encoding is a totally insecure format; if someone obtains a copy of a WIF encoded private key then they have complete access to that key and anything it controls. Great care should be taken then when importing/exporting keys in WIF format.

To export a private key in WIF format simply call the `exportAsWIF()` method. Conversely to import a WIF encoded key use the `importFromWIF()` method. For example:


**C++**

```c++
    // Create a new private key
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>()) };

    // Export as WIF
    SecureString wif { key->exportAsWIF() };

    // Recreate a copy of the key by importing from WIF
    KeyPtr importedKey { std::make_unique<KeyECSecp256k1<>>()) };
    importedKey->importFromWIF(wif);

    // The original and imported from WIF keys will be the same
    assert(key->getSecret() == importedKey->getSecret());
```

**Java**

```Java
    // Create a new private key
    Key key = KeyECSecp256k1.create();

    // Export as WIF
    byte[] exportedKey = key.exportAsWIF ();

    // Recreate a copy of the key by importing from WIF
    Key importedKey = KeyECSecp256k1.create();
    importedKey.importFromWIF(exportedKey);

    // The original and imported from WIF keys will be the same
    Assert.assertArrayEquals(importedKey.getContent(), key.getContent());
```

## The `KeyFragment` Interface

The SDK provides the ability to split a private key into a number of shares ([using INSERT PATENT LINK](http://)), and to be able later to recreate that private key using some subset of those shares. The `KeyFragment` interface describes the fragments returned by such a key splitting operation.

### Key Splitting and Reconstruction

Key splitting itself is another operation on a private key and is performed using the `split()` method. Splitting takes two parameters **_N_** and **_M_**, where **_N_** is the number of shares to split the key into and **_M_** is the number of those shares that must be brought together to recreate the original key. For example:


**C++**

```c++
    // Create a new private key
    KeyPtr originalKey { std::make_unique<KeyECSecp256k1<>>() };

    // Split using N=3, M=2
    std::vector<KeyFragmentPtr> fragments { originalKey->split(3, 2) };

    // Recreate key using any 2 of the fragments
    KeyFragments recreateUsing { *fragments[0], *fragments[1] };
    KeyPtr recreatedKey { std::make_unique<KeyECSecp256k1<>>(recreateUsing) };

    // The original and recreated keys will be the same
    assert(originalKey->getSecret() == recreatedKey->getSecret());
```

**Java**

```Java
    // Create a new private key
    Key key = KeyECSecp256k1.create();

    // Split using N=3, M=2
    KeyFragment[] keyFragments = key.split(3, 2);

    // Recreate key using any 2 of the fragments
    KeyFragment[] recreateUsing = { keyFragments[0], keyFragments[1] };
    Key recreatedKey = KeyECSecp256k1.create(recreateUsing);

    // The original and recreated keys will be the same
    Assert.assertArrayEquals(key.getSecret(), recreatedKey.getSecret());
```


**NOTE:** In the example above we are recreating the original key using the first 2 of 3 fragments, but we could have done the same using any 2 of the 3 received fragments.

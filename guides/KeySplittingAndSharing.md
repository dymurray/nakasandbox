---
title: Key Derivation, Splitting and Distribution
---

## Introduction

In this section we describe and illustrate with sample code the steps necessary to split a private key into some number of fragments (WIPO patent filing PCT/IB2017/050856), and to encrypt one of those shares using a common secret such that it can be sent to another instance of the SDK and decrypted again using the same common secret (WIPO patent filing PCT/IB2017/050829).

In the following example we consider the case of a ‘client’ instance of the SDK communicating with a ‘server’ instance of the SDK. The labels ‘client’ and ‘server’ are purely for ease of identification of the 2 parties involved, there is no actual requirement for one side to be a client and the other a server, they could just as easily be peers in a peer to peer network.

In this example then we consider the case of a client creating a new key they wish to split into a number of fragments and to securely send one of those fragments to another server.

Here we have a sequence diagram of the steps required to perform this task:

```plantuml
Client -> Client: Generate Master Key
Client -> Server: Send Master Public Key
Server -> Server: Generate Master Key
Server -> Client: Send Master Public Key
Client -> Client: Generate Random Message M
Client -> Client: Generate Shared Secret
Client -> Server: Send Message M
Server -> Server: Generate Shared Secret
Client -> Client: Generate Key For Splitting And Split It
Client -> Client: Encrypt Required Fragment Using Shared Secret
Client -> Server: Send Encrypted Fragment
Server -> Server: Decrypt Fragment Using Shared Secret
```

## Walkthrough

A more detailed description of the required steps illustrated with some sample code now follows.

### Master Key Generation

The first stage is for the client and server to generate the common secret that the client will use to encrypt the fragment before sending it to the server, and that the server will use to decrypt it once it arrives. This stage has several steps, the first of which is for the client and server to independently generate master keys. The client first:


**C++**

```c++
    // Client creates master key (private and public).
    KeyPtr clientMasterKey { std::make_unique<KeyECSecp256k1<RandutilsSeed>>() };
    PubKeyPtr clientMasterPublicKey { clientMasterKey->getPubKey() };
```


**Java**

```java
    // Client creates master key (private and public).
    Key clientMasterKey = KeyECSecp256k1.create();
    PubKey clientMasterPublicKey = clientMasterKey.getPubKey();
```

And then the server:


**C++**

```c++
    // Server creates master key (private and public).
    KeyPtr serverMasterKey { std::make_unique<KeyECSecp256k1<RandutilsSeed>>() };
    PubKeyPtr serverMasterPublicKey { serverMasterKey->getPubKey() };
```


**Java**

```java
    // Client creates master key (private and public).
    Key servertMasterKey = KeyECSecp256k1.create();
    PubKey serverMasterPublicKey = serverMasterKey.getPubKey();
```

### Master Public Key Exchange

The client and the server must then exchange master public keys with each other. The exact mechanism for this is beyond the scope of this document, but could for example be over a TCP/UDP socket. The client side:


**C++**

```c++
    // Client sends master public key to server and receives their master public key in return.
    std::vector<uint8_t> clientMasterPublicKeyBytes { clientMasterPublicKey->getContent() };
    sendToServer(clientMasterPublicKeyBytes);
    std::vector<uint8_t> serverMasterPublicKeyBytes { receiveFromServer() };
    PubKeyPtr clientCopyOfServerMasterPublicKey { std::make_unique<PubKeyECSecp256k1>(serverMasterPublicKeyBytes) };
```


**Java**

```java
    // Client sends master public key to server and receives their master public key in return.
    byte[] clientMasterPublicKeyBytes = clientMasterPublicKey->getContent();
    sendToServer(clientMasterPublicKeyBytes);
    byte[] serverMasterPublicKeyBytes = receiveFromServer();
    PubKey clientCopyOfServerMasterPublicKey = PubKeyECSecp256k1.create(serverMasterPublicKeyBytes);
```

And the server side:


**C++**

```c++
    // Server receives master public key from client and sends their master public key in return.
    std::vector<uint8_t> clientMasterPublicKeyBytes { receiveFromClient() };
    std::vector<uint8_t> serverMasterPublicKeyBytes { serverMasterPublicKey->getContent() };
    sendToClient(serverMasterPublicKeyBytes);
    PubKeyPtr serverCopyOfClientMasterPublicKey { std::make_unique<PubKeyECSecp256k1>(clientMasterPublicKeyBytes) };
```


**Java**

```java
    // Server receives master public key from client and sends their master public key in return.
    byte[] clientMasterPublicKeyBytes = receiveFromClient();
    byte[] serverMasterPublicKeyBytes = serverMasterPublicKey->getContent();
    sendToClient(serverMasterPublicKeyBytes);
    PubKey serverCopyOfClientMasterPublicKey = PubKeyECSecp256k1.create(clientMasterPublicKeyBytes);
```

### Random Message Construction

Next the client must generate a random message to use for shared secret generation and send this message to the server. Again, the exact mechanism for the generation of this message and forwarding it to the server is beyond the scope of this document, but the effect should be as follows:


**C++**

```c++
    // Client generates a message M to use for shared secret generation.
    // The message is 32 random bytes in this example, but can be arbitrary length and content.
    std::vector<uint8_t> messageM { getRandomBytes(32) };

    // Client sends message M to server
    sendToServer(messageM);
```


**Java**

```java
    // Client generates a message M to use for shared secret generation.
    // The message is 32 random bytes in this example, but can be arbitrary length and content.
    byte[] messageM = getRandomBytes(32);

    // Client sends message M to server
    sendToServer(messageM);
```

The server receives the message M from the client:


**C++**

```c++
    // Server reads message M from client.
    std::vector<uint8_t> messageM { receivefromclient() };
```


**Java**

```java
    // Server reads message M from client.
    byte[] messageM = receivefromclient();
```

### Common Secret Derivation

The client and the server are both now in possession of each others master public keys and a common message M, they can both now use those plus their own master private key to derive identical shared secrets. Client:


**C++**

```c++
    // Client uses message M, their master private key and the received server public
    // key to generate a shared secret.
    SecretSPtr clientSharedSecret { clientMasterKey->sharedSecret(clientCopyOfServerMasterPublicKey, messageM) };
```


**Java**

```java
    // Client uses message M, their master private key and the received server public
    // key to generate a shared secret.
    Secret clientSharedSecret = clientMasterKey.sharedSecret(clientCopyOfServerMasterPublicKey, messageM);
```

Server:


**C++**

```c++
    // Server uses message M, their master private key and the received client public
    // key to generate a shared secret.
    SecretSPtr serverSharedSecret { serverMasterKey->sharedSecret(serverCopyOfClientMasterPublicKey, messageM) };
```


**Java**

```java
    // Server uses message M, their master private key and the received client public
    // key to generate a shared secret.
    Secret serverSharedSecret = serverMasterKey.sharedSecret(serverCopyOfClientMasterPublicKey, messageM);
```

### Key Splitting

The client and server are both now in possession of a common shared secret. The next step is for the client to create the key they wish to partially share with the server, and to split it into a number of fragments:


**C++**

```c++
    // Client creates a new private key and splits it into shares.
    KeyPtr keyForSplitting { std::make_unique<KeyECSecp256k1<RandutilsSeed>>() };
    std::vector<KeyFragmentPtr> fragments { keyForSplitting->split(3,2) };
```


**Java**

```java
    // Client creates a new private key and splits it into shares.
    Key keyForSplitting = KeyECSecp256k1.create();
    KeyFragment[] fragments = keyForSplitting.split(3,2);
```

### Key Fragment Distribution

The client can now encrypt the share of the split key that they wish to send to the server using the shared secret, and serialise that encrypted share before sending it to the server.


**C++**

```c++
    // Client uses their copy of the shared secret to encrypt the key fragment they want to give to the server.
    KeyFragmentPtr clientCopyOfFragment { fragments[0] };
    clientCopyOfFragment->encrypt(clientSharedSecret->getSecret());

    // Client serialises the encrypted fragment ready for sending to the server.
    std::string serialisedFragment { JSONSerialiser::serialise(*clientCopyOfFragment) };

    // Client sends the encrypted fragment to the server using some unspecified communications channel.
    SendToServer(serialisedFragment);
```


**Java**

```java
    // Client uses their copy of the shared secret to encrypt the key fragment they want to give to the server.
    KeyFragment clientCopyOfFragment = fragments[0];
    clientCopyOfFragment.encrypt(clientSharedSecret.getSecret());

    // Client serialises the encrypted fragment ready for sending to the server.
    String serialisedFragment = JSONSerialiser.getInstance().serialiseKeyFragment(*clientCopyOfFragment) };

    // Client sends the encrypted fragment to the server using some unspecified communications channel.
    sendToServer(serialisedFragment);
```

### Key Fragment Receipt

Finally, the server reads the encrypted fragment from the client, deserialises it, and decrypts it using their copy of the shared secret. After decryption at the server both the client and server will have identical copies of the same key fragment.


**C++**

```c++
    // Server reads the serialised fragment from the client.
    std::string serialisedFragment { readFromClient() };

    // Server recreates the encrypted fragment from the received serialised fragment.
    std::stringstream deserialiseStream { serialisedFragment };
    KeyFragmentPtr serverCopyOfFragment { std::dynamic_pointer_cast<KeyFragment>(JSONSerialiser::deserialise(deserialiseStream)) };

    // Server decrypts the encrypted fragment using the previously derived shared secret.
    serverCopyOfFragment->decrypt(serverSharedSecret->getSecret());
```


**Java**

```java
    // Server reads the serialised share from the client.
    String serialisedFragment { readFromClient() };

    // Server recreates the encrypted share from the received serialised share.
    KeyFragment serverCopyOfFragment = JSONSerialiser.getInstance().deserialiseKeyFragment(serialisedFragment)) };

    // Server decrypts the encrypted share using the previously derived shared secret.
    serverCopyOfFragment.decrypt(serverSharedSecret.getSecret());
```

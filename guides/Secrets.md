---
title: Secret Handling
---

The SDK contains several facilities for handling secret data, where by secret we mean things such as keys, key fragments, or anything else the user doesn't want to be exposed unencrypted outside of the SDK.

### The `Secret` Class

The primary interface to secret data is the `Secret` class. This is both a base class to other more concrete secret data classes such as [`Key`](LINK_TO_KEY_DOC) and [`KeyFragment`](LINK_TO_KEY_DOC), and is a first class object in its own right. A secret consists of some arbitrary data (an array of bytes), a unique name, and some optional associated metadata (a list of key/value pairs where both the key and value are strings).

A secret can be constructed just by providing the secret bytes it should contain, in which case a random unique name will be automatically generated and assigned to it, or the name to use can also be specified:

###### C++
```c++
    // Create a secret from just raw bytes
    SecureVec rawBytes(32);
    SecretSPtr secret1 { std::make_shared<Secret>(rawBytes) };

    // Change the secrets contents
    SecureVec newRawBytes(64);
    secret1->setSecret(newRawBytes);

    // Create a secret from raw bytes and specify its name
    SecretSPtr secret2 { std::make_shared<Secret>(rawBytes, "SecretName") };

    // Change the secrets name
    secret2->setName("NewSecretName");
```

###### Java
```Java
    // Create a secret from just raw bytes
    byte[] rawBytes = getRawBytes();
    Secret secret1 = SecretImpl.create(rawBytes);

    // Change the secrets contents
    byte[] newRawBytes = getNewRawBytes();
    secret1.setSecret(newRawBytes);

    // Create a secret from raw bytes and specify its name
    Secret secret2 = SecretImpl.create(rawBytes, "SecretName");

    // Change the secrets name
    secret2.setName("NewSecretName");
```

Once created, metadata can be associated with a secret by calling the `setMetaData()` method:

###### C++
```c++
    // Set a piece of metadata on my secret
    secret1->setMetaData( {"SomeKey", "A value"} );
```

###### Java
```Java
    // Set a piece of metadata on my secret
    secret1.setMetaData( new String("SomeKey"), new String("A value") );
```

There is no limit other than available memory to the number of metadata key/value pairs that can be assigned to a given secret.

Secrets are serialisable and can be output to a stream using the [`JSONSerialiser`](LINK_TO_SERIALISATION) class. The output from serialisation can be read back in again later (or by another instance of the SDK) to recreate the original secret.

It is a requirement that no secret will be serialised in an insecure format, and as such it is required that before attempting to serialise a secret a user of the SDK must have previously encrypted that secret. Similarly, a deserialised secret will need to be decrypted before it can be used. For these purposes, the secret class provides the `encryptSecret()` and `decryptSecret()` methods. Both these methods take a password to use to perform the required operation. Should the SDK be asked to serialise an unencrypted secret it will refuse and will throw an exception.

The complete steps to create and encrypt a secret, serialise that secret to a stream and then deserialise that stream back to a secret would be as follows:

###### C++
```c++
    // Create an example secret
    SecretSPtr originalSecret { std::make_shared<Secret>(SecureVec{}) };
    // Encrypt using a password obtained from elsewhere
    originalSecret->encryptSecret(myPassword);

    // Serialise to a stream
    std::stringstream stream {};
    JSONSerialiser::serialise(*originalSecret, stream);

    // Deserialise and recreate a secret from a stream
    SecretSPtr deserialisedSecret { std::dynamic_pointer_cast<Secret>(JSONSerialiser::deserialise(stream)) };
    // Decrypt deserialised secret using the same password
    deserialisedSecret->decryptSecret(myPassword);
```

###### Java
```Java
    // Create an example secret
    Secret secret = SecretImpl.create();
	// Encrypt using a password obtained from elsewhere
    secret.encryptSecret(myPassword.getBytes());

	// Serialise to a string
    String serialisedSecret = JSONSerialiser.getInstance().serialiseSecret(secret);

	// Deserialise and recreate a secret from a stream
    Secret secretDeserialised = JSONSerialiser.getInstance().deserialiseSecret(serialisedSecret);
	// Decrypt deserialised secret using the same password
    secretDeserialised.decryptSecret(myPassword.getBytes());
```

### Other Secret Classes

So far we have only spoken in detail about the `Secret` class itself, and all the examples have used this class directly. It should be noted however that most of what has been said so far about secrets applies just as equally to the other classes that derive from `Secret` and are therefore also secrets, such as `Key` or `KeyFragment`.

So, using `Key` as an example:

###### C++
```c++
    // Create a key
    KeyPtr key { std::make_unique<KeyECSecp256k1<>>() };

    // Change the keys name
    key->setName("My special key");

    // Encrypt the secret part of the key
    key->encryptSecret(myPassword);

    // Serialise the key to a stream as a secret
    JSONSerialiser::serialise(*key, stream);

    // Deserialise and recreate a key from a stream
    KeySPtr deserialisedKey { std::dynamic_pointer_cast<Key>(JSONSerialiser::deserialise(stream)) };
    deserialisedKey->decryptSecret(myPassword);
```

### Secret Storage and the `SecretStore`

The SDK provides a secret storage facility for holding secrets in memory and writing them to a persitent backing store. These facilities are provided by the `SecretStore` class. The secret store is implemented as a singleton and can be accessed via the static `get()` method as follows:

###### C++
```c++
    // Get a reference to the secret store
    SecretStore& store { SecretStore::get() };
```

###### Java
```Java
    // Get a reference to the secret store
    SecretStore store = SecretStoreImpl.getInstance();
```

Before being used to store and retrieve secrets, the secret store must be told what to use as its persitent backing store and must be configured with a master password to use for encypting and decrypting secrets saved to the backing store.

The SDK provides a basic persistent backing store in the form of the `JSONSecretBackingStore` which just saves to and reads from a flat JSON text file. The secret store can be configured to use this backing store as follows:

###### C++
```c++
    // Set the master password to use when reading/writing secrets
    SecretStore::get().setMasterPassword(myPassword);

    // Configure the secret store to use the JSONSecretBackingStore
    std::string fileName { "/path/to/storage_file.json" };
    SecretBackingStoreSPtr backingStore { std::make_shared<JSONSecretBackingStore>(fileName) };
    SecretStore::get().setSecretBackingStore(backingStore);
```

###### Java
```Java
    // Set the master password to use when reading/writing secrets
    store.setMasterPassword(dbPasswd);

    // Configure the secret store to use the JSONSecretBackingStore
    String fileName { "c:\patch\to\storage_file.json" };
    SecretBackingStore backingstore = JSONSecretBackingStore.create(fileName);
    store.setSecretBackingStore(backingstore);
```

**NOTE:** The act of setting the persistent backing store triggers the secret store to clear out any currently held secrets and to re-load itself from the newly configured backing store. This operation requires the password to have been already set, so the correct sequence for configuring the secret store is to firstly set the master password and then to set the backing store.

**NOTE:** If the JSONSecretBackingStore is unable to open and read from the file with the name passed in to it (for example if the file does not yet exist) it will throw an exception. If you know that the file doesn't exist (for example if this is the first time it is running), then you can instruct the JSONSecretBackingStore to create an empty backing store file by passing a second parameter to the constructor:

###### C++
```c++
    // Instruct JSONSecretBackingStore to create an initial backing file
    JSONSecretBackingStore::InitialisationMode mode { JSONSecretBackingStore::InitialisationMode::INIT_CREATE };
    SecretBackingStoreSPtr backingStore { std::make_shared<JSONSecretBackingStore>(fileName, mode) };
```

###### Java
```Java
    // Instruct JSONSecretBackingStore to create an initial backing file
    InitialisationMode mode = InitialisationMode.INIT_CREATE;
    SecretBackingStore backingstore = JSONSecretBackingStore.create(fileName, mode);
```

Once the secret store is configured, users can add secrets to the store, remove secrets from the store and update existing secrets. All changes to the secret store will be transparently reflected in the persistent backing store. For example:

###### C++
```c++
    // Create a new secret
    SecretSPtr mySecret { std::make_shared<Secret>(SecureVec{}) };

    // Add the new secret to the store
    SecretStore::get().addSecret(mySecret);

    // Replace the secret with an updated version
    SecretSPtr myNewSecret { std::make_shared<Secret>(SecureVec{}, "NewSecretName") };
    SecretStore::get().replaceSecret(mySecret->getName(), myNewSecret);

    // Remove the updated secret from the store
    SecretStore::get().removeSecret(myNewSecret->getName());
```

###### Java
```Java
    // Create a new secret
    Secret mySecret = SecretImpl.create();

    // Add the new secret to the store
    store.addSecret(mySecret);

    // Replace the secret with an updated version
    Secret myNewSecret = SecretImpl.create(rawSecretBytes, "NewSecretName");
    store.replaceSecret(mySecret.getName(), myNewSecret);

    // Remove the updated secret from the store
    store.removeSecret(myNewSecret.getName());
```

The secret store enforces the requirement that all secrets have unique names and will throw an exception if you attempt to add a new secret to the store with the same name as an already existing secret in the store.

If you modify an existing secret that is already managed by the secret store (for example; by changing its name, contents or metadata) then those modifications will be automatically forwarded to the secret store and reflected in the persistent backing store.

Secrets in the store can be retrieved either individually by name, as a batch containing all the currently stored secrets, or can be searched for only those with matching metadata. In addition it is possible to ask for a list of all currently stored secret names. For example:

###### C++
```c++
    /* Assuming we already have a secret store populated with secrets */

    // Get a list of all secret names currently in the store
    SecretStore& store { SecretStore::get() };
    std::vector<std::string> allNames { store.getAllSecretNames() };

    // Fetch the secret with the first returned name
    SecretSPtr secret { store.getSecret(allNames[0]) };

    // Fetch all secrets
    SecretStore::SecretSet allSecrets { store.getAllSecrets() };

    // Fetch all secrets with metadata key 'SomeKey' = value 'SomeValue'
    SecretStore::SecretSet searchSecrets { store.getAllSecretsWhere("SomeKey", "SomeValue") };
```
###### Java
```Java
    /* Assuming we already have a secret store populated with secrets */

    // Get a list of all secret names currently in the store
    SecretStore store = SecretStoreImpl.getInstance();
    String[] allNames = store.getAllSecretNames();

    // Fetch the secret with the first returned name
    Secret secret = store.getSecret(allNames[0]);

    // Fetch all secrets
    Secret[] allSecrets = store.getAllSecrets();

    // Fetch all secrets with metadata key 'SomeKey' = value 'SomeValue'
    Secret[] searchSecrets = store.getAllSecretsWhere(new String("SomeKey"), new String("SomeValue"));
```

### Implementing Your Own Persistence Backend

As described above, the SDK provides a basic persistent backend for use with the secret store that just serialises secrets out to a flat JSON file. If you want to persist secrets somewhere else (for example to a database) you will need to implement your own backend and configure the secret store to use your new implementation.

In order to write your own persistence backend all you need to do is create a new class that derives from `SecretBackingStore` and implement the methods required by that interface. Those methods are as follows:

  * `void saveSecret(const SecretSPtr& secret)`
  * `void saveSecrets(const std::vector<SecretSPtr>& secrets)`
  * `void updateSecret(const std::string& name, const SecretSPtr& secret)`
  * `void removeSecret(const std::string& name)`
  * `void replaceAll(const std::vector<SecretSPtr>& secrets)`
  * `void loadAll()`

#### `saveSecret`

This method is called when a new secret is added to the store and needs to be persisted. The new secret in question is passed in to the method as its only argument.

###### C++
```c++
void MyDatabasePersister::saveSecret(const SecretSPtr& secret)
{
    // Take our mutex
    std::lock_guard<std::mutex> lck { m_mtx };
        
    // Save to hypothetical database
    m_dbHandle->save(secret);
}
```

#### `saveSecrets`

This method is called when a collection of new secrets have been added to the store and need to be persisted. It is expected that `saveSecrets` may be able to persist the new secrets more efficiently as a collection than would be possible by simply calling `saveSecret` repeatedly. The list of new secrets is passed in to the method as its only argument.

###### C++
```c++
void MyDatabasePersister::saveSecrets(const std::vector<SecretSPtr>& secrets)
{
    // Take our mutex
    std::lock_guard<std::mutex> lck { m_mtx };

    // Save to hypothetical database
    for(const SecretSPtr& secret : secrets)
        m_dbHandle->save(secret);        
}
```

#### `updateSecret`

This method is called when an existing secret in the store has been updated and those changes need to be persisted. It takes 2 arguments; the name of the old secret that has changed, and the new secret that it should be replaced with. Note that the name of the old secret is required because it is possible that it is the name itself that has changed.

###### C++
```c++
void MyDatabasePersister::updateSecret(const std::string& name, const SecretSPtr& secret)
{
    // Take our mutex
    std::lock_guard<std::mutex> lck { m_mtx };

    // Update in hypothetical database
    m_dbHandle->startTransaction();
    m_dbHandle->delete(name);
    m_dbHandle->save(secret);
    m_dbHandle->commitTransaction();
}
```

  #### `removeSecret`

  This method is called when a secret has been deleted from the store and that secret now needs to also be removed from the persistence layer. The name of the removed secret is passed in as the only argument.

###### C++
```c++
void MyDatabasePersister::removeSecret(const std::string& name)
{
    // Take our mutex
    std::lock_guard<std::mutex> lck { m_mtx };

    // Remove from hypothetical database
    m_dbHandle->delete(name);
}
  ```

#### `replaceAll`

This method is there for the special case when every secret in the secret store has been updated, and the whole contents of the persistence backend needs to be recreated. This is the case (for example) when the secret store master password is changed, and every secret then needs to be re-encrypted and persisted using that new password. The new list of secrets to persist are passed in as the only argument.

###### C++
```c++
void MyDatabasePersister::replaceAll(const std::vector<SecretSPtr>& secrets)
{
    // Take our mutex
    std::lock_guard<std::mutex> lck { m_mtx };

    // Update secrets in hypothetical database
    m_dbHandle->startTransaction();
    m_dbHandle->deleteAll();
    m_dbHandle->save(secrets);
    m_dbHandle->commitTransaction();
}
```

#### `loadAll`

This method is called when the secret store wants to triger a reload of all the currently persisted secrets. An implementation of this method simply needs to read everything from the current backing store, and call the special `loadSecret` method on the secret store for each retrieved secret. For example:

###### C++
```c++
void MyDatabasePersister::loadAll()
{
    // Take our mutex
    std::lock_guard<std::mutex> lck { m_mtx };

    // Fetch all persisted secrets from hypothetical database
    std::vector<SecretSPtr> allPersistedSecrets { m_dbHandle->readAll() };

    // Repopulate the secret store with all the persisted secrets
    for(const SecretSPtr& secret : allPersistedSecrets)
        SecretStore::get().loadSecret(secret);
}
```

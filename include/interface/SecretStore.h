// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * An in-memory registry for storing secrets. Implemented as a singleton.
 *
 * The store should have a master password set for encryption/decryption
 * and be hooked up with a pluggable backend for persisting secrets before
 * being used.
 */

#ifndef _NCHAIN_SDK_SECRET_STORE_H_
#define _NCHAIN_SDK_SECRET_STORE_H_

#include "SecretBackingStore.h"
#include "Secret.h"

namespace nakasendo
{

/// An in-memory store of secrets. Implemented as a singleton.
class SecretStore
{
  public:

    /// A collection of secrets.
    using SecretSet = std::vector<SecretSPtr>;

    /// Non copy-constructable.
    SecretStore(const SecretStore&) = delete;
    /// Non move-constructable.
    SecretStore(SecretStore&&) = delete;

    /// Default destructor
    virtual ~SecretStore() = default;

    /**
    * Accessor for the singleton instance.
    * @return A reference to the SecretStore.
    */
    static SecretStore& get();

    /**
    * Set the master password used for encrypting/decrypting all stored secrets.
    * @param passwd The master password to use for encrypting/decrypting.
    */
    virtual void setMasterPassword(const impl::memory::SecureByteVec& passwd) = 0;

    /**
    * Set the persistent backing store. The store will use this to save/load
    * the secrets going forward.
    * @param db The persistence DB to use.
    */
    virtual void setSecretBackingStore(const SecretBackingStoreSPtr& db) = 0;

    /**
    * Add a new secret to the store.
    * @param secret The new secret to add.
    */
    virtual void addSecret(const SecretSPtr& secret) = 0;

    /**
    * Add a list of new secrets to the store.
    * @param secrets The new secrets to add.
    */
    virtual void addSecrets(const SecretSet& secrets) = 0;

    /**
    * Replace an existing secret in the store.
    * @param name The name of the secret currently in the store to replace.
    * @param secret The new secret to replace the old one with.
    */
    virtual void replaceSecret(const std::string& name, const SecretSPtr& secret) = 0;

    /**
    * Remove the named secret from the store.
    * @param name The name of the secret to erase.
    */
    virtual void removeSecret(const std::string& name) = 0;

    /**
    * Fetch secret by name.
    * @param name The name of the secret to lookup.
    * @return A pointer to the requested secret if found, nullptr otherwise.
    */
    virtual SecretSPtr getSecret(const std::string& name) const = 0;

    /**
    * Fetch all secrets.
    * @return A list of all secrets in the store.
    */
    virtual SecretSet getAllSecrets() const = 0;

    /**
    * Lookup secrets according to metadata.
    * @param key Metadata key to lookup.
    * @param value Metadata value to lookup.
    * @return A list of secrets that have metadata key=value.
    */
    virtual SecretSet getAllSecretsWhere(const std::string& key,
                                         const std::string& value) const = 0;

    /**
    * Fetch all secrets names.
    * @return A list of all stored secret names.
    */
    virtual std::vector<std::string> getAllSecretNames() const = 0;

    /**
    * Add a newly loaded (restored from backing store) secret. Should only
    * be called from a SecretBackingStore implementation class.
    * @param secret A newly loaded secret to be added to the store.
    */
    virtual void loadSecret(const SecretSPtr& secret) = 0;

    /**
    * Notification that the named secret has changed and the perisitence
    * backend should be informed. Note: it is not expected that users of the
    * SDK should need to call this method, it should happen automatically if
    * a secret is modified.
    * @param name The name of the modified secret in the store.
    */
    virtual void secretUpdated(const std::string& name) = 0;

  protected:

    /// Constructor
    SecretStore() = default;

};

}

#endif

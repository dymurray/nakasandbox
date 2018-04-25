// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * The implementation for a secret store.
 */

#ifndef _NCHAIN_SDK_SECRET_STORE_IMPL_H_
#define _NCHAIN_SDK_SECRET_STORE_IMPL_H_

#include <interface/SecretStore.h>

#include <mutex>
#include <atomic>
#include <unordered_map>

namespace nakasendo { namespace impl
{

/// The implementation for a secret store.
class SecretStoreImpl final : public SecretStore
{
    /// For unit testing.
    friend class SecretStoreTester;

  public:
    /// Default constructor.
    SecretStoreImpl() = default;

    /// Forbid copying and assignment.
    SecretStoreImpl(const SecretStoreImpl&) = delete;
    SecretStoreImpl(SecretStoreImpl&&) = delete;
    SecretStoreImpl& operator=(const SecretStoreImpl&) = delete;
    SecretStoreImpl& operator=(SecretStoreImpl&&) = delete;

    /**
    * Set the master password used for encrypting/decrypting all stored secrets.
    * @param passwd The master password to use.
    */
    void setMasterPassword(const impl::memory::SecureByteVec& passwd) override;

    /**
    * Set the persistent backing store. The store will use this to save/load
    * the secrets going forward.
    * @param db The persistence DB to use.
    */
    void setSecretBackingStore(const SecretBackingStoreSPtr& db) override;

    /**
    * Add a new secret to the store.
    * @param secret The new secret to add.
    */
    void addSecret(const SecretSPtr& secret) override;

    /**
    * Add a list of new secrets to the store.
    * @param secrets The new secrets to add.
    */
    void addSecrets(const SecretSet& secrets) override;

    /**
    * Replace an existing secret in the store.
    * @param name The name of the secret currently in the store to replace.
    * @param secret The new secret to replace the old one with.
    */
    void replaceSecret(const std::string& name, const SecretSPtr& secret) override;

    /**
    * Remove the named secret from the store.
    * @param name The name of the secret to erase.
    */
    void removeSecret(const std::string& name) override;

    /**
    * Fetch secret by name.
    * @param name The name of the secret to lookup.
    * @return A pointer to the requested secret if found, nullptr otherwise.
    */
    SecretSPtr getSecret(const std::string& name) const override;

    /**
    * Fetch all secrets.
    * @return A list of all secrets in the store.
    */
    SecretSet getAllSecrets() const override;

    /**
    * Lookup secrets according to metadata.
    * @param key Metadata key to lookup.
    * @param value Metadata value to lookup.
    * @return A list of secrets that have metadata key=value.
    */
    SecretSet getAllSecretsWhere(const std::string& key,
                                 const std::string& value) const override;

    /**
    * Fetch all secrets names.
    * @return A list of all stored secret names.
    */
    std::vector<std::string> getAllSecretNames() const override;

    /**
    * Add a newly loaded (restored from backing store) secret. Should only
    * be called from a SecretBackingStore implementation class.
    * @param secret A newly loaded secret to be added to the store.
    */
    virtual void loadSecret(const SecretSPtr& secret) override;

    /**
    * Notification that the named secret has changed and the perisitence
    * backend should be informed.
    * @param name The name of the modified secret in the store.
    */
    void secretUpdated(const std::string& name) override;

  private:

    /// Check we have a backing store and return it
    SecretBackingStoreSPtr getBackingStore();

    /// A mutex for thread safety.
    mutable std::mutex mMtx {};

    /// A pointer to the backing store in use.
    SecretBackingStoreSPtr mBackingStore {};

    /// Map of secret names to Secrets
    using SecretMap = std::unordered_map<std::string, SecretSPtr>;
    SecretMap mSecretMap {};

    /// The master password to use when encrypting/descrypting.
    impl::memory::SecureByteVec mMasterPasswd {};

    /// Flag to indicate we're currently performing a reload
    std::atomic<bool> mReloading {false};
};


/// A helper class for unit testing only.
class SecretStoreTester
{
  public:
    /// Constructor
    SecretStoreTester(SecretStoreImpl& store) : mStore{store} {}

    /// Clear out the secret store
    void clear()
    {
        std::lock_guard<std::mutex> lck { mStore.mMtx };
        mStore.mSecretMap.clear();
    }

    /// Reload the secret store
    void reload()
    {
        mStore.setSecretBackingStore(mStore.mBackingStore);
    }

    /// Get size of secret store
    size_t storeSize() const
    {
        std::lock_guard<std::mutex> lck { mStore.mMtx };
        return mStore.mSecretMap.size();
    }

    /// Reset the backing store
    void resetBackingStore()
    {
        std::lock_guard<std::mutex> lck { mStore.mMtx };
        mStore.mBackingStore = nullptr;
    }

  private:
    /// Reference to the store.
    SecretStoreImpl& mStore;
};


}}

#endif

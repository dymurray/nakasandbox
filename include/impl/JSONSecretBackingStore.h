// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * An implementation of a secret backing store that serialises
 * secrets to a flat JSON file.
 */

#ifndef _NCHAIN_SDK_JSON_SECRET_BACKING_STORE_H_
#define _NCHAIN_SDK_JSON_SECRET_BACKING_STORE_H_

#include <interface/SecretBackingStore.h>

#include <string>
#include <mutex>

namespace nakasendo { namespace impl {

/// Forward declaration of JSONSecretBackingStore pointer type
class JSONSecretBackingStore;
/// Unique pointer type
using JSONSecretBackingStorePtr = std::unique_ptr<JSONSecretBackingStore>;
/// Shared pointer type
using JSONSecretBackingStoreSPtr = std::shared_ptr<JSONSecretBackingStore>;

/// A flat JSON file based secret DB.
class JSONSecretBackingStore : public SecretBackingStore
{
  public:

    /// Initialisation modes for the backing store file.
    enum class InitialisationMode
    {
        INIT_NONE,      // Do not initialise the file (we assume it already exists).
        INIT_CREATE     // Create an empty file.
    };

    /**
    * Constructor.
    * @param fileName Fully qualified filename of file to persist to.
    * @param init How to initialise the backing file.
    */
    JSONSecretBackingStore(const std::string& fileName,
                           InitialisationMode init = InitialisationMode::INIT_NONE);

    /**
    * Save a new secret.
    * @param secret The new secret to persist.
    */
    void saveSecret(const SecretSPtr& secret) override;

    /**
    * Save a list of new secrets. This will be more efficient than repeatedly
    * calling saveSecret() for each individual secret.
    * @param secrets The new secrets to persist.
    */
    void saveSecrets(const std::vector<SecretSPtr>& secrets) override;

    /**
    * Update a changed secret.
    * @param name The name of the changed secret. Note that this parameter is
    * required because it might be the name itself of the secret that has changed.
    * @param secret New details of the changed secret.
    */
    void updateSecret(const std::string& name, const SecretSPtr& secret) override;

    /**
    * Remove a secret.
    * @param name The name of the secret to remove.
    */
    void removeSecret(const std::string& name) override;

    /**
    * Load all stored secrets.
    */
    void loadAll() override;

    /**
    * Remove everything currently in the backing store and replace it with
    * whatever is currently held by the secret store.
    * @param secrets The required new contents of the backing store.
    */
    void replaceAll(const std::vector<SecretSPtr>& secrets) override;

  private:

    /// Rewrite our file from our map
    void writeToFile();

    /// A mutex for thread safety.
    mutable std::mutex mMtx {};

    /// Filename to use for saving to.
    std::string mFileName {};

    /// Maintain a map of persisted secrets
    std::unordered_map<std::string, SecretSPtr> mSecrets {};

};

}}

#endif

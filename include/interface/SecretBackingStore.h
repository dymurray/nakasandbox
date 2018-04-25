// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Interface definition for all secret persistence backends.
 *
 * See JSONSecretBackingStore for a simple file based example implementation
 * of this interface. A more sophisticated implementation could
 * persist (for example) to a database or an OS keyring.
 */

#ifndef _NCHAIN_SDK_SECRET_BACKING_STORE_H_
#define _NCHAIN_SDK_SECRET_BACKING_STORE_H_

#include "Secret.h"
#include "Types.h"

#include <memory>

namespace nakasendo
{

/// Forward declaration of SecretBackingStore pointer type
class SecretBackingStore;
/// Unique pointer type
using SecretBackingStorePtr = std::unique_ptr<SecretBackingStore>;
/// Shared pointer type
using SecretBackingStoreSPtr = std::shared_ptr<SecretBackingStore>;

/// Interface definition for all secret persistence backends.
class SecretBackingStore
{
  public:

    /// Destructor
    virtual ~SecretBackingStore() = default;

    /**
    * Save a new secret.
    * @param secret The new secret to persist.
    */
    virtual void saveSecret(const SecretSPtr& secret) = 0;

    /**
    * Save a list of new secrets. This will be more efficient than repeatedly
    * calling saveSecret() for each individual secret.
    * @param secrets The new secrets to persist.
    */
    virtual void saveSecrets(const std::vector<SecretSPtr>& secrets) = 0;

    /**
    * Update a changed secret.
    * @param name The name of the changed secret. Note that this parameter is
    * required because it might be the name itself of the secret that has changed.
    * @param secret New details of the changed secret.
    */
    virtual void updateSecret(const std::string& name, const SecretSPtr& secret) = 0;

    /**
    * Remove a secret.
    * @param name The name of the secret to remove.
    */
    virtual void removeSecret(const std::string& name) = 0;

    /**
    * Load all stored secrets.
    */
    virtual void loadAll() = 0;

    /**
    * Remove everything currently in the backing store and replace it with
    * whatever is currently held by the secret store.
    * @param secrets The required new contents of the backing store.
    */
    virtual void replaceAll(const std::vector<SecretSPtr>& secrets) = 0;

};

} // end of namespace nakasendo

#endif // _NCHAIN_SDK_SECRET_BACKING_STORE_H_

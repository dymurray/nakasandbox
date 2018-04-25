// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Base class for things that require the Secp256k1 context to be
 * setup and cleared down at construction / destruction time.
 */

#ifndef _NCHAIN_SDK_ECCONTEXT_SECP256K1_H_
#define _NCHAIN_SDK_ECCONTEXT_SECP256K1_H_

#include <interface/ContextObject.h>

#include "Ecc.h"

namespace nakasendo { namespace native
{

/// Base class that initialises and clears down the Secp256k1 context.
class EccontextSecp256k1 : public ContextObject
{
  public:

    /// Constructor
    EccontextSecp256k1();

    /// Destructor
    ~EccontextSecp256k1() override;

  protected:

    /// Fetch the underlying context
    const secp256k1_context* getEccContext() const;

  private:

    /// An alias for a context function.
    using ContextFunc = std::function<void()>;

    /// Define start context.            
    ContextFunc ctxStart = []() { EccContext::getInstance().ECC_Start(); };
    /// Define stop context.
    ContextFunc ctxStop = []() { EccContext::getInstance().ECC_Stop(); };

};

}}

#endif

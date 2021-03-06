//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CRANDOMBITSSUPPLIERHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CRANDOMBITSSUPPLIERHELPER_H__

#include "_Org_Apache_Harmony_Security_Provider_Crypto_CRandomBitsSupplierHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CarClass(CRandomBitsSupplierHelper)
    , public Singleton
    , public IRandomBitsSupplierHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI IsServiceAvailable(
        /* [out] */ Boolean * pAvailable);

    CARAPI GetRandomBits(
        /* [in] */ Int32 numBytes,
        /* [out, callee] */ ArrayOf<Byte> ** ppRandomBits);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CRYPTO_CRANDOMBITSSUPPLIERHELPER_H__

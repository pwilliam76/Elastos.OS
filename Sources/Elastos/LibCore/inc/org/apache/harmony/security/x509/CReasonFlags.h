
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CREASONFLAGS_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CREASONFLAGS_H__

#include "_Org_Apache_Harmony_Security_X509_CReasonFlags.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CReasonFlags)
    , public Object
    , public IReasonFlags
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI DumpValue(
        /* [in] */ IStringBuilder* pSb,
        /* [in] */ const String& prefix);

    CARAPI constructor(
        /* [in] */ ArrayOf<Boolean>* pFlags);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CREASONFLAGS_H__


#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREEHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralSubtreeHelper.h"
#include <elastos/core/Singleton.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralSubtreeHelper)
    , public Singleton
    , public IGeneralSubtreeHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetASN1(
        /* [out] */ IASN1Sequence** ppAsn1);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALSUBTREEHELPER_H__

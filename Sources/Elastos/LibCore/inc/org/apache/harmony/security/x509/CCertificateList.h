
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATELIST_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATELIST_H__

#include "_Org_Apache_Harmony_Security_X509_CCertificateList.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CCertificateList)
    , public Object
    , public ICertificateList
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetTbsCertList(
        /* [out] */ ITBSCertList** ppTbsCertList);

    CARAPI GetSignatureValue(
        /* [out, callee] */ ArrayOf<Byte>** ppSignatureValue);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncode);

    CARAPI constructor(
        /* [in] */ ITBSCertList* pTbsCertList,
        /* [in] */ IAlgorithmIdentifier* pSignatureAlgorithm,
        /* [in] */ ArrayOf<Byte>* pSignatureValue);

private:
    // TODO: Add your private member variables here.
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CCERTIFICATELIST_H__

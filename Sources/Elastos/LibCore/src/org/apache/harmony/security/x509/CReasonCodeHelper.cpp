
#include "org/apache/harmony/security/x509/CReasonCodeHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_SINGLETON_IMPL(CReasonCodeHelper)

CAR_INTERFACE_IMPL(CReasonCodeHelper, Singleton, IReasonCodeHelper)

ECode CReasonCodeHelper::GetASN1(
    /* [out] */ IASN1Type** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
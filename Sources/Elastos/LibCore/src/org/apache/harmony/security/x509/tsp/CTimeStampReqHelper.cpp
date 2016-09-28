
#include "org/apache/harmony/security/x509/tsp/CTimeStampReqHelper.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_SINGLETON_IMPL(CTimeStampReqHelper)

CAR_INTERFACE_IMPL(CTimeStampReqHelper, Singleton, ITimeStampReqHelper)

ECode CTimeStampReqHelper::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTimeStampReqHelper::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#include "org/apache/harmony/security/x509/CGeneralSubtree.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CGeneralSubtree)

CAR_INTERFACE_IMPL(CGeneralSubtree, Object, IGeneralSubtree)

ECode CGeneralSubtree::GetBase(
    /* [out] */ IGeneralName** ppBase)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralSubtree::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralSubtree::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder* pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CGeneralSubtree::constructor(
    /* [in] */ IGeneralName* pBase,
    /* [in] */ Int32 minimum,
    /* [in] */ Int32 maximum)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
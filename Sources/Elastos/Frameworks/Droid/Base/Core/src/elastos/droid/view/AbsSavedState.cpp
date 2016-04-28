
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/AbsSavedState.h"
#include "elastos/droid/view/CAbsSavedState.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL_2(AbsSavedState, Object, IAbsSavedState, IParcelable)

static AutoPtr<IAbsSavedState> InitEMPTY_STATE()
{
    AutoPtr<IAbsSavedState> state;
    CAbsSavedState::New((IAbsSavedState**)&state);
    return state;
}

const AutoPtr<IAbsSavedState> AbsSavedState::EMPTY_STATE = InitEMPTY_STATE();

/**
 * Constructor used to make the EMPTY_STATE singleton
 */
AbsSavedState::AbsSavedState()
{}

AbsSavedState::~AbsSavedState()
{}

ECode AbsSavedState::constructor()
{
    return NOERROR;
}

ECode AbsSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    if (superState == NULL) {
        Logger::E("AbsSavedState", "superState must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSuperState = superState != IParcelable::Probe(EMPTY_STATE) ? superState : NULL;
    return NOERROR;
}


ECode AbsSavedState::GetSuperState(
    /* [out] */ IParcelable** p)
{
    VALIDATE_NOT_NULL(p)
    *p = mSuperState;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode AbsSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mSuperState);
    return NOERROR;
}

ECode AbsSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> superState;
    source->ReadInterfacePtr((Handle32*)&superState);
    if (IParcelable::Probe(superState) != NULL) {
        Logger::E("AbsSavedState", "superState must be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSuperState = IParcelable::Probe(EMPTY_STATE);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos

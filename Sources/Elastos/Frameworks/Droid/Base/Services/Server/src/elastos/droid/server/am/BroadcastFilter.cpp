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

#include "elastos/droid/server/am/BroadcastFilter.h"
#include "elastos/droid/server/am/ReceiverList.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(BroadcastFilter, IntentFilter, IBroadcastFilter)

BroadcastFilter::BroadcastFilter(
    /* [in] */ ReceiverList* receiverList,
    /* [in] */ const String& packageName,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Int32 owningUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean isSystem)
    : mReceiverList(receiverList)
    , mPackageName(packageName)
    , mRequiredPermission(requiredPermission)
    , mOwningUid(owningUid)
    , mOwningUserId(userId)
    , mIsSystem(isSystem)
{
    assert(mReceiverList != NULL);
}

BroadcastFilter::~BroadcastFilter()
{
}

ECode BroadcastFilter::constructor(
    /* [in] */ IIntentFilter* intentFilter)
{
    return IntentFilter::constructor(intentFilter);
}

void BroadcastFilter::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    // TODO
    // DumpInReceiverList(pw, new PrintWriterPrinter(pw), prefix);
    if (mReceiverList) {
        mReceiverList->DumpLocal(pw, prefix);
    }
}

void BroadcastFilter::DumpBrief(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    DumpBroadcastFilterState(pw, prefix);
}

void BroadcastFilter::DumpInReceiverList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IPrinter* pr,
    /* [in] */ const String& prefix)
{
    IntentFilter::Dump(pr, prefix);
    DumpBroadcastFilterState(pw, prefix);
}

void BroadcastFilter::DumpBroadcastFilterState(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    if (mRequiredPermission != NULL) {
        pw->Print(prefix);
        pw->Print(String("mRequiredPermission="));
        pw->Println(mRequiredPermission);
    }
}

String BroadcastFilter::ToString()
{
    StringBuilder sb(128);
    sb += "BroadcastFilter{";
    sb += StringUtils::ToHexString(Int32(this));
    sb += " priority=";
    sb += mPriority;
    if (!mActions.IsEmpty()) {
        sb += " actions{";
        List<String>::Iterator it = mActions.Begin();
        for (; it != mActions.End(); ++it) {
            sb += *it;
            sb += ";";
        }
        sb += "}";
    }
    if (mCategories != NULL && !mCategories->IsEmpty()) {
        sb += " categories{";
        List<String>::Iterator it = mCategories->Begin();
        for (; it != mCategories->End(); ++it) {
            sb += *it;
            sb += ";";
        }
        sb += "}";
    }
    sb += " userId=";
    sb += mOwningUserId;
    sb += " packageName=";
    sb += mPackageName;
    sb += " requiredPermission=";
    sb += mRequiredPermission;
    if (mReceiverList) {
        sb += " ";
        sb += mReceiverList->ToString();
    }
    sb += "}";
    return sb.ToString();
}

ECode BroadcastFilter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

ECode BroadcastFilter::OnCompareTie(
    /* [in] */ IIntentFilter* other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // in case of a tie when sorting ordered broadcasts,
    // favor system apps.
    BroadcastFilter* bf = (BroadcastFilter*)other;
    if (mIsSystem) {
        *result = -1;
        return NOERROR;
    }
    if (bf->mIsSystem) {
        *result = 1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

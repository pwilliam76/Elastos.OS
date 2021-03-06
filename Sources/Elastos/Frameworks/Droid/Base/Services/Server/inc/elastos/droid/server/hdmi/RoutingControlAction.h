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

#ifndef __ELASTOS_DROID_SERVER_HDMI_ROUTINGCONTROLACTION_H__
#define __ELASTOS_DROID_SERVER_HDMI_ROUTINGCONTROLACTION_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "elastos/droid/server/hdmi/HdmiCecFeatureAction.h"
#include <Elastos.Droid.Hardware.h>

using Elastos::Droid::Hardware::Hdmi::IIHdmiControlCallback;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Feature action for routing control. Exchanges routing-related commands with other devices
 * to determine the new active source.
 *
 * <p>This action is initiated by various cases:
 * <ul>
 * <li> Manual TV input switching
 * <li> Routing change of a CEC switch other than TV
 * <li> New CEC device at the tail of the active routing path
 * <li> Removed CEC device from the active routing path
 * <li> Routing at CEC enable time
 * </ul>
 */
class RoutingControlAction
    : public HdmiCecFeatureAction
    , public IRoutingControlAction
{
private:
    class InnerSub_SendMessageCallback
        : public Object
        , public IHdmiControlServiceSendMessageCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_SendMessageCallback(
            /* [in] */ RoutingControlAction* host);

        //@Override
        CARAPI OnSendCompleted(
            /* [in] */ Int32 error);

    private:
        RoutingControlAction* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RoutingControlAction();

    CARAPI constructor(
        /* [in] */ IHdmiCecLocalDevice* localDevice,
        /* [in] */ Int32 path,
        /* [in] */ Boolean queryDevicePowerStatus,
        /* [in] */ IIHdmiControlCallback* callback);

    // @Override
    CARAPI Start(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ProcessCommand(
        /* [in] */ IHdmiCecMessage* cmd,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI HandleTimerEvent(
        /* [in] */ Int32 timeoutState);

private:
    CARAPI HandleReportPowerStatus(
        /* [in] */ Int32 devicePowerStatus);

    CARAPI GetTvPowerStatus(
        /* [out] */ Int32* result);

    static CARAPI_(Boolean) IsPowerOnOrTransient(
        /* [in] */ Int32 status);

    CARAPI SendSetStreamPath();

    CARAPI FinishWithCallback(
        /* [in] */ Int32 result);

    CARAPI QueryDevicePowerStatus(
        /* [in] */ Int32 address,
        /* [in] */ IHdmiControlServiceSendMessageCallback* callback);

    CARAPI HandlDevicePowerStatusAckResult(
        /* [in] */ Boolean acked);

    CARAPI InvokeCallback(
        /* [in] */ Int32 result);

private:
    static const String TAG;

    // State in which we wait for <Routing Information> to arrive. If timed out, we use the
    // latest routing path to set the new active source.
    static const Int32 STATE_WAIT_FOR_ROUTING_INFORMATION;

    // State in which we wait for <Report Power Status> in response to <Give Device Power Status>
    // we have sent. If the response tells us the device power is on, we send <Set Stream Path>
    // to make it the active source. Otherwise we do not send <Set Stream Path>, and possibly
    // just show the blank screen.
    static const Int32 STATE_WAIT_FOR_REPORT_POWER_STATUS;

    // Time out in millseconds used for <Routing Information>
    static const Int32 TIMEOUT_ROUTING_INFORMATION_MS;

    // Time out in milliseconds used for <Report Power Status>
    static const Int32 TIMEOUT_REPORT_POWER_STATUS_MS;

    // true if <Give Power Status> should be sent once the new active routing path is determined.
    Boolean mQueryDevicePowerStatus;

    // If set to TRUE, call {@link HdmiControlService#InvokeInputChangeListener()} when
    // the routing control/active source change happens. The listener should be called if
    // the events are triggered by external events such as manual switch port change or incoming
    // <Inactive Source> command.
    Boolean mNotifyInputChange;

    // @Nullable
    AutoPtr<IIHdmiControlCallback> mCallback;

    // The latest routing path. Updated by each <Routing Information> from CEC switches.
    Int32 mCurrentRoutingPath;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_ROUTINGCONTROLACTION_H__

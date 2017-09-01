//===================================================================================================================
//
// file :		zmq_event_subscription_change.h
//
// project :		tango
//
// author(s) :		ingvord
//
// Copyright (C) :      2004-2017
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//===================================================================================================================
#ifndef TANGO_ZMQ_EVENT_SUBSCRIPTION_CHANGE_H
#define TANGO_ZMQ_EVENT_SUBSCRIPTION_CHANGE_H

#include <tango/client/DeviceProxy.h>
#include "zmq_event_suscription_change_request.h"
#include "zmq_event_subscription_change_response.h"

namespace tango
{
namespace common
{
namespace admin
{
namespace commands
{

class ZmqEventSubscriptionChange
{
    Tango::DeviceProxy *proxy;
public:
    ZmqEventSubscriptionChange(Tango::DeviceProxy *proxy)
        : proxy(proxy)
    {}

    ZmqSubscriptionChangeResponse execute(const ZmqSubscriptionChangeRequest &request)
    {
        Tango::DeviceData input;

        input << request.to_DevVarStringArray();

        Tango::DeviceData output;
        output = proxy->command_inout("ZmqEventSubscriptionChange", input);

        return ZmqSubscriptionChangeResponse{output};
    }
};

}
}
}
}


#endif //TANGO_ZMQ_EVENT_SUBSCRIPTION_CHANGE_H

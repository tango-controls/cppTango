//===================================================================================================================
//
// file :		zmq_event_subscription_info.h
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
#ifndef TANGO_ZMQ_EVENT_SUBSCRIPTION_INFO_H
#define TANGO_ZMQ_EVENT_SUBSCRIPTION_INFO_H

#include <tango/client/DeviceProxy.h>
#include "zmq_event_subscription_info_response.h"

namespace tango
{
namespace common
{
namespace admin
{
namespace commands
{

class ZmqEventSubscriptionInfo
{
    Tango::DeviceProxy *proxy;
public:
    explicit ZmqEventSubscriptionInfo(Tango::DeviceProxy *proxy)
        : proxy(proxy)
    {}

    ZmqEventSubscriptionInfoResponse execute()
    {
        Tango::DeviceData output;
        try
        {
            output = proxy->command_inout("ZmqEventSubscriptionInfo");
        }
        catch (const Tango::DevFailed &ex)
        {
            //TODO replace DevFailed with actual exception
            if (ex.errors[0].reason == Tango::API_CommandNotFound)
            {
                //try pre 1003 logic
                Tango::DeviceData input;
                Tango::DevVarStringArray value{1};
                value[0] = "info";
                input << value;
                output = proxy->command_inout("ZmqEventSubscriptionChange", input);
            }
            else
            {
                throw;
            }

        }
        const Tango::DevVarLongStringArray *value;
        bool is_ok = output >> value;
        if (not(is_ok))
            throw std::logic_error{"Failed to extract DevVarLongStringArray from DeviceData"};

        return ZmqEventSubscriptionInfoResponse{value};
    }
};

}
}
}

}

#endif //TANGO_ZMQ_EVENT_SUBSCRIPTION_INFO_H

//===================================================================================================================
//
// file :		event_subscription.h
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
#ifndef TANGO_EVENT_SUBSCRIPTION_H
#define TANGO_EVENT_SUBSCRIPTION_H

#include <tango/server/tango_const.h>
#include <string>
#include <vector>
#include <common/utils.h>
namespace tango
{
namespace common
{
namespace event
{

/**
 * Fields are guarenteed to be in lower case
 *
 */
struct EventSubscription
{
    Tango::DeviceProxy *proxy;

    Tango::EventType event_type;
    std::string event_name;

    std::string device_name;
    std::string object_name;

    bool is_fwd;

    Tango::CallBack *callback;
    Tango::EventQueue *event_queue;

    EventSubscription(Tango::DeviceProxy *proxy,
                      Tango::EventType event_type,
                      string event_name,
                      string device_name,
                      string object_name,
                      bool is_fwd,
                      Tango::CallBack *callback,
                      Tango::EventQueue *ev_queue) noexcept
        : proxy(proxy),
          event_type(event_type),
          event_name(StringUtils::to_lower_case(std::move(event_name))),
          device_name(StringUtils::to_lower_case(std::move(device_name))),
          object_name(StringUtils::to_lower_case(std::move(object_name))),
          is_fwd(is_fwd),
          callback(callback),
          event_queue(ev_queue)
    {}
};

}
}
}

#endif //TANGO_EVENT_SUBSCRIPTION_H

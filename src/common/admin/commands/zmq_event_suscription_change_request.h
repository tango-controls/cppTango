//===================================================================================================================
//
// file :		zmq_event_suscription_change_request.h
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
#ifndef TANGO_ZMQ_EVENT_SUSCRIPTION_CHANGE_REQUEST_H
#define TANGO_ZMQ_EVENT_SUSCRIPTION_CHANGE_REQUEST_H

#include <string>
#include <idl/tango.h>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include "../../utils.h"


namespace tango
{
namespace common
{
namespace admin
{
namespace commands
{

struct ZmqEventSubscriptionChangeRequest
{
    std::string device_name;
    std::string object_name;
    std::string action_name;
    std::string event_name;

    ZmqEventSubscriptionChangeRequest(std::string device_name,
                                      std::string object_name,
                                      std::string action,
                                      std::string event_name)
        : device_name(std::move(device_name)),
          object_name(std::move(object_name)),
          action_name(std::move(action)),
          event_name(std::move(event_name))
    {}

    explicit ZmqEventSubscriptionChangeRequest(Tango::DevVarStringArray *argin) throw(std::invalid_argument)
    {
        if (argin->length() != 4)
            throw std::invalid_argument{"A string array of size = 4 is expected here"};

        device_name = StringUtils::to_lower_case(std::string{(*argin)[0]});
        object_name = StringUtils::to_lower_case(std::string{(*argin)[1]});
        action_name = StringUtils::to_lower_case(std::string{(*argin)[2]});
        event_name = StringUtils::to_lower_case(std::string{(*argin)[3]});
    }
};
}
}
}
}

#endif //TANGO_ZMQ_EVENT_SUSCRIPTION_CHANGE_REQUEST_H

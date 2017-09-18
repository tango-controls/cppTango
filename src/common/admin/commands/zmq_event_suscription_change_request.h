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

/**
 * Fields guaranteed to be in lower case
 *
 */
struct ZmqSubscriptionChangeRequest
{
    std::string device_name;
    std::string object_name;
    std::string action_name;
    std::string event_name;

    ZmqSubscriptionChangeRequest(std::string device_name,
                                 std::string object_name,
                                 std::string action,
                                 std::string event_name)
        : device_name(std::move(StringUtils::to_lower_case(device_name))),
          object_name(std::move(StringUtils::to_lower_case(object_name))),
          action_name(std::move(StringUtils::to_lower_case(action))),
          event_name(std::move(StringUtils::to_lower_case(event_name)))
    {}

    explicit ZmqSubscriptionChangeRequest(Tango::DevVarStringArray *argin) throw(std::invalid_argument)
    {
        if (argin->length() != 4)
            throw std::invalid_argument{"A string array of size = 4 is expected here"};

        device_name = StringUtils::to_lower_case(std::string{(*argin)[0]});
        object_name = StringUtils::to_lower_case(std::string{(*argin)[1]});
        action_name = StringUtils::to_lower_case(std::string{(*argin)[2]});
        event_name = StringUtils::to_lower_case(std::string{(*argin)[3]});
    }

    Tango::DevVarStringArray *to_DevVarStringArray() const
    {
        auto result = new Tango::DevVarStringArray{4};
        result->length(4);


        (*result)[0] = Tango::string_dup(device_name.c_str());
        (*result)[1] = Tango::string_dup(object_name.c_str());
        (*result)[2] = Tango::string_dup(action_name.c_str());
        (*result)[3] = Tango::string_dup(event_name.c_str());

        return result;
    }
};
}
}
}
}

#endif //TANGO_ZMQ_EVENT_SUSCRIPTION_CHANGE_REQUEST_H

//===================================================================================================================
//
// file :		zmq_event_subscription_info_response.h
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
#ifndef TANGO_ZMQ_EVENT_SUBSCRIPTION_INFO_RESPONSE_H
#define TANGO_ZMQ_EVENT_SUBSCRIPTION_INFO_RESPONSE_H

#include <string>
#include <utility>
#include <vector>
#include <idl/tango.h>
#include <tango.h>
namespace tango
{
namespace common
{
namespace admin
{
namespace commands
{

struct ZmqEventSubscriptionInfoResponse
{
    int tango_serve_lib_release;
    std::string heartbeat_endpoint;
    std::string event_endpoint;

    std::vector<std::pair<std::string, std::string>> alternative_endpoints;

    ZmqEventSubscriptionInfoResponse(int tango_serve_lib_release,
                                     string heartbeat_endpoint,
                                     string event_endpoint,
                                     vector<pair<string, string>> alternative_endpoints)
        : tango_serve_lib_release(tango_serve_lib_release),
          heartbeat_endpoint(std::move(heartbeat_endpoint)),
          event_endpoint(std::move(event_endpoint)),
          alternative_endpoints(std::move(alternative_endpoints))
    {}

    explicit ZmqEventSubscriptionInfoResponse(const Tango::DevVarLongStringArray *value)
    {
        tango_serve_lib_release = value->lvalue[0];

        heartbeat_endpoint = value->svalue[0];
        event_endpoint = value->svalue[1];

        //TODO
    }

    Tango::DevVarLongStringArray *to_DevVarLongStringArray()
    {
        auto value = new Tango::DevVarLongStringArray;

        value->lvalue.length(1);
        value->lvalue[0] = tango_serve_lib_release;

        value->svalue.length(2 + alternative_endpoints.size() * 2);
        value->svalue[0] = Tango::string_dup(heartbeat_endpoint.c_str());
        value->svalue[1] = Tango::string_dup(event_endpoint.c_str());

        for (size_t i = 2; i < alternative_endpoints.size(); ++i)
        {
            value->svalue[i] = Tango::string_dup(alternative_endpoints[i].first.c_str());
            value->svalue[i + 1] = Tango::string_dup(alternative_endpoints[i].first.c_str());
        }

        return value;
    }
};

}
}
}
}


#endif //TANGO_ZMQ_EVENT_SUBSCRIPTION_INFO_RESPONSE_H

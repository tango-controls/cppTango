//===================================================================================================================
//
// file :		zmq_event_subscription_change_response.h
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
#ifndef TANGO_ZMQ_EVENT_SUBSCRIPTION_CHANGE_RESPONSE_H
#define TANGO_ZMQ_EVENT_SUBSCRIPTION_CHANGE_RESPONSE_H

#include <string>
#include <vector>
#include <tango.h>
#include <idl/tango.h>
#include <tango/client/DeviceData.h>
#include <tango/server/tango_const.h>
#include <tango/server/except.h>
#include <memory>

namespace tango
{
namespace common
{
namespace admin
{
namespace commands
{

/**
 * This structure represents AdminDevice::ZmqSubscriptionChangeResponse
 *
 */
struct ZmqSubscriptionChangeResponse
{
    int tango_lib_release;
    int dev_idl_version;
    int zmq_sub_event_hwm;
    int rate;
    int ivl;
    int zmq_release_version;

    std::string heartbeat_endpoint;
    std::string event_endpoint;

    std::string zmq_topic;

    std::vector<std::pair<std::string, std::string>> alternative_endpoints;

    ZmqSubscriptionChangeResponse(int tango_lib_release,
                                  int dev_idl_version,
                                  int zmq_sub_event_hwm,
                                  int rate,
                                  int ivl,
                                  int zmq_release_version,
                                  std::string heartbeat_endpoint,
                                  std::string event_endpoint,
                                  std::string zmq_topic,
                                  std::vector<std::pair<std::string, std::string>> alternative_endpoints)
        : tango_lib_release(tango_lib_release), dev_idl_version(dev_idl_version), zmq_sub_event_hwm(zmq_sub_event_hwm),
          rate(rate), ivl(ivl), zmq_release_version(zmq_release_version), heartbeat_endpoint(std::move(
            heartbeat_endpoint)),
          event_endpoint(std::move(event_endpoint)), zmq_topic(std::move(zmq_topic)), alternative_endpoints(std::move(
            alternative_endpoints))
    {}

    explicit ZmqSubscriptionChangeResponse(Tango::DeviceData &response)
    {
        const Tango::DevVarLongStringArray *value = nullptr;

        bool extract_ok = response >> value;
        if (not(extract_ok))
        {
            Tango::Except::throw_exception(
                Tango::API_CommandFailed,
                "Could not extract DeviceData!",
                "ZmqChangeSubscriptionResponse::ZmqChangeSubscriptionResponse(DeviceData*)"
            );
        }

        tango_lib_release = value->lvalue[0];
        dev_idl_version = value->lvalue[1];
        zmq_sub_event_hwm = value->lvalue[2];
        rate = value->lvalue[3];
        ivl = value->lvalue[4];
        zmq_release_version = value->lvalue[5];


        heartbeat_endpoint = value->svalue[0];
        event_endpoint = value->svalue[1];

        bool has_alternative_endpoints = (value->svalue.length() - 3) > 0;
        if (has_alternative_endpoints)
        {
            alternative_endpoints = vector<pair<string, string>>{value->svalue.length() - 3};

            //TODO
        }

        zmq_topic = value->svalue[value->svalue.length() - 1];
    }

    Tango::DevVarLongStringArray *to_DevVarLongStringArray()
    {
        auto value = new Tango::DevVarLongStringArray;

        value->lvalue.length(6);

        value->lvalue[0] = tango_lib_release;
        value->lvalue[1] = dev_idl_version;
        value->lvalue[2] = zmq_sub_event_hwm;
        value->lvalue[3] = rate;
        value->lvalue[4] = ivl;
        value->lvalue[5] = zmq_release_version;

        value->svalue.length(3 + alternative_endpoints.size() * 2);
        value->svalue[0] = Tango::string_dup(heartbeat_endpoint.c_str());
        value->svalue[1] = Tango::string_dup(event_endpoint.c_str());

        for (size_t i = 2; i < alternative_endpoints.size(); ++i)
        {
            value->svalue[i] = Tango::string_dup(alternative_endpoints[i].first.c_str());
            value->svalue[i + 1] = Tango::string_dup(alternative_endpoints[i].first.c_str());
        }

        value->svalue[value->svalue.length() - 1] = Tango::string_dup(zmq_topic.c_str());

        return value;
    }
};
}
}
}
}

#endif //TANGO_ZMQ_EVENT_SUBSCRIPTION_CHANGE_RESPONSE_H

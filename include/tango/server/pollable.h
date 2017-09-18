//===================================================================================================================
//
// file :		pollable.h
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
#ifndef TANGO_POLLABLE_H
#define TANGO_POLLABLE_H

#include <string>

namespace Tango
{

/**
 * Pollable mixin
 */
class Pollable
{
protected:
    std::string zmq_subscription_topic;
public:
    std::string get_zmq_subscription_topic()
    { return zmq_subscription_topic; }
    template<typename T>
    void set_zmq_subscription_topic(T v)
    { this->zmq_subscription_topic = std::move(v); }
};

}


#endif //TANGO_POLLABLE_H

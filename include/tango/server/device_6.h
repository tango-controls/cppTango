//===================================================================================================================
//
// file :		device_6.h
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
#ifndef TANGO_DEVICE_6_H
#define TANGO_DEVICE_6_H

#include <tango.h>

namespace Tango {

    void handle_cmd_info_enum_labels(DevVarStringArray *, const vector<string> &);

    class Device_6Impl : public virtual POA_Tango::Device_6,
                         public Device_5Impl {
    public:
        Device_6Impl(DeviceClass *device_class, string &dev_name) : Device_5Impl(device_class, dev_name) {
            idl_version = 6;
        }

        Device_6Impl(DeviceClass *device_class, string &dev_name, string &desc) : Device_5Impl(device_class, dev_name,
                                                                                               desc) {
            idl_version = 6;
        }

        Device_6Impl(DeviceClass *device_class, string &dev_name, string &desc, DevState dev_state, string &dev_status)
                : Device_5Impl(device_class, dev_name, desc, dev_state, dev_status) {
            idl_version = 6;
        }

        Device_6Impl(DeviceClass *device_class, const char *dev_name, const char *desc = "A TANGO device",
                     DevState dev_state = Tango::UNKNOWN, const char *dev_status = StatusNotSet)
            : Device_5Impl(device_class, dev_name, desc, dev_state, dev_status) {
            idl_version = 6;
        }

        virtual ~Device_6Impl() = default;

        /**
         *
         * @param cmd_name
         * @return CommandInfo with enum labels
         */
        DevCmdInfo_6 *command_query_6(const char *cmd_name) override;

        DevCmdInfoList_6 *command_list_query_6() override;

    private:
        friend void handle_cmd_info_enum_labels(DevVarStringArray *result, const vector<string> &enum_labels);
    };

}//EOF namespace Tango

#endif //TANGO_DEVICE_6_H

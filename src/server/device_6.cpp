//===================================================================================================================
//
// file :		device_6.cpp
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

#include <tango.h>

Tango::DevCmdInfo_6 *Tango::Device_6Impl::command_query_6(const char *cmd_name)
{
    cout4 << "Device_6Impl::command_query_6 arrived" << endl;

    DevCmdInfo_2 *cmdInfo_2 = command_query_2(cmd_name);

    DevCmdInfo_6 *result = new DevCmdInfo_6();
    result->cmd_name = cmdInfo_2->cmd_name;
    result->level = cmdInfo_2->level;
    result->cmd_tag = cmdInfo_2->cmd_tag;
    result->in_type = cmdInfo_2->in_type;
    result->out_type = cmdInfo_2->out_type;
    result->in_type_desc = cmdInfo_2->in_type_desc;
    result->out_type_desc = cmdInfo_2->out_type_desc;

    string cmd(cmd_name);
    Command *cmd_ptr = get_cmd_ptr(cmd);

    vector<string> &in_enum_labels = cmd_ptr->get_ext()->in_enum_labels;
    vector<string> &out_enum_labels = cmd_ptr->get_ext()->out_enum_labels;

    handle_cmd_info_enum_labels(&(result->in_enum_labels), in_enum_labels);
    handle_cmd_info_enum_labels(&(result->out_enum_labels), out_enum_labels);

    cout4 << "Leaving Device_6Impl::command_query_6" << endl;
    return result;
}

Tango::DevCmdInfoList_6 *Tango::Device_6Impl::command_list_query_6()
{
    cout4 << "Device_6Impl::command_list_query_6 arrived" << endl;

    DevCmdInfoList_2 *cmdInfoList_2 = command_list_query_2();

    DevCmdInfoList_6 *result = new DevCmdInfoList_6();
    result->length(cmdInfoList_2->length());

    for (size_t i = 0, size = cmdInfoList_2->length(); i < size; ++i) {
        (*result)[i] = *command_query_6((*cmdInfoList_2)[i].cmd_name);
    }

    cout4 << "Leaving Device_6Impl::command_list_query_6" << endl;
    return result;
}

void Tango::handle_cmd_info_enum_labels(Tango::DevVarStringArray *result, const vector<string> &enum_labels) {
    *result = DevVarStringArray();
    result->length(enum_labels.size());

    for (size_t i = 0, size = enum_labels.size(); i < size; ++i) {
        (*result)[i] = string_dup(enum_labels[i].c_str());
    }
}

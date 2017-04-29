//===================================================================================================================
//
// file :		device_command_info.cxx
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
#include <tango/client/device/device_command_info.hxx>

namespace Tango {


    CommandInfo newCommandInfo(const DevCmdInfo &devCmdInfo) {
        CommandInfo result;

        result.cmd_name = devCmdInfo.cmd_name;
        result.cmd_tag = devCmdInfo.cmd_tag;
        result.in_type = devCmdInfo.in_type;
        result.out_type = devCmdInfo.out_type;
        result.in_type_desc = devCmdInfo.in_type_desc;
        result.out_type_desc = devCmdInfo.out_type_desc;
        result.disp_level = Tango::OPERATOR;
        return(result);
    }

    CommandInfo newCommandInfo(const DevCmdInfo_2 &devCmdInfo_2) {
        CommandInfo result;

        result.cmd_name = devCmdInfo_2.cmd_name;
        result.cmd_tag = devCmdInfo_2.cmd_tag;
        result.in_type = devCmdInfo_2.in_type;
        result.out_type = devCmdInfo_2.out_type;
        result.in_type_desc = devCmdInfo_2.in_type_desc;
        result.out_type_desc = devCmdInfo_2.out_type_desc;
        result.disp_level = devCmdInfo_2.level;
        return(result);
    }


    CommandInfo newCommandInfo(const DevCmdInfo_3 &devCmdInfo_3) {
        CommandInfo result;

        result.cmd_name = devCmdInfo_3.cmd_name;
        result.cmd_tag = devCmdInfo_3.cmd_tag;
        result.in_type = devCmdInfo_3.in_type;
        result.out_type = devCmdInfo_3.out_type;
        result.in_type_desc = devCmdInfo_3.in_type_desc;
        result.out_type_desc = devCmdInfo_3.out_type_desc;
        result.disp_level = devCmdInfo_3.level;

        fill_vector(result.in_enum_labels, devCmdInfo_3.in_enum_labels);
        fill_vector(result.out_enum_labels, devCmdInfo_3.out_enum_labels);

        return(result);
    }

    void fill_vector(vector<string> &dst, const DevVarStringArray &src) {
        dst.resize(src.length());
        for(size_t i = 0, size = src.length(); i < size; ++i){
            dst[i] = src[i].in();
        }
    }
}//namespace Tango

//
// Created by ingvord on 3/26/17.
//

#include <tango.h>
#include <tango/client/device/device_command_info.hxx>

namespace Tango {


    CommandInfo newCommandInfo(const DevCmdInfo &devCmdInfo){
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

    CommandInfo newCommandInfo(const DevCmdInfo_2 &devCmdInfo_2){
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


    CommandInfo newCommandInfo(const DevCmdInfo_3 &devCmdInfo_3){
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

    void fill_vector(vector<string> &dst, const DevVarStringArray &src){
        dst.resize(src.length());
        for(size_t i = 0, size = src.length(); i < size; ++i){
            dst[i] = src[i].in();
        }
    }
}//namespace Tango

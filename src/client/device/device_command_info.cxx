//
// Created by ingvord on 3/26/17.
//

#include <tango.h>
#include <tango/client/device/device_command_info.hxx>

namespace Tango {


    CommandInfo newCommandInfo(DevCmdInfo_var devCmdInfo_var){
        CommandInfo result;

        result.cmd_name = devCmdInfo_var->cmd_name;
        result.cmd_tag = devCmdInfo_var->cmd_tag;
        result.in_type = devCmdInfo_var->in_type;
        result.out_type = devCmdInfo_var->out_type;
        result.in_type_desc = devCmdInfo_var->in_type_desc;
        result.out_type_desc = devCmdInfo_var->out_type_desc;
        result.disp_level = Tango::OPERATOR;
        return(result);
    }

    CommandInfo newCommandInfo(DevCmdInfo_2_var devCmdInfo_2_var){
        CommandInfo result;

        result.cmd_name = devCmdInfo_2_var->cmd_name;
        result.cmd_tag = devCmdInfo_2_var->cmd_tag;
        result.in_type = devCmdInfo_2_var->in_type;
        result.out_type = devCmdInfo_2_var->out_type;
        result.in_type_desc = devCmdInfo_2_var->in_type_desc;
        result.out_type_desc = devCmdInfo_2_var->out_type_desc;
        result.disp_level = devCmdInfo_2_var->level;
        return(result);
    }


    CommandInfo newCommandInfo(DevCmdInfo_3_var devCmdInfo_3_var){
        CommandInfo result;

        result.cmd_name = devCmdInfo_3_var->cmd_name;
        result.cmd_tag = devCmdInfo_3_var->cmd_tag;
        result.in_type = devCmdInfo_3_var->in_type;
        result.out_type = devCmdInfo_3_var->out_type;
        result.in_type_desc = devCmdInfo_3_var->in_type_desc;
        result.out_type_desc = devCmdInfo_3_var->out_type_desc;
        result.disp_level = devCmdInfo_3_var->level;

        fill_vector(result.in_enum_labels, devCmdInfo_3_var->in_enum_labels);
        fill_vector(result.out_enum_labels, devCmdInfo_3_var->out_enum_labels);

        return(result);
    }

    void fill_vector(vector<string>& dst, DevVarStringArray& src){
        dst.resize(src.length());
        for(size_t i = 0, size = src.length(); i < size; ++i){
            dst[i] = src[i].in();
        }
    }
}//namespace Tango

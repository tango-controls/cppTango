//
// Created by ingvord on 3/13/17.
//

#include <tango/server/command_with_enum_arg.h>

namespace Tango {
namespace server {

    CommandWithEnumArg::CommandWithEnumArg(String name, Tango::CmdArgType out_type, String in_desc,
                                           String out_desc): Tango::Command(name, Tango::CmdArgType::DEV_ENUM, out_type, in_desc, out_desc),
                                                             _enum_labels({}) {
    }

    CommandWithEnumArg::Set &CommandWithEnumArg::get_enum_labels() {
        return this->_enum_labels;
    }

    void CommandWithEnumArg::set_enum_labels(CommandWithEnumArg::Set &labels) {
        this->_enum_labels = labels;
    }
}

}

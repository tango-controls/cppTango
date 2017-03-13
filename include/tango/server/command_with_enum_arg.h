//
// Created by ingvord on 3/13/17.
//

#ifndef TANGO_COMMAND_WITH_ENUM_ARG_H
#define TANGO_COMMAND_WITH_ENUM_ARG_H

#include <set>
#include "command.h"

namespace Tango {
namespace server {
    class CommandWithEnumArg : public Tango::Command {
        using String = std::string;
        using Set = std::set<String>;
    public:
        CommandWithEnumArg(String, Tango::CmdArgType, String, String);
    protected:
        Set _enum_labels;
    public:
        Set& get_enum_labels();
        void set_enum_labels(Set&);
    };
}
}

#endif //TANGO_COMMAND_WITH_ENUM_ARG_H

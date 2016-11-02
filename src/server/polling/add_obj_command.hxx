//
// Created by ingvord on 11/1/16.
//

#pragma once

#include "command.hxx"

namespace Tango{
namespace polling {
    class AddObjCommand : public Command {
    public:
        AddObjCommand(DeviceImpl *dev, string &&name, PollObjType type, long index, int new_upd);

        virtual void operator()(PollThread &poll_thread) override;

        virtual void execute(PollThread &poll_engine) override;

        virtual operator std::string() override;
    };
}
}//Tango



//
// Created by ingvord on 11/1/16.
//

#pragma once

#include "command.hxx"

namespace Tango {
namespace polling {
    class AddTriggerCommand : public Command {
    public:
        AddTriggerCommand(DeviceImpl *dev, PollObjType type, long index);

        virtual void operator()(PollThread &poll_thread) override;

        virtual void execute(PollThread &poll_engine) override;

        virtual operator std::string() override;
    };
}//polling
}//Tango



//
// Created by ingvord on 11/1/16.
//

#pragma once

#include "command.hxx"

namespace Tango {
    namespace polling {
        class TriggerPollingCommand : public Command{
        public:
            TriggerPollingCommand(Tango::DeviceImpl *dev, string &&name,
                                              Tango::PollObjType type);

            virtual void operator()(PollThread &poll_thread) override;

            virtual void execute(PollThread &poll_thread) override;

            virtual operator std::string() override;

        };
    }//polling
}//Tango



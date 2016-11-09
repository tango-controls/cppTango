//
// Created by ingvord on 11/1/16.
//

#pragma once

#include "command.hxx"

namespace Tango {
    namespace polling {
        class UpdatePollPeriodCommand : public Command {
            /**
             *
             * @return true if work item was found in work list
             */
            bool update_polled_obj(PollThread &);
        public:
            UpdatePollPeriodCommand(DeviceImpl *dev, string &&name, PollObjType type, long index,
                                    int new_upd);

            virtual void operator()(PollThread &poll_thread) override;

            virtual void execute(PollThread &poll_engine) override;

            virtual operator std::string() override;
        };
    }//polling
}//Tango



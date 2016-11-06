//
// Created by ingvord on 11/1/16.
//

#pragma once

#include <string>
#include <memory>
#include <experimental/optional>
#include <tango.h>
//#include <tango/server/pollthread.h>

#include "helpers.hxx"

namespace Tango {
    class DeviceImpl;

    namespace polling {
        class Command {
            friend class PollThread;
        protected:
//            using DeviceImplPtr = std::shared_ptr<DeviceImpl>;
            //TODO replace with shared
            DeviceImpl* 	dev_;			// The device pointer (servant)
            PollCmdCode		code_;		    // The command code
            std::string		obj_name_;			// Object name
            PollObjType		obj_type_;			// Object type (cmd/attr)
            long			index_;			// Index in the device poll_list
            std::chrono::milliseconds				new_upd_;		// New update period (For upd period com.)

            Command(DeviceImpl* dev, PollCmdCode code, string&& name, PollObjType type, long index, int new_upd):
                    dev_{dev},
                    code_{code},
                    obj_name_{move(name)},
                    obj_type_{type},
                    index_{index},
                    new_upd_{new_upd}
                    {}
            virtual ~Command(){}
        public:
            virtual void operator()(PollThread &poll_thread) = 0;
            virtual void execute(PollThread &poll_thread) = 0;
            virtual operator std::string() = 0;
        };

    }
}



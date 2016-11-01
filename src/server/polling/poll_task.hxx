//
// Created by ingvord on 11/1/16.
//

#pragma once

#include <future>

namespace Tango {
    class WorkItem;

    namespace polling {
        class PollTask {
            WorkItem &work_;

            //+----------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::poll_cmd
            //
            // description :
            //		Execute a command and store the result in the device ring buffer
            //
            // args :
            //		in :
            // 			- to_do : The work item
            //
            //----------------------------------------------------------------------------------------------------------------
            void poll_cmd();

            //+---------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::poll_attr
            //
            // description :
            //		Read attribute and store the result in the device ring buffer
            //
            // args :
            //		in :
            // 			- to_do : The work item
            //
            //----------------------------------------------------------------------------------------------------------------
            void poll_attr();

            void execute();

            template<typename T>
            void robb_data(T &, T &);

            template<typename T>
            void copy_remaining(T &, T &);

        public:
            PollTask(WorkItem &work);

            std::future<void> operator()();
        };
    }
}//Tango



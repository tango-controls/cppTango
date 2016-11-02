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



            template<typename T>
            void steal_data(T &, T &);


            //------------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::copy_remaining
            //
            // description :
            //      Copy from one AttributeValue_X structure to another one data elements which are not the data themselves
            //
            // argument :
            //		in :
            //			- attr_value : The origin attribute value
            //      out :
            //          - new_attr_value : The new attribute value
            //
            //------------------------------------------------------------------------------------------------------------------
            template<typename T>
            void copy_remaining(T &, T &);

        public:
            PollTask(WorkItem &work);

            std::future<void> operator()();

            void execute();
        };
    }
}//Tango



//
// Created by ingvord on 11/1/16.
//

#pragma once

#include "../threading/enhanced_thread.hxx"

namespace Tango {
    class PollThread;
    class WorkItem;

    namespace polling {
        class PollingThread {
            threading::enhanced_thread thread_;
            PollThread& engine_;

            void run();

            //+---------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::err_out_of_sync
            //
            // description :
            //		To force one event if the polling thread has discarded one work item because it is late
            //
            // args :
            //		in :
            // 			- to_do : The work item
            //
            //----------------------------------------------------------------------------------------------------------------
            void err_out_of_sync(WorkItem&);
            //+----------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::tune_list
            //
            // description :
            //		This method tunes the work list.
            //
            // args :
            //		in :
            // 			- from_needed : Set to true if the delta between work should be at least equal to the
            //							time needed to execute the previous work
            //			- min_delta : Min. delta between polling works when from_needed is false
            //
            //----------------------------------------------------------------------------------------------------------------
            void tune_list(bool from_needed, long min_delta);
        public:
            PollingThread(PollThread &engine_);

            const threading::enhanced_thread& start(){ return thread_.start();}

            //+----------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::compute_sleep_time
            //
            // description :
            //		This method computes how many mS the thread should sleep before the next poll time. If this time is
            //		negative and greater than a pre-defined threshold, the polling is discarded.
            //
            //----------------------------------------------------------------------------------------------------------------
            std::chrono::milliseconds compute_next_sleep();
        };
    }//polling
}//Tango



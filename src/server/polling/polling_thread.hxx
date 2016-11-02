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



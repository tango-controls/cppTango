//
// Created by ingvord on 11/1/16.
//

#pragma once

#include "../threading/enhanced_thread.hxx"

namespace Tango {
    class PollThread;
    class WorkItem;

    namespace polling {


        /**
         *
         * Implements Tango polling algorithm
         */
        class PollingThread {
            threading::enhanced_thread thread_;
            PollThread& engine_;

            void run();
        public:
            PollingThread(PollThread &);

            std::thread::id id(){
                return thread_.as_std_thread().get_id();
            }

            void interrupt(){
                thread_.interrupt();
            }

            const threading::enhanced_thread& start(){ return thread_.start();}
        };
    }//polling
}//Tango



//
// Created by ingvord on 10/30/16.
//

#pragma once

#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
//#include <type_traits>

namespace Tango {
    class RepeatedTask {
        std::chrono::milliseconds delay_;
        std::atomic_bool aborted_;
    protected:
        std::thread thread_;
        template <typename Duration>
        RepeatedTask(Duration);
        virtual ~RepeatedTask();
        virtual void execute_internal() = 0;
    public:
        void abort();
    private:
        void execute();
    };

}//Tango
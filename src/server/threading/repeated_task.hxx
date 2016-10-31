//
// Created by ingvord on 10/30/16.
//

#pragma once

#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
//#include <type_traits>

#include "enhanced_thread.hxx"

namespace Tango {
    class RepeatedTask {
        std::chrono::milliseconds delay_;
    protected:
        threading::enhanced_thread thread_;
        template <typename  T, typename Duration>
        RepeatedTask(T&&, Duration);
        virtual ~RepeatedTask();
        virtual void execute() = 0;
    public:
        void start();
        void abort();
    private:
        void run();
    };

}//Tango
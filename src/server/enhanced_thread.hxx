//
// Created by ingvord on 10/31/16.
//

#pragma once

#include <thread>
#include <functional>
#include <condition_variable>
#include <atomic>

namespace Tango {
    namespace threading {
        class interrupted_exception: public std::exception {
            //TODO
        };

        class enhanced_thread {
            std::function<void()> bind_;
            std::thread thread_{};
            std::string name_;
            std::atomic_bool interrupted_{false};
            std::condition_variable monitor_;
            std::mutex monitor_guard_;
        public:
            template <typename T, typename... Args>
            enhanced_thread(T&& name, Args&&... args):
                    name_{std::forward<T>(name)},
                    bind_{std::bind(std::forward<Args>(args)...)}
            {}
            ~enhanced_thread();
            const std::string& name();
            bool interrupted();
            void start();
            //TODO return status instead of exception
            template <typename Duration>
            void sleep_for(Duration) throw(interrupted_exception);
            void interrupt();
            std::thread& as_std_thread();
        };
    }//threading
}//Tango



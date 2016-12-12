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
            std::string name_;
            std::function<void()> bind_;
            mutable std::thread thread_{};
            mutable std::atomic_bool interrupted_{false};
            mutable std::condition_variable monitor_;
            std::mutex monitor_guard_;
        public:
            enhanced_thread() = default;
            template <typename T>
            enhanced_thread(T&& name):
                    name_{std::forward<T>(name)}
            {}
            template <typename T, typename... Args>
            enhanced_thread(T&& name, Args&&... args):
                    name_{std::forward<T>(name)},
                    bind_{std::bind(std::forward<Args>(args)...)}
            {}
            enhanced_thread(enhanced_thread&&) = default;
            ~enhanced_thread();
            const std::string& name() const noexcept;
            bool interrupted() const noexcept;
            const enhanced_thread & start() const ;

            //TODO rename to start using mock or inheritance
            /**
             * Runs specified command in this thread
             *
             * For testing
             *
             * @return
             */
            const enhanced_thread & start_synchronously() const ;
            /**
             *
             * @return true if thread has been interrupted
             */
            template <typename Duration>
            bool sleep_for(Duration);
            void interrupt() const noexcept ;
            const std::thread& as_std_thread() const noexcept ;
            std::thread& as_std_thread() noexcept ;
        };
    }//threading
}//Tango



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
            mutable std::thread thread_{};
            std::string name_;
            mutable std::atomic_bool interrupted_{false};
            mutable std::condition_variable monitor_;
            std::mutex monitor_guard_;
        public:
            void swap(const enhanced_thread&) noexcept;

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



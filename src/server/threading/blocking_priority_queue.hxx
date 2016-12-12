//
// Created by ingvord on 11/7/16.
//

#pragma once

#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace Tango {
    namespace threading {
        template<typename Value, typename Comparator = std::function<bool(const Value&, const Value&)>, typename Queue = std::priority_queue<Value,std::deque<Value>, Comparator>>
        class blocking_priority_queue : public Queue {
        protected:
            using Lock = std::unique_lock<std::mutex>;

            std::mutex queue_guard_;
            std::condition_variable monitor_;
        public:
            blocking_priority_queue(const Comparator& comparator): Queue{comparator, std::deque<Value>{}}{}

            void push(const Value& item){
                {
                    Lock lock{queue_guard_};
                    Queue::push(item);
                }
                if (Queue::size() == 1) monitor_.notify_one();
            }
            Value pop() {
                Lock lock{queue_guard_};
                monitor_.wait(lock, [this]() { return !Queue::empty(); });

                Value rv = Queue::top();
                Queue::pop();
                return rv;
            }
        };
    }//threading
}//Tango



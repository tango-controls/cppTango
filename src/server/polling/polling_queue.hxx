//
// Created by ingvord on 11/1/16.
//

#pragma once

#include <queue>
#include <deque>
#include <functional>
#include <experimental/optional>
#include "../threading/blocking_priority_queue.hxx"


namespace Tango {
    class WorkItem;
    namespace polling {
        bool compare_work_items(const WorkItem&, const WorkItem&);
        //TODO extract template
        class PollingQueue : public threading::blocking_priority_queue<WorkItem> {
        public:
            PollingQueue();

            PollingQueue(PollingQueue&&) = default;

            void erase(std::function<bool(const WorkItem&)>);

            void for_each(std::function<void(const WorkItem&)>);

            /**
             * Removes found element
             *
             * @return optional found element
             */
            std::experimental::optional<WorkItem>
            find_if(std::function<bool(const WorkItem & )>);

            int64_t count_if(std::function<bool(const WorkItem & )>);
        };
    }//polling
}//Tango



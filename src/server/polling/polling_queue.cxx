//
// Created by ingvord on 11/1/16.
//

#include "polling_queue.hxx"

#include <tango.h>

using namespace std;

using namespace Tango;

bool Tango::polling::compare_work_items(const WorkItem &lhs, const WorkItem &rhs) {
    //making comparision by `>` effectively forces reverse order - which is what we need, i.e. smaller values go first
    return lhs.wake_up_date > rhs.wake_up_date;
}

Tango::polling::PollingQueue::PollingQueue() : threading::blocking_priority_queue<WorkItem>(compare_work_items) {}

void Tango::polling::PollingQueue::erase(function<bool(const WorkItem&)> predicate){
    Lock lock{queue_guard_};
    c.erase(std::remove_if(c.begin(), c.end(), predicate), c.end());
}


std::experimental::optional<WorkItem>
Tango::polling::PollingQueue::find_if(function<bool(const WorkItem&)> predicate) {
    Lock lock{queue_guard_};
    auto it = std::remove_if(c.begin(), c.end(), predicate);

    if(it == c.end()) {
        return experimental::optional<WorkItem>{};
    }

    auto result = experimental::make_optional(*it);
    c.erase(it, c.end());
    return result;
}


void polling::PollingQueue::for_each(std::function<void(const WorkItem &)> f) {
    Lock lock{queue_guard_};
    std::for_each(c.begin(), c.end(), f);
}

int64_t polling::PollingQueue::count_if(std::function<bool(const WorkItem &)> predicate) {
    Lock lock{queue_guard_};
    return std::count_if(c.begin(), c.end(), predicate);
}



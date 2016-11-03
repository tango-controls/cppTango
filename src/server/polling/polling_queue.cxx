//
// Created by ingvord on 11/1/16.
//

#include "polling_queue.hxx"

#include <tango.h>

using namespace std;

using namespace Tango;

bool Tango::polling::compare_work_items(WorkItem &lhs, WorkItem &rhs) {
    if (lhs.wake_up_date.tv_sec < rhs.wake_up_date.tv_sec)
        return true;
    else if (lhs.wake_up_date.tv_sec == rhs.wake_up_date.tv_sec) {
        return lhs.wake_up_date.tv_usec < rhs.wake_up_date.tv_usec;
    } else {
        return false;
    }
}

Tango::polling::PollingQueue::PollingQueue() : priority_queue(compare_work_items, std::list<WorkItem>{}) {}

void Tango::polling::PollingQueue::erase(function<bool(const WorkItem&)> predicate){
    c.erase(std::remove_if(c.begin(), c.end(), predicate), c.end());
}


std::experimental::optional<WorkItem>
Tango::polling::PollingQueue::find_if(function<bool(const WorkItem&)> predicate) {
    auto it = std::find_if(c.begin(), c.end(), predicate);

    if(it == c.end()) return experimental::optional<WorkItem>{};
    return experimental::make_optional(*it);
}

std::experimental::optional<WorkItem>
Tango::polling::PollingQueue::remove_if(function<bool(const WorkItem&)> predicate) {
    auto it = std::remove_if(c.begin(), c.end(), predicate);

    if(it == c.end()) return experimental::optional<WorkItem>{};
    return experimental::make_optional(*it);
}


void polling::PollingQueue::for_each(std::function<void(const WorkItem &)> f) {
    std::for_each(c.begin(), c.end(), f);
}

int64_t polling::PollingQueue::count_if(std::function<bool(const WorkItem &)> predicate) {
    return std::count_if(c.begin(), c.end(), predicate);
}



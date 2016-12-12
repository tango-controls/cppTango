//
// Created by ingvord on 10/31/16.
//

#include "asymmetric_unbound_blocking_queue.hxx"

using namespace std;

using Lock = unique_lock<mutex>;

template<typename Item>
Tango::threading::asymmetric_unbound_blocking_queue<Item>::asymmetric_unbound_blocking_queue():
        queue_{} {}

template<typename Item>
void Tango::threading::asymmetric_unbound_blocking_queue<Item>::push(Item &&item) {
    {
        Lock lock{queue_guard_};
        queue_.push_back(move(item));
    }
    if (queue_.size() == 1) monitor_.notify_one();
}

template<typename Item>
Item Tango::threading::asymmetric_unbound_blocking_queue<Item>::pop() {
    Lock lock{queue_guard_};
    monitor_.wait(lock, [this]() { return !queue_.empty(); });

    Item rv = queue_.front();
    queue_.pop_front();
    return rv;
}

//
// Created by ingvord on 10/31/16.
//

#include "asymmetric_unbounded_blocking_queue.hxx"

using namespace std;
using namespace Tango;

using Lock = unique_lock<mutex>;

threading::asymmetric_unbounded_blocking_queue::asymmetric_unbounded_blocking_queue(threading::asymmetric_unbounded_blocking_queue::size_type size):
queue_{size}
{}

template <typename Item>
void threading::asymmetric_unbounded_blocking_queue::push(Item &&item) {
    Lock lock{guard_};
    queue_.push_back(move(item));
    if(queue_.size() == 1) monitor_.notify_one();
}

template <typename Item>
Item threading::asymmetric_unbounded_blocking_queue::pop() {
    Item rv;
    {
        Lock lock{guard_};
        if (queue_.empty())
            monitor_.wait(lock, [this]() { return !queue_.empty(); });

        rv = queue_.pop_front();
    }
    return rv;
}

//
// Created by ingvord on 10/30/16.
//

#include <tango.h>
#include <tango/server/eventsupplier.h>
#include "heartbeat_thread.hxx"


using namespace std;
using namespace Tango;


HeartbeatThread::HeartbeatThread(RootAttRegistry& root_att_registry, EventSupplier* suppliers...) :
        thread_{&HeartbeatThread::run, this},
        interrupted_{false},
        suppliers_{suppliers},
        heartbeat_ctr_{0},
        root_att_registry_{root_att_registry}
{}

HeartbeatThread::~HeartbeatThread() {
    if (thread_.joinable())
        thread_.join();
}

thread::id HeartbeatThread::id() {
    return thread_.get_id();
}

void HeartbeatThread::interrupt() {
    interrupted_.store(true);
}

void HeartbeatThread::send_heartbeat() {
    cout5 << "----------> Time = " << chrono::system_clock::now().time_since_epoch().count() << " Sending event heartbeat" << endl;

    for_each(suppliers_.begin(), suppliers_.end(), [](EventSupplier* event_supplier) {
        if (event_supplier->get_one_subscription_cmd())
            event_supplier->push_heartbeat_event();
    });
}

void HeartbeatThread::run() {
    while (!interrupted_.load()) {
        //sleep

        send_heartbeat();
        if (heartbeat_ctr_++ % 3 == 0) {
            root_att_registry_.auto_unsub();
        }
    }
}



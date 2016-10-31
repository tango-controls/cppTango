//
// Created by ingvord on 10/30/16.
//

#include <tango.h>
#include <tango/server/eventsupplier.h>
#include "heartbeat_task.hxx"


using namespace std;
using namespace Tango;


HeartbeatTask::HeartbeatTask(RootAttRegistry& root_att_registry, vector<EventSupplier*>&& suppliers) :
RepeatedTask(chrono::seconds{9}),
suppliers_{move(suppliers)},
heartbeat_ctr_{0},
root_att_registry_{root_att_registry}
{}

thread::id HeartbeatTask::id() {
    return thread_.get_id();
}

void HeartbeatTask::send_heartbeat() {
    cout5 << "----------> Time = " << chrono::system_clock::now().time_since_epoch().count() << " Sending event heartbeat" << endl;

    for_each(suppliers_.begin(), suppliers_.end(), [](EventSupplier* event_supplier) {
        if (event_supplier->get_one_subscription_cmd())
            event_supplier->push_heartbeat_event();
    });
}

void HeartbeatTask::execute() {
        send_heartbeat();
        if (heartbeat_ctr_++ % 3 == 0) {
            root_att_registry_.auto_unsub();
        }
}



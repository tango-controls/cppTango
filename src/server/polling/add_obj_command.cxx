//
// Created by ingvord on 11/1/16.
//

#include "add_obj_command.hxx"
#include "helpers.hxx"

#include <sstream>
#include <cassert>
#include "polling_queue.hxx"

using namespace std;

using chrono::milliseconds;
using chrono::duration_cast;
using chrono::system_clock;

using namespace Tango;

polling::AddObjCommand::AddObjCommand(DeviceImpl *dev, string &&name,
                                      PollObjType type, long index, int new_upd) : Command(dev, POLL_ADD_OBJ,
                                                                                           move(name), type,
                                                                                           index,
                                                                                           new_upd) {}

void polling::AddObjCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void polling::AddObjCommand::execute(PollThread &poll_engine) {
    cout5 << "Execute an Add object command" << endl;

    PollObj *poll_list_item = dev_->get_poll_obj_list()[index_];//TODO pass as a parameter from dserverpoll

    auto wo = poll_engine.new_work_item(dev_, *poll_list_item);

    assert(wo.update.count() != 0);// "Trying to add trigger object is not expected here"*/);


    wo.wake_up_date = duration_cast<milliseconds>(system_clock::now().time_since_epoch());//TODO was: poll_engine.now;
    if (new_upd_.count() != 0) {
        cout5 << "Received a delta from now of " << new_upd_.count() << endl;
        wo.wake_up_date += new_upd_;
    }
    //TODO protected queue in case many incoming requests
    poll_engine.add_work_item(wo);//TODO set flag - queue has changed, catch the flag in polling thread
    poll_engine.set_need_two_tuning(true);
}

polling::AddObjCommand::operator std::string() {
    stringstream out;
    out << "AddObjCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

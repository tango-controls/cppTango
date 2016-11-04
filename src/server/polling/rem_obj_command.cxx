//
// Created by ingvord on 11/1/16.
//

#include "rem_obj_command.hxx"
#include "polling_queue.hxx"

using namespace std;

using namespace Tango;

Tango::polling::RemObjCommand::RemObjCommand(DeviceImpl *dev, string &&name, PollObjType type, long index) : Command(dev, POLL_REM_OBJ,
                                                                                                                     move(name), type,
                                                                                                                     index,
                                                                                                                     -1) {}

void polling::RemObjCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void polling::RemObjCommand::execute(PollThread &poll_engine) {
    cout5 << "Execute a Rem object command" << endl;

    poll_engine.remove_work_item_by(dev_, obj_name_, obj_type_);
}

polling::RemObjCommand::operator std::string() {
    stringstream out;
    out << "RemObjCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

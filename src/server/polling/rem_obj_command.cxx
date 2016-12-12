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


    auto work_item = poll_engine.find_work_item(dev_, obj_type_, obj_name_);

    if (work_item) {


        work_item->name.erase(remove_if(
                work_item->name.begin(),
                work_item->name.end(),
                [this](const string &name) { return name == obj_name_; }
        ), work_item->name.end());


        if (!work_item->name.empty()) {
            poll_engine.add_work_item(*work_item);
        }
    }
}

polling::RemObjCommand::operator std::string() {
    stringstream out;
    out << "RemObjCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

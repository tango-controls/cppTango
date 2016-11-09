//
// Created by ingvord on 11/1/16.
//

#include "rem_ext_trigger_command.hxx"

Tango::polling::RemExtTriggerCommand::RemExtTriggerCommand(DeviceImpl *dev, string &&name, PollObjType type, long index)
        : Command(dev, POLL_REM_EXT_TRIG_OBJ,
                  move(name), type, index,
                  -1) {}

void Tango::polling::RemExtTriggerCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::RemExtTriggerCommand::execute(PollThread &poll_engine) {
    cout5 << "Received a Ext Trig Rem object command" << endl;


    auto work_item = poll_engine.find_trigger(dev_, obj_type_, obj_name_);

    if (work_item) {
        work_item->name.erase(remove_if(
                work_item->name.begin(),
                work_item->name.end(),
                [this](const string &name) { return name == obj_name_; }
        ), work_item->name.end());

        if (!work_item->name.empty()) {
            poll_engine.add_trigger(*work_item);
        }
    }
}

Tango::polling::RemExtTriggerCommand::operator std::string() {
    stringstream out;
    out << "RemExtTriggerCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

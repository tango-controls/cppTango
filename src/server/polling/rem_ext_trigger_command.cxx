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

    poll_engine.remove_trigger(dev_, obj_name_, obj_type_);
}

Tango::polling::RemExtTriggerCommand::operator std::string() {
    stringstream out;
    out << "RemExtTriggerCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

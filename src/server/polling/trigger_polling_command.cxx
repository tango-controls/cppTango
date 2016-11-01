//
// Created by ingvord on 11/1/16.
//

#include "trigger_polling_command.hxx"

Tango::polling::TriggerPollingCommand::TriggerPollingCommand(Tango::DeviceImpl *dev, string &&name,
                                                             Tango::PollObjType type)
        : Command(dev, POLL_START, name, type, -1,
                  -1) {}

void Tango::polling::TriggerPollingCommand::operator()(Tango::PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::TriggerPollingCommand::execute(Tango::PollThread &poll_thread) {
    //TODO launch one_more trigger task
}

Tango::polling::TriggerPollingCommand::operator std::string() {
    stringstream out;
    out << "TriggerPollingCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

//
// Created by ingvord on 11/1/16.
//

#include "trigger_polling_command.hxx"
#include "poll_task.hxx"
#include "polling_queue.hxx"

Tango::polling::TriggerPollingCommand::TriggerPollingCommand(Tango::DeviceImpl *dev, string &&name,
                                                             Tango::PollObjType type)
        : Command(dev, POLL_START, move(name), type, -1,
                  -1) {}

void Tango::polling::TriggerPollingCommand::operator()(Tango::PollThread &poll_thread) {
    return execute(poll_thread);
}

//TODO return future
void Tango::polling::TriggerPollingCommand::execute(Tango::PollThread &poll_engine) {
    experimental::optional<WorkItem> work_item = poll_engine.find_trigger(dev_, obj_type_, obj_name_);

    if(work_item){
            PollTask poll_task{work_item.value(), poll_engine};

            poll_task.execute();
    }
}

Tango::polling::TriggerPollingCommand::operator std::string() {
    stringstream out;
    out << "TriggerPollingCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

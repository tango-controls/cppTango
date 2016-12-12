//
// Created by ingvord on 11/1/16.
//

#include "add_trigger_command.hxx"
#include "helpers.hxx"
#include "polling_queue.hxx"

Tango::polling::AddTriggerCommand::AddTriggerCommand(DeviceImpl *dev, PollObjType type, long index)
        : Command(dev,
                  POLL_ADD_OBJ,
                  "",
                  type,
                  index,
                  0) {}

void Tango::polling::AddTriggerCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::AddTriggerCommand::execute(PollThread &poll_engine) {
    cout5 << "Execute an Add object command" << endl;

    PollObj *poll_list_item = dev_->get_poll_obj_list()[index_];//TODO pass as a parameter from dserverpoll

    chrono::milliseconds update{poll_list_item->get_upd()};
    auto work_item = poll_engine.find_work_item(dev_, poll_list_item->get_type(), update, poll_list_item->get_name());

    if (work_item) {
        //TODO should we really return in this case
        work_item->name.push_back(poll_list_item->get_name());
        return;
    }

    auto wo = poll_engine.new_work_item(dev_, *poll_list_item);

    assert(wo.update.count() == 0);// "Trying to add non-trigger object is not expected here"*/);

    poll_engine.add_trigger(wo);
}

Tango::polling::AddTriggerCommand::operator std::string() {
        stringstream out;
        out << "AddTriggerCommand[device=" << dev_->get_name() << ";"
            << "obj_type=" << obj_type_ << ";"
            << "]";
        return out.str();
}

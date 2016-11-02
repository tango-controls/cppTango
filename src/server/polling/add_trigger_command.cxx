//
// Created by ingvord on 11/1/16.
//

#include "add_trigger_command.hxx"
#include "helpers.hxx"
#include "polling_queue.hxx"

Tango::polling::AddTriggerCommand::AddTriggerCommand(Tango::DeviceImpl *dev, string &&name,
                                                     Tango::PollObjType type, long index, int new_upd) : Command(dev,
                                                                                                                 POLL_ADD_OBJ,
                                                                                                                 move(name),
                                                                                                                 type,
                                                                                                                 index,
                                                                                                                 new_upd) {}

void Tango::polling::AddTriggerCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::AddTriggerCommand::execute(PollThread &poll_engine) {
    cout5 << "Execute an Add object command" << endl;

    PollObj *poll_list_item = dev_->get_poll_obj_list()[index_];//TODO pass as a parameter from dserverpoll

    auto work_item = poll_engine.find_work_item(dev_, poll_list_item->get_type(), poll_list_item->get_upd());

    if (work_item) {
        work_item->name.push_back(poll_list_item->get_name());
        return;
    }

    auto wo = poll_engine.new_work_item(dev_, *poll_list_item);

    assert(wo.update == 0);// "Trying to add non-trigger object is not expected here"*/);

    wo.wake_up_date.tv_sec = 0;
    wo.wake_up_date.tv_usec = 0;
    poll_engine.ext_trig_works.push(wo);
}

Tango::polling::AddTriggerCommand::operator std::string() {
        stringstream out;
        out << "AddTriggerCommand[device=" << dev_->get_name() << ";"
            << "obj_type=" << obj_type_ << ";"
            << "obj_name=" << obj_name_ << ";"
            << "]";
        return out.str();
}

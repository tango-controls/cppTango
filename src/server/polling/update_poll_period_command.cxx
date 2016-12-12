//
// Created by ingvord on 11/1/16.
//

#include "update_poll_period_command.hxx"
#include "polling_queue.hxx"

Tango::polling::UpdatePollPeriodCommand::UpdatePollPeriodCommand(Tango::DeviceImpl *dev, string &&name,
                                                                 Tango::PollObjType type, long index,
                                                                 int new_upd) : Command(dev, POLL_UPD_PERIOD,
                                                                                        move(name), type, index,
                                                                                        new_upd) {}

void Tango::polling::UpdatePollPeriodCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

//
// Update polling period
// Several cases has to be implemented here.
// 1 - A classical command from the external world. In this case updating
//     polling period means removing the already inserted object from the work list,
//     compute its new polling time and insert it in the work list with its new
//     polling time and polling period
// 2 - This is executed by the polling thread itself
// 2-1 - The command updates polling period for another object: idem than previous
// 2-2 - The commands updates polling period for the object it is actually polled.
//       In this case, the object is not in the work list. It has been removed from the
//	 work list at the beginning of the "one_more_poll" method and is memorized there
//	 Therefore, simply stores new polling period in a data member. The "one_more_poll"
//	 method will get its new polling period before re-inserting the object in the work
// 	 list with the new update period.
//	 We detect this case because the object is not in any work list (either the work
//	 list or the trigger list)
//
void Tango::polling::UpdatePollPeriodCommand::execute(PollThread &poll_engine) {
    cout5 << "Execute an update polling period command" << endl;


    auto poll_list = dev_->get_poll_obj_list();
    auto &poll_list_item = *poll_list[index_];

    auto work_item = poll_engine.find_work_item(dev_, obj_type_, obj_name_);
    if(work_item){
        if(new_upd_.count() == 0)
            poll_engine.add_trigger(work_item.value());
        else
            update_polled_obj(poll_engine);
    } else {
        work_item = poll_engine.find_trigger(dev_, obj_type_, obj_name_);

        if (work_item) {
            WorkItem wo = poll_engine.new_work_item(dev_, poll_list_item);
            wo.type = obj_type_;
            wo.update = {new_upd_};
            poll_engine.add_work_item(wo);
        } else {
            poll_engine.auto_upd.push_back(new_upd_);
            poll_engine.auto_name.push_back(obj_name_);
        }
    }
}

bool Tango::polling::UpdatePollPeriodCommand::update_polled_obj(PollThread &poll_engine) {
    if (poll_engine.empty()) return false;
    auto work_item = poll_engine.find_work_item(dev_, obj_type_, obj_name_);

    if (work_item) {
        work_item->type = obj_type_;
        work_item->update = {new_upd_};

        poll_engine.add_work_item(work_item.value());


        poll_engine.reset_tune_counter();


        return true;
    } else {
        poll_engine.rem_upd.push_back(new_upd_);
        poll_engine.rem_name.push_back(obj_name_);
        return false;
    }
}

Tango::polling::UpdatePollPeriodCommand::operator std::string() {
    stringstream out;
    out << "UpdatePollPeriodCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "new_update_period=" << new_upd_.count() << ";"
        << "]";
    return out.str();
}

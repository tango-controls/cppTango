//
// Created by ingvord on 11/1/16.
//

#include "add_obj_command.hxx"
#include "helpers.hxx"

#include <sstream>

using namespace std;

using namespace Tango;

polling::AddObjCommand::AddObjCommand(DeviceImpl *dev, string &&name,
                                      PollObjType type, long index, int new_upd) : Command(dev, POLL_ADD_OBJ,
                                                                                           move(name), type,
                                                                                           index,
                                                                                           new_upd) {}

void polling::AddObjCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void polling::AddObjCommand::execute(PollThread &poll_thread) {
    cout5 << "Execute an Add object command" << endl;

    PollObj *poll_list_item = dev_->get_poll_obj_list()[index_];//TODO pass as a parameter from dserverpoll

    experimental::optional <WorkItem> work_item = find_work_item(poll_thread.polling_bef_9, *this, *poll_list_item, poll_thread.works);

    if (work_item) {
        work_item->name.push_back(poll_list_item->get_name());
        return;
    }

    auto wo = poll_thread.new_work_item(dev_, *poll_list_item);

    assert(wo.update != 0);// "Trying to add trigger object is not expected here"*/);

    wo.wake_up_date = poll_thread.now;
    if (new_upd_ != 0) {
        cout5 << "Received a delta from now of " << new_upd_ << endl;
        T_ADD(wo.wake_up_date, new_upd_ * 1000);
    }
    poll_thread.insert_in_list(wo);
    unsigned long nb_works = poll_thread.works.size();
    poll_thread.tune_ctr = (nb_works << 2);
    poll_thread.need_two_tuning = true;
}

polling::AddObjCommand::operator std::string() {
    stringstream out;
    out << "AddObjCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

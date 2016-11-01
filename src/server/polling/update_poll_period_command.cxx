//
// Created by ingvord on 11/1/16.
//

#include "update_poll_period_command.hxx"

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
void Tango::polling::UpdatePollPeriodCommand::execute(PollThread &poll_thread) {
    cout5 << "Execute an update polling period command" << endl;


    auto poll_list = dev_->get_poll_obj_list();
    auto &poll_list_item = *poll_list[index_];

    list<WorkItem>::iterator ite;
    vector<WorkItem>::iterator et_ite;

    bool found_in_work_list = false;

    if (new_upd_ != 0) {
        size_t i, nb_elt;
        nb_elt = poll_thread.works.size();
        ite = poll_thread.works.begin();

        if (nb_elt != 0) {
            bool found = false;

            for (i = 0; i < nb_elt; i++) {
                if (ite->dev == dev_) {
                    if (ite->type == obj_type_) {

                        vector<string>::iterator ite_str;
                        for (ite_str = ite->name.begin(); ite_str != ite->name.end(); ++ite_str) {
                            if (*ite_str == obj_name_) {
                                ite->name.erase(ite_str);
                                if (ite->name.empty() == true) {
                                    poll_thread.works.erase(ite);
                                }

                                found = true;
                                found_in_work_list = true;
                                break;
                            }
                        }

                        if (found == true)
                            break;
                    }
                }
                ++ite;
            }

            WorkItem tmp_work = poll_thread.new_work_item(dev_, poll_list_item);
            tmp_work.type = obj_type_;
            tmp_work.update = new_upd_;
            tmp_work.name.push_back(obj_name_);

            poll_thread.compute_new_date(poll_thread.now, new_upd_);

            tmp_work.wake_up_date = poll_thread.now;

            poll_thread.add_insert_in_list(tmp_work);
            poll_thread.tune_ctr = 0;
            found_in_work_list = true;

            if (found == false) {
                poll_thread.rem_upd.push_back(new_upd_);
                poll_thread.rem_name.push_back(obj_name_);
            }

        }
    } else {

//
// First, remove object from polling list and insert it in externally triggered list
//

        size_t i, nb_elt;
        nb_elt = poll_thread.works.size();
        ite = poll_thread.works.begin();
        for (i = 0; i < nb_elt; i++) {
            if (ite->dev == dev_) {
                if (ite->type == obj_type_) {
                    bool found = false;
                    vector<string>::iterator ite_str;
                    for (ite_str = ite->name.begin(); ite_str != ite->name.end(); ++ite_str) {
                        if (*ite_str == obj_name_) {
                            ite->name.erase(ite_str);
                            if (ite->name.empty() == true)
                                poll_thread.works.erase(ite);
                            found = true;
                            found_in_work_list = true;
                            break;
                        }
                    }
                    if (found == true)
                        break;
                }
            }
            ++ite;
        }

        WorkItem wo = poll_thread.new_work_item(dev_, poll_list_item);

        poll_thread.ext_trig_works.push_back(wo);
    }

//
// If not found in work list, it should be in the externally triggered object. Therefore, remove it from externally
// triggered list and insert it in work list. If not found in work list and in trig list, we are in case
// 2-2 as described above (polling thread itself updating polling period of the object it actually polls)
//

    if (found_in_work_list == false) {
        bool found = false;
        for (et_ite = poll_thread.ext_trig_works.begin();
             et_ite != poll_thread.ext_trig_works.end(); ++et_ite) {
            if (et_ite->dev == dev_) {
                if (et_ite->type == obj_type_) {
                    if (et_ite->name[0] == obj_name_) {
                        poll_thread.ext_trig_works.erase(et_ite);
                        found = true;
                        break;
                    }
                }
            }
        }

        if (found == true) {
            WorkItem wo = poll_thread.new_work_item(dev_, poll_list_item);
            wo.type = obj_type_;
            wo.update = new_upd_;
            wo.name.push_back(obj_name_);
            poll_thread.insert_in_list(wo);
        } else {
            poll_thread.auto_upd.push_back(new_upd_);
            poll_thread.auto_name.push_back(obj_name_);
        }
    }
}

Tango::polling::UpdatePollPeriodCommand::operator std::string() {
    stringstream out;
    out << "UpdatePollPeriodCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "new_update_period=" << new_upd_ << ";"
        << "]";
    return out.str();
}

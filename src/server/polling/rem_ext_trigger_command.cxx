//
// Created by ingvord on 11/1/16.
//

#include "rem_ext_trigger_command.hxx"

Tango::polling::RemExtTriggerCommand::RemExtTriggerCommand(Tango::DeviceImpl *dev, string &&name,
                                                           Tango::PollObjType type, long index,
                                                           int new_upd) : Command(dev, POLL_REM_EXT_TRIG_OBJ,
                                                                                  move(name), type, index,
                                                                                  new_upd) {}

void Tango::polling::RemExtTriggerCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::RemExtTriggerCommand::execute(PollThread &poll_thread) {
    cout5 << "Received a Ext Trig Rem object command" << endl;

    vector<WorkItem>::iterator et_ite;

    for (et_ite = poll_thread.ext_trig_works.begin();
         et_ite != poll_thread.ext_trig_works.end(); ++et_ite) {
        if (et_ite->dev == dev_) {
            if (et_ite->type == obj_type_) {
                if (et_ite->name[0] == obj_name_) {
                    poll_thread.ext_trig_works.erase(et_ite);
                    break;
                }
            }
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

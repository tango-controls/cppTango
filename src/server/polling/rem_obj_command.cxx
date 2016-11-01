//
// Created by ingvord on 11/1/16.
//

#include "rem_obj_command.hxx"

using namespace std;

using namespace Tango;

Tango::polling::RemObjCommand::RemObjCommand(DeviceImpl *dev, string &&name, PollObjType type, long index) : Command(dev, POLL_REM_OBJ,
                                                                                                                     move(name), type,
                                                                                                                     index,
                                                                                                                     -1) {}

void polling::RemObjCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void polling::RemObjCommand::execute(PollThread &poll_thread) {
    cout5 << "Execute a Rem object command" << endl;

    auto ite = poll_thread.works.begin();
    //TODO find work item; delete if found
    for (size_t i = 0, size = poll_thread.works.size(); i < size; i++) {
        if (ite->dev == dev_) {
            if (ite->type == obj_type_) {
                vector<string>::iterator ite_str;
                bool found = false;
                for (ite_str = ite->name.begin(); ite_str != ite->name.end(); ++ite_str) {
                    if (*ite_str == obj_name_) {
                        ite->name.erase(ite_str);
                        if (ite->name.empty() == true)
                            poll_thread.works.erase(ite);
                        found = true;
                        break;
                    }
                }
                if (found == true)
                    break;
            }
        }
        ++ite;
    }
}

polling::RemObjCommand::operator std::string() {
    stringstream out;
    out << "RemObjCommand[device=" << dev_->get_name() << ";"
        << "obj_type=" << obj_type_ << ";"
        << "obj_name=" << obj_name_ << ";"
        << "]";
    return out.str();
}

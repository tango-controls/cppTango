//
// Created by ingvord on 11/1/16.
//

#include "rem_dev_command.hxx"

Tango::polling::RemDevCommand::RemDevCommand(Tango::DeviceImpl *dev) : Command(dev, POLL_REM_DEV,
                                                                               "",
                                                                               POLL_ATTR,
                                                                               -1,
                                                                               -1) {}

void Tango::polling::RemDevCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::RemDevCommand::execute(PollThread &poll_thread) {
    cout5 << "Execute a Rem device command" << endl;

#ifdef _TG_WINDOWS_

    list<WorkItem>::iterator ite;
        vector<WorkItem>::iterator et_ite;

        nb_elt = works.size();
        ite = works.begin();
        for (i = 0;i < nb_elt;i++)
        {
            if (ite->dev == PollThread::dev_to_del)
            {
                ite = works.erase(ite);
            }
            else
                ++ite;
        }
        nb_elt = ext_trig_works.size();
        et_ite = ext_trig_works.begin();
        for (i = 0;i < nb_elt;i++)
        {
            if (et_ite->dev == PollThread::dev_to_del)
            {
                et_ite = ext_trig_works.erase(et_ite);
            }
            else
                ++et_ite;
        }
#else
    auto predicate = [this](const WorkItem &work_item) { return work_item.dev == dev_; };
    poll_thread.works.remove_if(predicate);

    poll_thread.ext_trig_works.erase(
            remove_if(poll_thread.ext_trig_works.begin(),
                      poll_thread.ext_trig_works.end(),
                      predicate),
            poll_thread.ext_trig_works.end());
#endif
}

Tango::polling::RemDevCommand::operator std::string() {
    return "RemDevCommand[device=" + dev_->get_name() + "]";
}

//
// Created by ingvord on 11/1/16.
//

#include "rem_dev_command.hxx"
#include "polling_queue.hxx"

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
    poll_thread.remove_work_items_by(dev_);
#endif
}

Tango::polling::RemDevCommand::operator std::string() {
    return "RemDevCommand[device=" + dev_->get_name() + "]";
}

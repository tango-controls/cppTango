//
// Created by ingvord on 11/1/16.
//

#include "polling_thread.hxx"
#include "poll_task.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>
#include "polling_queue.hxx"
#include "pollthread.h"

using namespace std;

Tango::polling::PollingThread::PollingThread(Tango::PollThread &engine) :
        thread_{"PollingThread", &PollingThread::run, this},
        engine_(engine) {}

void Tango::polling::PollingThread::run() {
    //TODO memory fence or somethig to fetch latest values from engine
    while (!engine_.polling_stop) {
        bool interrupted = thread_.sleep_for(compute_next_sleep());
        if (interrupted) return;

        if(engine_.works.empty()) return;//TODO wait?

#ifdef _TG_WINDOWS_
        _ftime(&now_win);
			now.tv_sec = (unsigned long)now_win.time;
			now.tv_usec = (long)now_win.millitm * 1000;
#else
        gettimeofday(&now,NULL);
#endif
        now.tv_sec = now.tv_sec - DELTA_T;





        WorkItem tmp = engine_.works.top();
        engine_.works.pop();


        PollTask task{tmp};
        task.execute();

//
// For case where the polling thread itself modify the polling period of the object it already polls
//

        if (engine_.auto_upd.empty() == false) {
            for (size_t loop = 0; loop < engine_.auto_upd.size(); loop++) {
                vector<string>::iterator pos = remove(tmp.name.begin(), tmp.name.end(), engine_.auto_name[loop]);
                tmp.name.erase(pos, tmp.name.end());
            }

            if (tmp.name.empty() == false) {
                tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                engine_.works.push(tmp);
            }

            list<WorkItem>::iterator ite;
            vector<WorkItem>::iterator et_ite;

            for (size_t loop = 0; loop < engine_.auto_upd.size(); loop++) {
                int auto_upd_value = engine_.auto_upd[loop];
                auto work_item = engine_.works.find_if(
                        [auto_upd_value](const WorkItem &work_item) {
                            return work_item.dev == tmp.dev &&
                                   work_item.type == tmp.type &&
                                   work_item.update == auto_upd_value;
                        });

                if (work_item)
                    work_item->name.push_back(engine_.auto_name[loop]);
                else {
                    WorkItem new_tmp = engine_.new_work_item(tmp.dev, poll_obj);
                    new_tmp.update = engine_.auto_upd[loop];
                    new_tmp.name.push_back(engine_.auto_name[loop]);

                    engine_.now = engine_.compute_new_date(engine_.now, local_cmd.new_upd);
                    new_tmp.wake_up_date = engine_.now;
                    engine_.works.push(new_tmp);
                }
            }

            engine_.auto_upd.clear();
            engine_.auto_name.clear();
        }

//
// Compute new polling date and insert work in list
//

        else {
            if (engine_.rem_upd.empty() == false) {
                for (size_t loop = 0; loop < engine_.rem_upd.size(); loop++) {
                    vector<string>::iterator pos = remove(tmp.name.begin(), tmp.name.end(), engine_.rem_name[loop]);
                    tmp.name.erase(pos, tmp.name.end());
                }

                if (tmp.name.empty() == false) {
                    tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                    engine_.works.push(tmp);
                }

                engine_.rem_upd.clear();
                engine_.rem_name.clear();
            } else {
                tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                engine_.works.push(tmp);
            }
        }

        engine_.tune_ctr--;


        engine_.adjust_work_items();

    }
}



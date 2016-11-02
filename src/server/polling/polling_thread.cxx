//
// Created by ingvord on 11/1/16.
//

#include "polling_thread.hxx"
#include "poll_task.hxx"

#include <tango.h>
#include "polling_queue.hxx"

using namespace std;

Tango::polling::PollingThread::PollingThread(Tango::PollThread &engine) :
        thread_{"PollingThread", &PollingThread::run, this},
        engine_(engine) {}

void Tango::polling::PollingThread::run() {
    cout3 << "Thread " << thread_.name() << " has started." << endl;
    //TODO memory fence or somethig to fetch latest values from engine
    while (!engine_.polling_stop) {
        bool interrupted = thread_.sleep_for(compute_next_sleep());
        if (interrupted) return;

        if(engine_.works.empty()) return;//TODO wait?

        engine_.set_time();


        WorkItem tmp = engine_.works.top();
        engine_.works.pop();


        PollTask task{tmp};
        task.execute();

        engine_.adjust_work_items(tmp);
    }
}



//
// Created by ingvord on 10/30/16.
//

#include <tango.h>
#include "repeated_task.hxx"
#include <ostream>

using namespace std;
using namespace Tango;

template <typename T, typename Duration>
RepeatedTask::RepeatedTask(T&& name, Duration delay):
delay_{chrono::duration_cast<chrono::milliseconds>(delay)},
thread_{move(name), &RepeatedTask::run, this}
{
}

template RepeatedTask::RepeatedTask(string&&, chrono::minutes);

template RepeatedTask::RepeatedTask(string&&, chrono::seconds);

template RepeatedTask::RepeatedTask(string&&, chrono::milliseconds);

RepeatedTask::~RepeatedTask() {
    cout3 << "RepeatedTask::~RepeatedTask()" << endl;
    if(thread_.as_std_thread().joinable()) {
        cout3 << "RepeatedTask: interrupt thread[" << thread_.name() << "]" << endl;
        thread_.interrupt();
        thread_.as_std_thread().join();
    }
    cout3 << "RepeatedTask::~RepeatedTask() exiting" << endl;
}

void RepeatedTask::start() {
    thread_.start();
}

void RepeatedTask::abort() {
    thread_.interrupt();
}

void RepeatedTask::run() {
    cout3 << thread_.name() << "::run()" << endl;
    while(!thread_.interrupted()){
        bool interrupted = thread_.sleep_for(delay_);
        if(interrupted) return;
        cout3 << thread_.name() << "::execute()" << endl;
        execute();
    }
}

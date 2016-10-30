//
// Created by ingvord on 10/30/16.
//

#include <tango.h>
#include "repeated_task.hxx"
#include <ostream>

using namespace std;
using namespace Tango;

template <typename Duration>
RepeatedTask::RepeatedTask(Duration delay):
delay_{chrono::duration_cast<chrono::milliseconds>(delay)},
aborted_{false}
{
    thread_ = thread{&RepeatedTask::execute, this};
}

template RepeatedTask::RepeatedTask(chrono::minutes);

template RepeatedTask::RepeatedTask(chrono::seconds);

template RepeatedTask::RepeatedTask(chrono::milliseconds);

RepeatedTask::~RepeatedTask() {
    if(thread_.joinable())
        thread_.join();
}

void RepeatedTask::abort() {
    aborted_.store(true);
}

void RepeatedTask::execute() {
    cout3 << "RepeatedTask::execute()" << endl;
    while(!aborted_){
        //TODO wait on conditional variable to implement interruption
        this_thread::sleep_for(delay_);
        execute_internal();
    }
}

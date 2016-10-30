//
// Created by ingvord on 10/31/16.
//

#include "named_thread.hxx"

using namespace std;
using namespace Tango;

Tango::thread::named_thread::~named_thread() {
    if(thread_.joinable())
        thread_.join();
}

std::string Tango::thread::named_thread::name() {
    return std::string();
}

const std::thread &Tango::thread::named_thread::as_thread() {
    return thread_;
}

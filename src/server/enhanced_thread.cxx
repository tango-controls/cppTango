//
// Created by ingvord on 10/31/16.
//

#include "enhanced_thread.hxx"

using namespace std;
using namespace Tango;

Tango::thread::enhanced_thread::~enhanced_thread() {
    if(thread_.joinable())
        thread_.join();
}

std::string Tango::thread::enhanced_thread::name() {
    return std::string();
}

const std::thread &Tango::thread::enhanced_thread::as_thread() {
    return thread_;
}

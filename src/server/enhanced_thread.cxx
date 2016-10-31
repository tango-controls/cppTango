//
// Created by ingvord on 10/31/16.
//

#include "enhanced_thread.hxx"

using namespace std;

static_assert(chrono::steady_clock::duration{1} <= chrono::milliseconds{1},
              "This steady_clock implementation has an outrageously coarse duration");

using Lock = unique_lock<mutex>;

Tango::threading::enhanced_thread::~enhanced_thread() {
    if(thread_.joinable())
        thread_.join();
}

const std::string& Tango::threading::enhanced_thread::name() {
    return move(std::string());
}

template <typename Duration>
void Tango::threading::enhanced_thread::sleep_for(Duration duration) throw(interrupted_exception) {
    Lock lock{monitor_guard_};
    cv_status _cv_status;
    do {
        _cv_status = monitor_.wait_for(lock, duration);//TODO how long will it wait in case spurious wake up
    } while(_cv_status != cv_status::timeout || !interrupted_);
    if(interrupted_) throw interrupted_exception{};
}

template void Tango::threading::enhanced_thread::sleep_for(chrono::milliseconds);

template void Tango::threading::enhanced_thread::sleep_for(chrono::seconds);

template void Tango::threading::enhanced_thread::sleep_for(chrono::minutes);

void Tango::threading::enhanced_thread::interrupt() {
    interrupted_.store(true);//TODO investigate memory model
    monitor_.notify_one();
}

bool Tango::threading::enhanced_thread::interrupted() {
    return interrupted_.load();//TODO investigate memory model
}

void Tango::threading::enhanced_thread::start() {
    thread_ = std::thread{bind_};
}

std::thread &Tango::threading::enhanced_thread::as_std_thread() {
    return thread_;
}

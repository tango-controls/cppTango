//
// Created by ingvord on 10/31/16.
//

#include "enhanced_thread.hxx"

using namespace std;

static_assert(chrono::steady_clock::duration{1} <= chrono::milliseconds{1},
              "This steady_clock implementation has an outrageously coarse duration");

using Lock = unique_lock<mutex>;

Tango::threading::enhanced_thread::~enhanced_thread() {
    if(thread_.joinable()) {
        interrupt();
        thread_.join();
    }
}

const std::string& Tango::threading::enhanced_thread::name() const noexcept {
    return name_;
}

template <typename Duration>
bool Tango::threading::enhanced_thread::sleep_for(Duration duration) {
    Lock lock{monitor_guard_};
    cv_status _cv_status;
    do {
        _cv_status = monitor_.wait_for(lock, duration);//TODO how long will it wait in case spurious wake up
    } while(_cv_status == cv_status::no_timeout && !interrupted_.load());
    return interrupted_.load();
}

template bool Tango::threading::enhanced_thread::sleep_for(chrono::milliseconds);

template bool Tango::threading::enhanced_thread::sleep_for(chrono::seconds);

template bool Tango::threading::enhanced_thread::sleep_for(chrono::minutes);

void Tango::threading::enhanced_thread::interrupt() const noexcept {
    interrupted_.store(true);//TODO investigate memory model
    monitor_.notify_one();
}

bool Tango::threading::enhanced_thread::interrupted() const noexcept  {
    return interrupted_.load();//TODO investigate memory model
}

const Tango::threading::enhanced_thread & Tango::threading::enhanced_thread::start() const {
    thread_ = std::thread{bind_};
    return *this;
}

const Tango::threading::enhanced_thread & Tango::threading::enhanced_thread::start_synchronously() const {
    bind_();
    return *this;
}

const std::thread &Tango::threading::enhanced_thread::as_std_thread() const noexcept  {
    return thread_;
}

std::thread &Tango::threading::enhanced_thread::as_std_thread() noexcept  {
    return thread_;
}

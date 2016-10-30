//
// Created by ingvord on 10/30/16.
//

#pragma once

#include <thread>
#include <atomic>
#include <vector>

namespace Tango {
    class EventSupplier;
    class RootAttRegistry;

    class HeartbeatThread {
        std::thread thread_;
        std::atomic_bool interrupted_;
        std::vector<EventSupplier*> suppliers_;
        uint32_t heartbeat_ctr_;
        RootAttRegistry& root_att_registry_;
    public:
        HeartbeatThread(RootAttRegistry&,EventSupplier* suppliers...);
        ~HeartbeatThread();
        std::thread::id id();
        void run();
        void interrupt();
    private:
        void send_heartbeat();
    };
}//Tango
//
// Created by ingvord on 10/30/16.
//

#pragma once

#include <memory>
#include <thread>
#include <atomic>
#include <vector>
#include "repeated_task.hxx"

namespace Tango {
    class EventSupplier;
    class RootAttRegistry;

    class HeartbeatTask : public RepeatedTask {
        std::vector<EventSupplier*> suppliers_;
        uint32_t heartbeat_ctr_;
        RootAttRegistry& root_att_registry_;
    public:
        HeartbeatTask(RootAttRegistry&,vector<EventSupplier*>&& suppliers);
        ~HeartbeatTask() = default;
        std::thread::id id();
        void execute() override ;
    private:
        void send_heartbeat();
    };

    using HeartbeatTask_ptr = std::unique_ptr<HeartbeatTask>;
}//Tango
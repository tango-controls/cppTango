//
// Created by ingvord on 10/30/16.
//

#pragma once

#include <repeated_task.hxx>

namespace Tango {
    class SubDevDiag;

    class StoreSubDevicesTask : public RepeatedTask {
        SubDevDiag& sub_dev_diag_;
    private:
        void execute_internal() override ;
    public:
        template <typename Duration>
        StoreSubDevicesTask(Duration, SubDevDiag&);
        ~StoreSubDevicesTask() = default;
    };


    typedef std::unique_ptr<StoreSubDevicesTask> StoreSubDevicesTask_ptr;

}//Tango
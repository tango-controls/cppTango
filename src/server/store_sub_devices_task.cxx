//
// Created by ingvord on 10/30/16.
//

#include <tango.h>
#include "store_sub_devices_task.hxx"
#include <tango/server/subdev_diag.h>

#include <iostream>


using namespace std;
using namespace Tango;

template <typename Duration>
StoreSubDevicesTask::StoreSubDevicesTask(Duration delay, SubDevDiag& sub_dev_diag):
RepeatedTask(delay),
sub_dev_diag_{sub_dev_diag}
{}

template StoreSubDevicesTask::StoreSubDevicesTask(chrono::minutes delay, SubDevDiag& sub_dev_diag);

void StoreSubDevicesTask::execute(){
    cout3 << "----------> Time = " << chrono::system_clock::now().time_since_epoch().count() << " Store sub device property data if needed!" << endl;

    sub_dev_diag_.store_sub_devices();
}
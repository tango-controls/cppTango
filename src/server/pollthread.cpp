static const char *RcsId = "$Id$\n$Name$";

//+=================================================================================================================
//
// file :               PollThread.cpp
//
// description :        C++ source code for the PollThread class. This class is used for the polling thread. The rule
//						of this thread is to regulary exceute command on device or read attribute and store result
//						in a ring buffer.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <tango/server/eventsupplier.h>

#ifdef _TG_WINDOWS_
#include <sys/timeb.h>
#else

#include <sys/time.h>

#endif

#include <iomanip>
#include <polling/command.hxx>

#include "threading/repeated_task.hxx"
#include "threading/asymmetric_unbound_blocking_queue.hxx"
//import template definitions
#include "polling/polling_queue.hxx"


namespace Tango {

    extern map<thread::id, string> kThreadNameMap;
    extern thread_local std::shared_ptr<PyData> kPerThreadPyData;

    namespace threading {
        template
        class asymmetric_unbound_blocking_queue<PollThCmd>;
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::Pollthread
//
// description :
//		The polling thread constructor.
//
// args :
//		in :
// 			- cmd : The buffer used to pass command to the polling thread
//			- m : The mutex used to protect the previous buffer
//			- heartbeat : Flag set to true if this polling thread is the thread used to send the heartbeat event
//
//------------------------------------------------------------------------------------------------------------------

    PollThread::PollThread(TangoMonitor &m, string &&name, bool polling_as_before_tango_9)
            : polling_stop(true),
              tune_ctr(1),
              need_two_tuning(false),
              name_(move(name)),
              previous_nb_late{0},
              polling_bef_9{polling_as_before_tango_9},
              thread_{},
              works{},
              ext_trig_works{}
    {
#ifdef _TG_WINDOWS_
        LARGE_INTEGER f;
        BOOL is_ctr;
        is_ctr = QueryPerformanceFrequency(&f);
        if (is_ctr != 0)
            ctr_frequency = 1000000.0 / (double)f.QuadPart;
        else
            ctr_frequency = 0.0;
#endif
    }

    thread::id PollThread::id(){
        return thread_.as_std_thread().get_id();
    }

    WorkItem PollThread::new_work_item(DeviceImpl *device, /*TODO const*/ PollObj &poll_obj) {
        //TODO forward arguments
        return {
                device,
                &(device->get_poll_obj_list()),
                now,
                poll_obj.get_upd(),
                poll_obj.get_type(),
                {poll_obj.get_name()},
                {0, 0}
        };
    }


    void PollThread::execute_cmd(polling::Command &&cmd) {
        cmd.execute(*this);


        if (Tango::Util::_tracelevel >= 4)
            print_work_items();

        return;
    }

    std::experimental::optional<WorkItem>
    PollThread::find_work_item(DeviceImpl *device, PollObjType type, long update) {
        experimental::optional<WorkItem> optional_work_item{};
        if (polling_bef_9) return optional_work_item;

        bool is_attr = type == POLL_ATTR;
        long idl_version = device->get_dev_idl_version();
        if (is_attr && idl_version >= 4) {
            return works.find_if([device, update, type](const WorkItem &work_item) {
                return work_item.dev == device && work_item.update == update && work_item.type == type;
            });
        }
        return optional_work_item;
    }

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::print_work_items
//
// description :
//		To print work list
//
//-----------------------------------------------------------------------------------------------------------------

    void PollThread::print_work_items() {
        works.for_each([](const WorkItem& work_item){
            ostringstream oss;
            copy(work_item.name.begin(), work_item.name.end()-1,
                      ostream_iterator<int>(oss, ","));

            oss << work_item.name.back();

            cout5 << "Dev name = " << work_item.dev->get_name() << ", obj name(s) = " << oss.str()
                  << ", next wake_up at " << +work_item.wake_up_date.tv_sec
                  << "," << setw(6) << setfill('0')
                  << work_item.wake_up_date.tv_usec << endl;
        });
    }

    void PollThread::add_or_push(WorkItem &new_work) {
        experimental::optional<WorkItem> work_item = find_work_item(new_work.dev, new_work.type, new_work.update);

        if(work_item)
            work_item->name.push_back(new_work.name[0]);
        else
            works.push(new_work);
    }

    std::experimental::optional<WorkItem> PollThread::remove_work_item(DeviceImpl * device, std::string obj_name, PollObjType obj_type) {
        std::experimental::optional<WorkItem> work_item = works.find_if([](const WorkItem &work_item) {
            return work_item.dev == device && work_item.type == obj_type &&
                   find_if(work_item.name.begin(), work_item.name.end(),
                           [](const string &name) { return name == obj_name; }) != work_item.name.end();
        });

        if (work_item) {
            work_item->name.erase(remove_if(
                    work_item->name.begin(),
                    work_item->name.end(),
                    [](const string &name) { return name == obj_name; }
            ), work_item->name.end());
            if (work_item->name.empty()){
                auto outer = work_item.value();
                works.erase([&outer](const WorkItem& inner){ return inner.dev == outer.dev && inner.type == outer.type;});
            }
        }

        return work_item;
    }

    std::experimental::optional<WorkItem> PollThread::remove_trigger(DeviceImpl * device, std::string obj_name, PollObjType obj_type) {
        std::experimental::optional<WorkItem> work_item = works.find_if([](const WorkItem &work_item) {
            return work_item.dev == device && work_item.type == obj_type &&
                   find_if(work_item.name.begin(), work_item.name.end(),
                           [](const string &name) { return name == obj_name; }) != work_item.name.end();
        });

        if (work_item) {
            work_item->name.erase(remove_if(
                    work_item->name.begin(),
                    work_item->name.end(),
                    [](const string &name) { return name == obj_name; }
            ), work_item->name.end());
            if (work_item->name.empty()){
                auto outer = work_item.value();
                works.erase([&outer](const WorkItem& inner){ return inner.dev == outer.dev && inner.type == outer.type;});
            }
        }

        return work_item;
    }

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::compute_new_date
//
// description :
//		This method computes the new poll date.
//
// args :
//		in :
// 			- time : The actual date
//			- upd : The polling update period (mS)
//
//------------------------------------------------------------------------------------------------------------------

    timeval PollThread::compute_new_date(timeval time, int upd) {
        double ori_d = (double) time.tv_sec + ((double) time.tv_usec / 1000000);
        double new_d = ori_d + ((double) (upd) / 1000);
        return {(long) new_d, (long) ((new_d - time.tv_sec) * 1000000)};
    }

    WorkItem::WorkItem(DeviceImpl *dev, vector<PollObj *> *poll_list, const timeval &wake_up_date, int update,
                       PollObjType type, const vector<string> &name, const timeval &needed_time) : dev(dev),
                                                                                                   poll_list(poll_list),
                                                                                                   wake_up_date(
                                                                                                           wake_up_date),
                                                                                                   update(update),
                                                                                                   type(type),
                                                                                                   name(name),
                                                                                                   needed_time(
                                                                                                           needed_time) {}
} // End of Tango namespace

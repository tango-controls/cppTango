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
              need_two_tuning{false},
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

    void PollThread::set_need_two_tuning(bool v) {
        size_t nb_works = works.size();
        tune_ctr = (nb_works << 2); //* 4
        need_two_tuning = v;
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

    std::chrono::milliseconds PollThread::compute_next_sleep() {
            //TODO use integral type to calculate time related values
            uint64_t next{};
            uint64_t diff{};
            uint64_t after_d = after.tv_sec + (after.tv_usec / 1000000);//TODO precision loss :/

            uint64_t sleep{};

            bool discard = false;
            u_int nb_late = 0;

            if (polling_bef_9) {
                discard = true;
            } else {

//
// Compute for how many items the polling thread is late
//
                works.for_each([](const WorkItem& work_item){
                    next = (double) work_item.wake_up_date.tv_sec + ((double) work_item.wake_up_date.tv_usec / 1000000);
                    diff = next - after_d;
                    if (diff < 0 && fabs(diff) > DISCARD_THRESHOLD)
                        nb_late++;
                });

//
// If we are late for some item(s):
//  - Seriously late (number of late items equal number of items) --> We will discard items
//  - Late for the first time: Poll immediately but memorize the number of items for which we are late
//  - Late again: If the number of late items increase --> Discard items
//

                if (nb_late != 0) {
                    if (nb_late == works.size()) {
                        cout5 << "Setting discard to true because nb_late == works.size() --> " << nb_late << endl;
                        discard = true;
                    } else {
                        if (previous_nb_late != 0) {
                            if (nb_late < previous_nb_late) {
                                previous_nb_late = nb_late;
                                cout5 << "Late but trying to catch up" << endl;
                            } else {
                                previous_nb_late = 0;
                                discard = true;
                            }
                        } else
                            previous_nb_late = nb_late;
                        sleep = -1;
                    }
                }
            }

//
// Analyse work list
//

//        cout5 << "discard = " << boolalpha << discard << endl;
            if (nb_late == 0 || discard == true) {
                previous_nb_late = 0;

                auto item = works.top();
                next = (double) item.wake_up_date.tv_sec +
                       ((double) item.wake_up_date.tv_usec / 1000000);
                diff = next - after_d;

                if (diff < 0) {
                    if (fabs(diff) < DISCARD_THRESHOLD)
                        sleep = -1;
                    else {
                        while ((diff < 0) && (fabs(diff) > DISCARD_THRESHOLD)) {
                            cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
                            WorkItem& tmp = const_cast<WorkItem&>(works.top());
                            if (tmp.type == POLL_ATTR)
                                err_out_of_sync(tmp);

                            tmp.wake_up_date = compute_new_date(tmp.wake_up_date, tmp.update);
                            works.push(tmp);
                            works.pop();
                            tune_ctr--;

                            WorkItem &top = const_cast<WorkItem&>(works.top());
                            next = (double) top.wake_up_date.tv_sec +
                                   ((double) top.wake_up_date.tv_usec / 1000000);
                            diff = next - after_d;
                        }

                        if (fabs(diff) < DISCARD_THRESHOLD)
                            sleep = -1;
                        else
                            sleep = (diff * 1000);
                    }
                } else
                    sleep = (diff * 1000);
            }

            cout5 << "Sleep for : " << sleep << endl;
            return chrono::milliseconds{sleep};
    }

    void PollThread::err_out_of_sync(WorkItem &to_do) {
        EventSupplier *event_supplier_nd = NULL;
        EventSupplier *event_supplier_zmq = NULL;

//
// Retrieve the event supplier(s) for this attribute
//

        size_t nb_obj = to_do.name.size();
        for (size_t ctr = 0; ctr < nb_obj; ctr++) {
            Attribute &att = to_do.dev->get_device_attr()->get_attr_by_name(to_do.name[ctr].c_str());

            if (att.use_notifd_event() == true && event_supplier_nd == NULL)
                event_supplier_nd = Util::instance()->get_notifd_event_supplier();
            if (att.use_zmq_event() == true && event_supplier_zmq == NULL)
                event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

            if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL)) {
                DevErrorList errs;
                errs.length(1);

                errs[0].severity = ERR;
                errs[0].reason = string_dup("API_PollThreadOutOfSync");
                errs[0].origin = string_dup("PollThread::err_out_of_sync");
                errs[0].desc = string_dup(
                        "The polling thread is late and discard this object polling.\nAdvice: Tune device server polling");

                DevFailed except(errs);
                long idl_vers = to_do.dev->get_dev_idl_version();

                struct EventSupplier::SuppliedEventData ad{};
                memset(&ad, 0, sizeof(ad));

                if (idl_vers > 4)
                    ad.attr_val_5 = &dummy_att5;
                else if (idl_vers == 4)
                    ad.attr_val_4 = &dummy_att4;
                else if (idl_vers == 3)
                    ad.attr_val_3 = &dummy_att3;
                else
                    ad.attr_val = &dummy_att;

//
// Fire event
//

                SendEventType send_event;
                if (event_supplier_nd != NULL)
                    send_event = event_supplier_nd->detect_and_push_events(to_do.dev, ad, &except, to_do.name[ctr],
                                                                           (struct timeval *) NULL);
                if (event_supplier_zmq != NULL) {
                    if (event_supplier_nd != NULL) {
                        vector<string> f_names;
                        vector<double> f_data;
                        vector<string> f_names_lg;
                        vector<long> f_data_lg;

                        if (send_event.change == true)
                            event_supplier_zmq->push_event_loop(to_do.dev, CHANGE_EVENT, f_names, f_data, f_names_lg,
                                                                f_data_lg, ad, att, &except);
                        if (send_event.archive == true)
                            event_supplier_zmq->push_event_loop(to_do.dev, ARCHIVE_EVENT, f_names, f_data, f_names_lg,
                                                                f_data_lg, ad, att, &except);
                        if (send_event.periodic == true)
                            event_supplier_zmq->push_event_loop(to_do.dev, PERIODIC_EVENT, f_names, f_data, f_names_lg,
                                                                f_data_lg, ad, att, &except);
                    } else
                        event_supplier_zmq->detect_and_push_events(to_do.dev, ad, &except, to_do.name[ctr],
                                                                   (struct timeval *) NULL);
                }
            }
        }
    }

    void PollThread::adjust_work_items() {
#ifdef _TG_WINDOWS_
        _ftime(&after_win);
			after.tv_sec = (unsigned long)after_win.time;
			after.tv_usec = (long)after_win.millitm * 1000;
#else
        gettimeofday(&after,NULL);
#endif
        after.tv_sec = after.tv_sec - DELTA_T;

        if (tune_ctr <= 0)
        {
            tune_list(true,0);
            if (need_two_tuning)
            {
                set_need_two_tuning(false);
            }
            else
                tune_ctr = POLL_LOOP_NB;
        }
    }

    void PollThread::tune_list(bool from_needed, long min_delta) {
        list::iterator ite, ite_next, ite_prev;

        unsigned long nb_works = works.size();
        cout4 << "Entering tuning list. The list has " << nb_works << " item(s)" << endl;

//
// Nothing to do if only one let in list
//

        if (nb_works < 2)
            return;

//
// If we try to tune the list with respect to works needed time, compute works needed time sum and find minimun update
// period
//

        if (from_needed == true) {
            unsigned long needed_sum = 0;
            unsigned long min_upd = 0;
            long max_delta_needed;

            for (ite = works.begin(); ite != works.end(); ++ite) {
                long needed_time_usec = (ite->needed_time.tv_sec * 1000000) + ite->needed_time.tv_usec;
                needed_sum = needed_sum + (unsigned long) needed_time_usec;

                unsigned long update_usec = (unsigned long) ite->update * 1000;

                if (ite == works.begin()) {
                    min_upd = update_usec;
                } else {
                    if (min_upd > update_usec)
                        min_upd = update_usec;
                }
            }

//
// In some cases, it is impossible to tune
//

            if (needed_sum > min_upd)
                return;
            else {
                long sleeping = min_upd - needed_sum;
                max_delta_needed = sleeping / (nb_works);
            }

//
// Now build a new tuned list
// Warning: On Windows 64 bits, long are 32 bits data. Convert everything to DevULong64 to be sure
// that we will have computation on unsigned 64 bits data
//
// To tune the list
// - Take obj j and compute when it should be polled (next_work)
// - Compute when object j-1 should be polled (prev_obj_work)
// - Compute the number of poll between these two dates (n)
// - Compute date of previous object polling just before "next_work"
// - Assign next_work to this date and add
//       the time needed to execute previous object polling
//		 the delta computed from the smallest upd and the obj number
//

            DevULong64 now_us = ((DevULong64) now.tv_sec * 1000000LL) + (DevULong64) now.tv_usec;
            DevULong64 next_tuning = now_us + (POLL_LOOP_NB * (DevULong64) min_upd);

            PollingQueue new_works{};
            new_works.push(works.front());

            ite = works.begin();
            ite_prev = new_works.begin();

            for (++ite; ite != works.end(); ++ite, ++ite_prev) {
                DevULong64 needed_time_usec = ((DevULong64) ite_prev->needed_time.tv_sec * 1000000) +
                                              (DevULong64) ite_prev->needed_time.tv_usec;
                WorkItem wo = *ite;
                DevULong64 next_work = ((DevULong64) wo.wake_up_date.tv_sec * 1000000LL) +
                                       (DevULong64) wo.wake_up_date.tv_usec;

                DevULong64 next_prev;
                if (next_work < next_tuning) {
                    DevULong64 prev_obj_work = ((DevULong64) ite_prev->wake_up_date.tv_sec * 1000000LL) +
                                               (DevULong64) ite_prev->wake_up_date.tv_usec;
                    if (next_work > prev_obj_work) {
                        DevULong64 n =
                                (next_work - prev_obj_work) / ((DevULong64) ite_prev->update * 1000LL);
                        next_prev = prev_obj_work + (n * (ite_prev->update * 1000LL));
                    } else
                        next_prev = prev_obj_work;

                    wo.wake_up_date.tv_sec = (long) (next_prev / 1000000LL);
                    wo.wake_up_date.tv_usec = (long) (next_prev % 1000000LL);

                    T_ADD(wo.wake_up_date, needed_time_usec + max_delta_needed);
                }
                new_works.push(wo);
            }

//
// Replace work list
//

            works.swap(new_works);
        } else {
            ite_next = works.begin();
            ite = ite_next;
            ++ite_next;

            for (unsigned int i = 1; i < nb_works; i++) {
                long diff;
                T_DIFF(ite->wake_up_date, ite_next->wake_up_date, diff);

//
// If delta time between works is less than min, shift following work
//

                if (diff < min_delta)
                T_ADD(ite_next->wake_up_date, min_delta - diff);

                ++ite;
                ++ite_next;
            }
        }

        cout4 << "Tuning list done" << endl;
        print_work_items();
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

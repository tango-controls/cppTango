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
#include <polling/event_system.hxx>

#include "threading/repeated_task.hxx"
#include "threading/asymmetric_unbound_blocking_queue.hxx"
//import template definitions
#include "polling/polling_queue.hxx"
#include "polling/event_system.hxx"


namespace Tango {

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
            : polling_stop_(true),
              tune_ctr(1),
              need_two_tuning{false},
              name_(move(name)),
              previous_nb_late{0},
              polling_bef_9{polling_as_before_tango_9},
              thread_{},
              works{},
              ext_trig_works{},
              event_system_(Util::instance()->get_notifd_event_supplier(), Util::instance()->get_zmq_event_supplier()) {
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

    thread::id PollThread::id() {
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
    PollThread::find_work_item(DeviceImpl *device, PollObjType type, chrono::milliseconds update) {
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
        works.for_each([](const WorkItem &work_item) {
            ostringstream oss;
            copy(work_item.name.begin(), work_item.name.end() - 1,
                 ostream_iterator<int>(oss, ","));

            oss << work_item.name.back();

            cout5 << "Dev name = " << work_item.dev->get_name() << ", obj name(s) = " << oss.str()
                  << ", next wake_up at " << +work_item.wake_up_date.time_since_epoch() << endl;
        });
    }

    void PollThread::add_or_push(WorkItem &new_work) {
        experimental::optional<WorkItem> work_item = find_work_item(new_work.dev, new_work.type, new_work.update);

        if (work_item)
            work_item->name.push_back(new_work.name[0]);
        else
            works.push(new_work);
    }

    std::experimental::optional<WorkItem>
    PollThread::remove_work_item(DeviceImpl *device, std::string obj_name, PollObjType obj_type) {
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
            if (work_item->name.empty()) {
                auto outer = work_item.value();
                works.erase(
                        [&outer](const WorkItem &inner) { return inner.dev == outer.dev && inner.type == outer.type; });
            }
        }

        return work_item;
    }

    std::experimental::optional<WorkItem>
    PollThread::remove_trigger(DeviceImpl *device, std::string obj_name, PollObjType obj_type) {
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
            if (work_item->name.empty()) {
                auto outer = work_item.value();
                works.erase(
                        [&outer](const WorkItem &inner) { return inner.dev == outer.dev && inner.type == outer.type; });
            }
        }

        return work_item;
    }

    bool PollThread::analyze_work_list() {
        auto calculate_diff = [](const WorkItem &work_item) {
            return work_item.wake_up_date.time_since_epoch() - work_item.stop_time.time_since_epoch();
        };
        auto diff = calculate_diff(works.top());

        while ((diff < 0) && (abs(diff) > kDiscardThreshold)) {
            cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
            WorkItem tmp = works.top();
            if (tmp.type == POLL_ATTR) {
                DevErrorList errs{1};

                errs[0].severity = ERR;
                errs[0].reason = string_dup("API_PollThreadOutOfSync");
                errs[0].origin = string_dup("PollThread::push_error_event");
                errs[0].desc = string_dup(
                        "The polling thread is late and discard this object polling.\nAdvice: Tune device server polling");

                DevFailed except{errs};

                get_event_system().push_error_event(tmp, except);
            }

            compute_new_date(tmp.wake_up_date, tmp.update);
            works.push(tmp);
            works.pop();
            tune_ctr--;

            diff = calculate_diff(works.top());
        }

        return (abs(diff) < kDiscardThreshold);
    }

    bool PollThread::discard_late_items() {
        if (polling_bef_9) {
            previous_nb_late = 0;

            return analyze_work_list();
        } else {
            auto wake_up_before_stop = [](const WorkItem &work_item) {
                int64_t diff = work_item.wake_up_date.time_since_epoch() - work_item.stop_time.time_since_epoch();
                return (diff < 0 && abs(diff) > kDiscardThreshold);
            };

            //
            // Compute for how many items the polling thread is late
            //
            auto nb_late = works.count_if(wake_up_before_stop);
            // If we are late for some item(s):
            if (nb_late > 0) {

                //  - Late for the first time: Poll immediately but memorize the number of items for which we are late
                if (previous_nb_late == 0) {
                    previous_nb_late = nb_late;
                    return true;//TODO should analyze list happen in this case?
                } else {
                    //  - Late again: If the number of late items increase --> Discard items
                    if (nb_late < previous_nb_late) {
                        previous_nb_late = nb_late;
                        cout5 << "Late but trying to catch up" << endl;
                        return false;
                    } else {
                        previous_nb_late = 0;
                    }
                }

                return analyze_work_list();
            }

            return false;
        }
    }

    chrono::time_point PollThread::compute_new_date(chrono::time_point current_time, chrono::milliseconds update) {
        return current_time.time_since_epoch() + update;
    }

    void PollThread::set_time(chrono::time_point start, chrono::time_point stop) {
        start_ = start;
        stop_ = stop;
    }

    polling::EventSystem &PollThread::get_event_system() {
        return event_system_;
    }

    std::chrono::milliseconds PollThread::compute_next_sleep(bool discarded) {
        if (discarded)
            return {0};
        else {
            auto item = works.top();
            auto diff = item.wake_up_date.time_since_epoch() - item.stop_time.time_since_epoch();
            return {diff * 1000};//TODO return previous value?; *1000?
        }
    }


    void PollThread::adjust_work_items(WorkItem &just_polled_item) {
        //
        // For case where the polling thread itself modify the polling period of the object it already polls
        //

        if (auto_upd.empty() == false) {
            for (size_t i = 0; i < auto_upd.size(); i++) {
                just_polled_item.name.erase(
                        remove(
                                just_polled_item.name.begin(),
                                just_polled_item.name.end(), auto_name[i]),
                        just_polled_item.name.end());
            }

            if (just_polled_item.name.empty() == false) {
                just_polled_item.wake_up_date = compute_new_date(just_polled_item.wake_up_date,
                                                                 just_polled_item.update);
                works.push(just_polled_item);
            }

            list<WorkItem>::iterator ite;
            vector<WorkItem>::iterator et_ite;

            for (size_t i = 0; i < auto_upd.size(); i++) {
                int auto_upd_value = auto_upd[i];
                auto work_item = works.find_if(
                        [auto_upd_value, &work_item](const WorkItem &work_item_in_c) {
                            return work_item_in_c.dev == work_item.dev &&
                                   work_item_in_c.type == work_item.type &&
                                   work_item_in_c.update == auto_upd_value;
                        });

                if (work_item)
                    work_item->name.push_back(auto_name[i]);
                else {
                    WorkItem new_work_item = new_work_item(work_item.dev, poll_obj);
                    new_work_item.update = auto_upd[i];
                    new_work_item.name.push_back(auto_name[i]);

                    new_work_item.wake_up_date = compute_new_date(now,
                                                                  new_work_item.update);//TODO local_cmd.new_upd??? -- last non-timeout command
                    works.push(new_work_item);
                }
            }

            auto_upd.clear();
            auto_name.clear();
        }

//
// Compute new polling date and insert work in list
//

        else {
            if (rem_upd.empty() == false) {
                for (size_t loop = 0; loop < rem_upd.size(); loop++) {
                    vector<string>::iterator pos = remove(just_polled_item.name.begin(), just_polled_item.name.end(),
                                                          rem_name[loop]);
                    just_polled_item.name.erase(pos, just_polled_item.name.end());
                }

                if (just_polled_item.name.empty() == false) {
                    just_polled_item.wake_up_date = compute_new_date(just_polled_item.wake_up_date,
                                                                     just_polled_item.update);
                    works.push(just_polled_item);
                }

                rem_upd.clear();
                rem_name.clear();
            } else {
                just_polled_item.wake_up_date = compute_new_date(just_polled_item.wake_up_date,
                                                                 just_polled_item.update);
                works.push(just_polled_item);
            }
        }

        tune_ctr--;

        if (tune_ctr <= 0) {
            tune_list();
            if (need_two_tuning) {
                set_need_two_tuning(false);
            } else
                tune_ctr = kPollLoop;
        }
    }

    void PollThread::tune_list() {
        size_t nb_works = works.size();

//
// Nothing to do if only one let in list
//

        if (nb_works < 2)
            return;


        cout4 << "Entering tuning list. The list has " << nb_works << " item(s)" << endl;
//
// If we try to tune the list with respect to works needed time, compute works needed time sum and find minimun update
// period
//

        chrono::nanoseconds needed_sum{0};
        chrono::milliseconds min_upd = works.top().update;
        chrono::nanoseconds max_delta_needed = works.top().needed_time;

        //transform -> collect;
        //transofmr -> min;
        works.for_each([&](const WorkItem &work_item) {
            needed_sum += work_item.needed_time;

            if (min_upd > work_item.update)
                min_upd = work_item.update;
        });

//
// In some cases, it is impossible to tune
//

        if (needed_sum > min_upd)//TODO should not it be LT
            return;
        else {
            chrono::nanoseconds sleeping = min_upd - needed_sum;
            max_delta_needed = sleeping / (nb_works);
        }

//
// Now build a new tuned list

// To tune the list
// - Take obj j and compute when it should be polled (next_work)
// - Compute when object j-1 should be polled (prev_obj_work)
// - Compute the number of poll between these two dates (n)
// - Compute date of previous object polling just before "next_work"
// - Assign next_work to this date and add
//       the time needed to execute previous object polling
//		 the delta computed from the smallest upd and the obj number
//

        auto now = chrono::high_resolution_clock::now();//TODO now? or last polled item.stop
        auto next_tuning = now + (kPollLoop * min_upd);

        polling::PollingQueue new_works{};
        new_works.push(works.top());

        auto ite = works.top();
        auto previous_work_item = new_works.top();

        works.for_each([&](WorkItem& wo){
            auto previous_needed_time = previous_work_item.needed_time;
            auto wake_up_date = wo.wake_up_date;

            chrono::time_point next_prev;
            if (wake_up_date.time_since_epoch() < next_tuning) {
                auto previous_wake_up = previous_work_item.wake_up_date;
                if (wake_up_date.time_since_epoch() > previous_wake_up) {
                    auto n =
                            (wake_up_date.time_since_epoch() - previous_wake_up) / (previous_work_item.update);//*1000LL
                    next_prev = previous_wake_up + (n * (previous_work_item.update));
                } else
                    next_prev = previous_wake_up;

                wo.wake_up_date = next_prev;

                wo.wake_up_date = {previous_needed_time + max_delta_needed};
            }
            new_works.push(wo);
            previous_work_item = wo;//TODO pointer
        });

//
// Replace work list
//

        works.swap(new_works);

        cout4 << "Tuning list done" << endl;
        print_work_items();
    }
} // End of Tango namespace

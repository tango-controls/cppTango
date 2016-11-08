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
#include "threading/enhanced_thread.hxx"
//import template definitions
#include "polling/polling_queue.hxx"
#include "polling/polling_thread.hxx"
#include "polling/event_system.hxx"


namespace Tango {

    extern thread_local std::shared_ptr<PyData> kPerThreadPyData;

    const int PollThread::kPollLoop{500};
    constexpr std::chrono::milliseconds PollThread::kDiscardThreshold;

    std::experimental::optional<WorkItem>
    find_work_item(polling::PollingQueue &works, DeviceImpl *device, std::string obj_name, PollObjType obj_type) {
        auto same_device_type_contains_name = [device, obj_name, obj_type](const WorkItem &work_item) {
            return work_item.dev == device && work_item.type == obj_type &&
                   find_if(work_item.name.begin(), work_item.name.end(),
                           [obj_name](const string &name) { return name == obj_name; }) != work_item.name.end();
        };
        return works.find_if(same_device_type_contains_name);
    }

    PollThreadPtr PollThread::create_instance_ptr(std::string &&name, bool polling_as_before_tango_9) {
        return Tango::PollThreadPtr(
                new PollThread(move(name), polling_as_before_tango_9));
    }

    PollThread::PollThread(string &&name, bool polling_as_before_tango_9)
            : polling_stop_(true),
              tune_counter_(1),
              need_two_tuning{false},
              name_(move(name)),
              previous_nb_late{0},
              polling_bef_9{polling_as_before_tango_9},
              thread_(new polling::PollingThread(*this)),
              works(new polling::PollingQueue{}),
              ext_trig_works(new polling::PollingQueue{}),
              event_system_(new polling::EventSystem(Util::instance()->get_notifd_event_supplier(),
                                                     Util::instance()->get_zmq_event_supplier())) {
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
        return thread_->id();
    }

    using chrono::system_clock;
    using chrono::milliseconds;

    WorkItem PollThread::new_work_item(DeviceImpl *device, /*TODO const*/ PollObj &poll_obj) {
        //TODO forward arguments
        auto now = chrono::duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        WorkItem result{
                device,
                &(device->get_poll_obj_list()),
                now,
                milliseconds{poll_obj.get_upd()},
                poll_obj.get_type(),
                {poll_obj.get_name()},
                nullptr,
                now,
                chrono::nanoseconds{0},
                now
        };

        return result;
    }

    void PollThread::set_need_two_tuning(bool v) {
        size_t nb_works = works->size();
        tune_counter_ = (nb_works << 2); //* 4
        need_two_tuning = v;
    }


    void PollThread::execute_cmd(polling::Command &&cmd) {
        cmd.execute(*this);


        if (Tango::Util::_tracelevel >= 4)
            print_work_items();

        return;
    }

    std::experimental::optional<WorkItem>
    PollThread::find_work_item(DeviceImpl *device, PollObjType type, milliseconds update) {
        experimental::optional<WorkItem> optional_work_item{};
        if (polling_bef_9) return optional_work_item;

        bool is_attr = type == POLL_ATTR;
        long idl_version = device->get_dev_idl_version();
        if (is_attr && idl_version >= 4) {
            return works->find_if([device, update, type](const WorkItem &work_item) {
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
        works->for_each([](const WorkItem &work_item) {
            ostringstream oss;
            copy(work_item.name.begin(), work_item.name.end() - 1,
                 ostream_iterator<string>(oss, ","));

            oss << work_item.name.back();

            cout5 << "Dev name = " << work_item.dev->get_name() << ", obj name(s) = " << oss.str()
                  << ", next wake_up at " << +work_item.wake_up_date.count() << endl;
        });
    }

    void PollThread::add_work_item(WorkItem &new_work) {
        experimental::optional<WorkItem> work_item = find_work_item(new_work.dev, new_work.type, new_work.update);

        if (work_item)
            work_item->name.push_back(new_work.name[0]);
        else
            works->push(new_work);
    }

    std::experimental::optional<WorkItem>
    PollThread::remove_work_item_by(DeviceImpl *device, std::string obj_name, PollObjType obj_type) {
        auto work_item = Tango::find_work_item(*works, device, obj_name, obj_type);

        if (work_item) {
            work_item->name.erase(remove_if(
                    work_item->name.begin(),
                    work_item->name.end(),
                    [obj_name](const string &name) { return name == obj_name; }
            ), work_item->name.end());
            if (work_item->name.empty()) {
                auto outer = work_item.value();
                works->erase(
                        [&outer](const WorkItem &inner) { return inner.dev == outer.dev && inner.type == outer.type; });
            }
        }

        return work_item;
    }

    std::experimental::optional<WorkItem>
    PollThread::remove_trigger_by(DeviceImpl *device, std::string obj_name, PollObjType obj_type) {
        auto work_item = Tango::find_work_item(*ext_trig_works, device, obj_name, obj_type);

        if (work_item) {
            work_item->name.erase(remove_if(
                    work_item->name.begin(),
                    work_item->name.end(),
                    [obj_name](const string &name) { return name == obj_name; }
            ), work_item->name.end());
            if (work_item->name.empty()) {
                auto outer = work_item.value();
                ext_trig_works->erase(
                        [&outer](const WorkItem &inner) { return inner.dev == outer.dev && inner.type == outer.type; });
            }
        }

        return work_item;
    }

    //TODO return discarded items list
    bool PollThread::analyze_work_list() {
        auto now = chrono::high_resolution_clock::now().time_since_epoch();
        auto calculate_diff = [&now](const WorkItem &work_item) {
            return work_item.wake_up_date - now;
        };
        auto diff = calculate_diff(works->top());

        while ((diff.count() < 0) && (abs(diff.count()) > kDiscardThreshold.count())) {
            cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
            WorkItem tmp = works->pop();
            if (tmp.type == POLL_ATTR) {
                //TODO put into dedicated list and send events in a dedicated method
                try {
                    DevErrorList errs{};
                    errs.length(1);

                    errs[0].severity = ERR;
                    errs[0].reason = string_dup("API_PollThreadOutOfSync");
                    errs[0].origin = string_dup("PollThread::push_error_event");
                    errs[0].desc = string_dup(
                            "The polling thread is late and discard this object polling.\nAdvice: Tune device server polling");

                    DevFailed except{errs};


                    get_event_system().push_error_event(tmp,
                                                        except);
                } catch (const CORBA::BAD_PARAM &ex) {
                    //TODO throws SIGABRT: terminate called after throwing an instance of 'CORBA::BAD_PARAM'
                    cerr << ex._name() << endl;
                }
            }

            tmp.wake_up_date += tmp.update;
            works->push(tmp);
            tune_counter_--;

            diff = calculate_diff(works->top());
        }

        return (abs(diff.count()) < kDiscardThreshold.count());
    }

    bool PollThread::discard_late_items() {
        //TODO remove this check (preserve only false)
        if (polling_bef_9) {
            previous_nb_late = 0;

            return analyze_work_list();
        } else {
            auto wake_up_before_stop = [](const WorkItem &work_item) {
                auto diff = work_item.wake_up_date - work_item.stop_time;
                return (diff.count() < 0 && abs(diff.count()) > kDiscardThreshold.count());
            };

            //
            // Compute for how many items the polling thread is late
            //
            auto nb_late = works->count_if(wake_up_before_stop);
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

//    chrono::time_point PollThread::compute_new_date(chrono::time_point current_time, chrono::milliseconds update) {
//        return current_time.time_since_epoch() + update;
//    }

    /*TODO const*/ polling::EventSystem &PollThread::get_event_system() {
        return *event_system_;
    }

    std::chrono::milliseconds PollThread::compute_next_sleep(bool discarded) {
        if (discarded)
            return milliseconds{0};
        else {
            auto item = works->top();
            auto now = chrono::high_resolution_clock::now().time_since_epoch();
            auto diff = item.wake_up_date - now;
            return chrono::duration_cast<milliseconds>(diff);
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
                just_polled_item.wake_up_date += just_polled_item.update;
                works->push(just_polled_item);
            }

            for (size_t i = 0; i < auto_upd.size(); i++) {
                milliseconds auto_upd_value = auto_upd[i];
                auto work_item = works->find_if(
                        [auto_upd_value, &just_polled_item](const WorkItem &work_item_in_c) {
                            return work_item_in_c.dev == just_polled_item.dev &&
                                   work_item_in_c.type == just_polled_item.type &&
                                   work_item_in_c.update == just_polled_item.update;
                        });

                if (work_item)
                    work_item->name.push_back(auto_name[i]);
                else {
                    WorkItem new_work_item(just_polled_item);

                    new_work_item.update = auto_upd[i];
                    new_work_item.name.push_back(auto_name[i]);

                    new_work_item.wake_up_date += new_work_item.update;//TODO local_cmd.new_upd??? -- last non-timeout command
                    works->push(new_work_item);
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
                    just_polled_item.wake_up_date += just_polled_item.update;
                    works->push(just_polled_item);
                }

                rem_upd.clear();
                rem_name.clear();
            } else {
                just_polled_item.wake_up_date = just_polled_item.stop_time + just_polled_item.update;
                works->push(just_polled_item);
            }
        }

        cout4 << "Adjusting list has been done." << endl;
        print_work_items();
    }

    void PollThread::tune_work_items_list() {
        if (tune_counter_-- > 0) return;

        size_t nb_works = works->size();

//
// Nothing to do if only one let in list
//

        if (nb_works < 2) return;


        cout4 << "Entering tuning list. The list has " << nb_works << " item(s)" << endl;
//
// If we try to tune the list with respect to works needed time, compute works needed time sum and find minimun update
// period
//

        chrono::nanoseconds needed_sum{0};
        milliseconds min_upd = works->top().update;
        chrono::nanoseconds max_delta_needed = works->top().needed_time;

        //transform -> collect;
        //transofmr -> min;
        works->for_each([&](const WorkItem &work_item) {
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
        new_works.push(works->top());

        auto previous_work_item = new_works.top();

        works->for_each([&](const WorkItem &wo) {
            auto previous_needed_time = previous_work_item.needed_time;
            auto wake_up_date = chrono::duration_cast<chrono::nanoseconds>(wo.wake_up_date);


            if (wake_up_date < next_tuning.time_since_epoch()) {
                auto previous_wake_up = previous_work_item.wake_up_date;
                milliseconds next_prev;
                if (wake_up_date > previous_wake_up) {
                    auto n =
                            (wake_up_date - previous_wake_up) / (previous_work_item.update);//*1000LL
                    next_prev = previous_wake_up + (n * (previous_work_item.update));
                } else
                    next_prev = previous_wake_up;

                next_prev += chrono::duration_cast<milliseconds>(previous_needed_time + max_delta_needed);
                const_cast<WorkItem &>(wo).wake_up_date = next_prev;
            }
            new_works.push(wo);
            previous_work_item = wo;//TODO pointer
        });

//
// Replace work list
//

        works->swap(new_works);

        cout4 << "Tuning list has been done." << endl;
        print_work_items();

        //reset
        if (need_two_tuning)
            set_need_two_tuning(false);
        else
            tune_counter_ = kPollLoop;
    }

    void PollThread::remove_work_items_by(DeviceImpl *device) {
        auto predicate = [device](const WorkItem &work_item) { return work_item.dev == device; };

        works->erase(predicate);

        ext_trig_works->erase(predicate);
    }

    std::experimental::optional<WorkItem>
    PollThread::find_trigger(DeviceImpl *device, PollObjType type, std::string name) {
        auto predicate = [device, type, name](const WorkItem &work_item) {
            return work_item.dev == device && work_item.type == type && work_item.name[0] == name;
        };

        return ext_trig_works->find_if(predicate);
    }

    void PollThread::add_trigger(WorkItem &trigger) {
        ext_trig_works->push(trigger);
    }

    bool PollThread::empty() {
        return works->empty();
    }

    void PollThread::start_thread_if_required() {
        //TODO protected id
        if (thread_->id() != thread::id()) return;

        //TODO get_thread_pool.submit()
        thread_.reset(new polling::PollingThread(*this));
        thread_->start();
    }
} // End of Tango namespace

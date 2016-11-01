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
#include <tango/server/pollthread.tpp>

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
#include "threading/asymmetric_unbound_blocking_queue.cxx"


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
            : sleep(1),
              polling_stop(true),
              tune_ctr(1),
              need_two_tuning(false),
              name_(move(name)),
              polling_bef_9{polling_as_before_tango_9},
              queue_{new threading::asymmetric_unbound_blocking_queue<PollThCmd>()} {
        local_cmd.cmd_pending = false;

        dummy_cl_id.cpp_clnt(0);
        previous_nb_late = 0;
        polling_bef_9 = false;

#ifdef _TG_WINDOWS_
        LARGE_INTEGER f;
        BOOL is_ctr;
        is_ctr = QueryPerformanceFrequency(&f);
        if (is_ctr != 0)
            ctr_frequency = 1000000.0 / (double)f.QuadPart;
        else
            ctr_frequency = 0.0;
#endif

        dummy_att5.value.union_no_data(true);
        dummy_att5.quality = ATTR_INVALID;
        dummy_att4.value.union_no_data(true);
        dummy_att4.quality = ATTR_INVALID;
        dummy_att3.quality = ATTR_INVALID;
    }

    WorkItem PollThread::new_work_item(DeviceImpl* device, /*TODO const*/ PollObj& poll_obj) {
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


    void PollThread::execute_cmd(polling::Command &cmd) {
        cmd.execute(*this);


        if (Tango::Util::_tracelevel >= 4)
            print_list();

        return;
    }

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::one_more_poll
//
// description :
//
//-------------------------------------------------------------------------------------------------------------------

    void PollThread::one_more_poll() {
        if(polling_stop) return;

        WorkItem tmp = works.front();
        works.pop_front();

        switch (tmp.type) {
            case Tango::POLL_CMD:
                poll_cmd(tmp);
                break;

            case Tango::POLL_ATTR:
                poll_attr(tmp);
                break;
        }

//
// For case where the polling thread itself modify the polling period of the object it already polls
//

        if (auto_upd.empty() == false) {
            for (size_t loop = 0; loop < auto_upd.size(); loop++) {
                vector<string>::iterator pos = remove(tmp.name.begin(), tmp.name.end(), auto_name[loop]);
                tmp.name.erase(pos, tmp.name.end());
            }

            if (tmp.name.empty() == false) {
                compute_new_date(tmp.wake_up_date, tmp.update);
                insert_in_list(tmp);
            }

            list<WorkItem>::iterator ite;
            vector<WorkItem>::iterator et_ite;

            for (size_t loop = 0; loop < auto_upd.size(); loop++) {
                size_t nb_elt = works.size();
                ite = works.begin();

                bool found = false;

                for (size_t i = 0; i < nb_elt; i++) {
                    if (ite->dev == tmp.dev &&
                        ite->type == tmp.type &&
                        ite->update == auto_upd[loop]) {
                        ite->name.push_back(auto_name[loop]);
                        found = true;
                        break;
                    }
                    ++ite;
                }

                if (found == false) {
                    WorkItem new_tmp;
                    new_tmp.update = auto_upd[loop];
                    new_tmp.name.push_back(auto_name[loop]);
                    new_tmp.dev = tmp.dev;
                    new_tmp.poll_list = tmp.poll_list;
                    new_tmp.type = tmp.type;
                    new_tmp.needed_time.tv_sec = 0;
                    new_tmp.needed_time.tv_usec = 0;
                    compute_new_date(now, local_cmd.new_upd);
                    new_tmp.wake_up_date = now;
                    insert_in_list(new_tmp);
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
                    vector<string>::iterator pos = remove(tmp.name.begin(), tmp.name.end(), rem_name[loop]);
                    tmp.name.erase(pos, tmp.name.end());
                }

                if (tmp.name.empty() == false) {
                    compute_new_date(tmp.wake_up_date, tmp.update);
                    insert_in_list(tmp);
                }

                rem_upd.clear();
                rem_name.clear();
            } else {
                compute_new_date(tmp.wake_up_date, tmp.update);
                insert_in_list(tmp);
            }
        }

        tune_ctr--;
    }

//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::one_more_trigg
//
// description :
//		This method is called when a trigger command has been received
//
//----------------------------------------------------------------------------------------------------------------


    void PollThread::one_more_trigg() {
        cout5 << "Polling thread has received a trigger" << endl;

//
// Check that the object is registered
//

        //TODO extract find function
        vector<WorkItem>::iterator et_ite;
        for (et_ite = ext_trig_works.begin(); et_ite != ext_trig_works.end(); ++et_ite) {
            if (et_ite->dev == local_cmd.dev) {
                if (et_ite->type == local_cmd.type) {
                    if (et_ite->name[0] == local_cmd.name)
                        break;
                }
            }
        }

//
// Check that the object to poll has been installed. If not, simply returns. This case should never happens because
// it is tested in the Util::trigger_polling() method before the trigger is effectively sent to this thread.
//

        if (et_ite == ext_trig_works.end()) {
            cout5 << "Object externally triggered not found !!!" << endl;
            return;
        }

//
// Do the job
//

        WorkItem tmp = *et_ite;
        if (polling_stop == false) {
            if (tmp.type == Tango::POLL_CMD)
                poll_cmd(tmp);
            else
                poll_attr(tmp);
        }

//
// TODO Inform requesting thread that the work is done
//

    }


//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::print_list
//
// description :
//		To print work list
//
//-----------------------------------------------------------------------------------------------------------------

    void PollThread::print_list() {
        list<WorkItem>::iterator ite;
        long nb_elt, i;

        nb_elt = works.size();
        ite = works.begin();
        for (i = 0; i < nb_elt; i++) {
            string obj_list;
            for (size_t ctr = 0; ctr < ite->name.size(); ctr++) {
                obj_list = obj_list + ite->name[ctr];
                if (ctr < (ite->name.size() - 1))
                    obj_list = obj_list + ", ";
            }

            cout5 << "Dev name = " << ite->dev->get_name() << ", obj name = " << obj_list
                  << ", next wake_up at " << +ite->wake_up_date.tv_sec
                  << "," << setw(6) << setfill('0')
                  << ite->wake_up_date.tv_usec << endl;

            ++ite;
        }
    }


//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::insert_in_list
//
// description :
//		To insert (at the correct place) a new Work Item in the work list
//
// args :
//		in :
// 			- new_work : The new work item
//
//-----------------------------------------------------------------------------------------------------------------

    void PollThread::insert_in_list(WorkItem &new_work) {
        list<WorkItem>::iterator ite;
        for (ite = works.begin(); ite != works.end(); ++ite) {
            if (ite->wake_up_date.tv_sec < new_work.wake_up_date.tv_sec)
                continue;
            else if (ite->wake_up_date.tv_sec == new_work.wake_up_date.tv_sec) {
                if (ite->wake_up_date.tv_usec < new_work.wake_up_date.tv_usec)
                    continue;
                else {
                    works.insert(ite, new_work);
                    return;
                }
            } else {
                works.insert(ite, new_work);
                return;
            }
        }
        if (ite == works.end())
            works.push_back(new_work);
    }

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::add_insert_in_list
//
// description :
//		To insert (at the correct place) a new Work Item in the work list but before this for attribute, if we
//      already have a work item with the same device at the same period, simply add the obj name in it.
//      (in order to use read_attributes() in a optimal way)
//
// args :
//		in :
// 			- new_work : The new work item
//
//-----------------------------------------------------------------------------------------------------------------

    void PollThread::add_insert_in_list(WorkItem &new_work) {
        if (new_work.type == POLL_ATTR && new_work.dev->get_dev_idl_version() >= 4 && polling_bef_9 == false) {
            list<WorkItem>::iterator ite;
#ifdef HAS_LAMBDA_FUNC
            ite = find_if(works.begin(), works.end(),
                          [&](const WorkItem &wi) {
                              return wi.dev == new_work.dev && wi.update == new_work.update && wi.type == new_work.type;
                          });
#else
            for (ite = works.begin();ite != works.end();++ite)
            {
                if (ite->dev == new_work.dev && ite->update == new_work.update && ite->type == new_work.type)
                    break;
            }
#endif

            if (ite != works.end()) {
                ite->name.push_back(new_work.name[0]);
            } else
                insert_in_list(new_work);
        } else
            insert_in_list(new_work);
    }

//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::tune_list
//
// description :
//		This method tunes the work list.
//
// args :
//		in :
// 			- from_needed : Set to true if the delta between work should be at least equal to the
//							time needed to execute the previous work
//			- min_delta : Min. delta between polling works when from_needed is false
//
//----------------------------------------------------------------------------------------------------------------

    void PollThread::tune_list(bool from_needed, long min_delta) {
        list<WorkItem>::iterator ite, ite_next, ite_prev;

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

            Tango::DevULong64 now_us = ((Tango::DevULong64) now.tv_sec * 1000000LL) + (Tango::DevULong64) now.tv_usec;
            Tango::DevULong64 next_tuning = now_us + (POLL_LOOP_NB * (Tango::DevULong64) min_upd);

            list<WorkItem> new_works;
            new_works.push_front(works.front());

            ite = works.begin();
            ite_prev = new_works.begin();

            for (++ite; ite != works.end(); ++ite, ++ite_prev) {
                Tango::DevULong64 needed_time_usec = ((Tango::DevULong64) ite_prev->needed_time.tv_sec * 1000000) +
                                                     (Tango::DevULong64) ite_prev->needed_time.tv_usec;
                WorkItem wo = *ite;
                Tango::DevULong64 next_work = ((Tango::DevULong64) wo.wake_up_date.tv_sec * 1000000LL) +
                                              (Tango::DevULong64) wo.wake_up_date.tv_usec;

                Tango::DevULong64 next_prev;
                if (next_work < next_tuning) {
                    Tango::DevULong64 prev_obj_work = ((Tango::DevULong64) ite_prev->wake_up_date.tv_sec * 1000000LL) +
                                                      (Tango::DevULong64) ite_prev->wake_up_date.tv_usec;
                    if (next_work > prev_obj_work) {
                        Tango::DevULong64 n =
                                (next_work - prev_obj_work) / ((Tango::DevULong64) ite_prev->update * 1000LL);
                        next_prev = prev_obj_work + (n * (ite_prev->update * 1000LL));
                    } else
                        next_prev = prev_obj_work;

                    wo.wake_up_date.tv_sec = (long) (next_prev / 1000000LL);
                    wo.wake_up_date.tv_usec = (long) (next_prev % 1000000LL);

                    T_ADD(wo.wake_up_date, needed_time_usec + max_delta_needed);
                }
                new_works.push_back(wo);
            }

//
// Replace work list
//

            works = new_works;
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
        print_list();
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

    void PollThread::compute_new_date(struct timeval &time, int upd) {
        double ori_d = (double) time.tv_sec + ((double) time.tv_usec / 1000000);
        double new_d = ori_d + ((double) (upd) / 1000);
        time.tv_sec = (long) new_d;
        time.tv_usec = (long) ((new_d - time.tv_sec) * 1000000);
    }




//+----------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::compute_sleep_time
//
// description :
//		This method computes how many mS the thread should sleep before the next poll time. If this time is
//		negative and greater than a pre-defined threshold, the polling is discarded.
//
//----------------------------------------------------------------------------------------------------------------

    void PollThread::compute_sleep_time() {
        print_list();

        if (works.empty() == false) {
            //TODO use integral type to calculate time related values
            double next{};
            double diff{};
            double after_d = (double) after.tv_sec + ((double) after.tv_usec / 1000000);

            bool discard = false;
            u_int nb_late = 0;

            if (polling_bef_9 == false) {

//
// Compute for how many items the polling thread is late
//

                list<WorkItem>::iterator ite;

                for (ite = works.begin(); ite != works.end(); ++ite) {
                    next = (double) ite->wake_up_date.tv_sec + ((double) ite->wake_up_date.tv_usec / 1000000);
                    diff = next - after_d;
                    if (diff < 0 && fabs(diff) > DISCARD_THRESHOLD)
                        nb_late++;
                }

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
            } else
                discard = true;

//
// Analyse work list
//

//        cout5 << "discard = " << boolalpha << discard << endl;
            if (nb_late == 0 || discard == true) {
                previous_nb_late = 0;

                next = (double) works.front().wake_up_date.tv_sec +
                       ((double) works.front().wake_up_date.tv_usec / 1000000);
                diff = next - after_d;

                if (diff < 0) {
                    if (fabs(diff) < DISCARD_THRESHOLD)
                        sleep = -1;
                    else {
                        while ((diff < 0) && (fabs(diff) > DISCARD_THRESHOLD)) {
                            cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
                            WorkItem tmp = works.front();
                            if (tmp.type == POLL_ATTR)
                                err_out_of_sync(tmp);

                            compute_new_date(tmp.wake_up_date, tmp.update);
                            insert_in_list(tmp);
                            works.pop_front();
                            tune_ctr--;

                            next = (double) works.front().wake_up_date.tv_sec +
                                   ((double) works.front().wake_up_date.tv_usec / 1000000);
                            diff = next - after_d;
                        }

                        if (fabs(diff) < DISCARD_THRESHOLD)
                            sleep = -1;
                        else
                            sleep = (long) (diff * 1000);
                    }
                } else
                    sleep = (long) (diff * 1000);
            }

            cout5 << "Sleep for : " << sleep << endl;
        }
    }


//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::err_out_of_sync
//
// description :
//		To force one event if the polling thread has discarded one work item because it is late
//
// args :
//		in :
// 			- to_do : The work item
//
//----------------------------------------------------------------------------------------------------------------

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
                Tango::DevErrorList errs;
                errs.length(1);

                errs[0].severity = Tango::ERR;
                errs[0].reason = Tango::string_dup("API_PollThreadOutOfSync");
                errs[0].origin = Tango::string_dup("PollThread::err_out_of_sync");
                errs[0].desc = Tango::string_dup(
                        "The polling thread is late and discard this object polling.\nAdvice: Tune device server polling");

                Tango::DevFailed except(errs);
                long idl_vers = to_do.dev->get_dev_idl_version();

                struct EventSupplier::SuppliedEventData ad;
                ::memset(&ad, 0, sizeof(ad));

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





//+---------------------------------------------------------------------------------------------------------------
//
// method :
//		PollThread::poll_attr
//
// description :
//		Read attribute and store the result in the device ring buffer
//
// args :
//		in :
// 			- to_do : The work item
//
//----------------------------------------------------------------------------------------------------------------

    void PollThread::poll_attr(WorkItem &to_do) {
        size_t nb_obj = to_do.name.size();
        string att_list;
        for (size_t ctr = 0; ctr < nb_obj; ctr++) {
            att_list = att_list + to_do.name[ctr];
            if (ctr < (nb_obj - 1))
                att_list = att_list + ", ";
        }

        cout5 << "----------> Time = " << now.tv_sec << ","
              << setw(6) << setfill('0') << now.tv_usec
              << " Dev name = " << to_do.dev->get_name()
              << ", Attr name = " << att_list << endl;

        struct timeval before_cmd, after_cmd, needed_time;
#ifdef _TG_WINDOWS_
        struct _timeb before_win,after_win;
        LARGE_INTEGER before,after;
#endif
        Tango::AttributeValueList *argout = NULL;
        Tango::AttributeValueList_3 *argout_3 = NULL;
        Tango::AttributeValueList_4 *argout_4 = NULL;
        Tango::AttributeValueList_5 *argout_5 = NULL;
        Tango::DevFailed *save_except = NULL;
        bool attr_failed = false;
        vector<PollObj *>::iterator ite;
        map<size_t, Tango::DevFailed *> map_except;

        long idl_vers = to_do.dev->get_dev_idl_version();
        try {
#ifdef _TG_WINDOWS_
            if (ctr_frequency != 0)
                QueryPerformanceCounter(&before);
            _ftime(&before_win);
            before_cmd.tv_sec = (unsigned long)before_win.time;
            before_cmd.tv_usec = (long)before_win.millitm * 1000;
#else
            gettimeofday(&before_cmd, NULL);
#endif
            before_cmd.tv_sec = before_cmd.tv_sec - DELTA_T;

//
// Read the attributes
//

            attr_names.length(nb_obj);
            for (size_t ctr = 0; ctr < nb_obj; ctr++)
                attr_names[ctr] = to_do.name[ctr].c_str();

            if (idl_vers >= 5)
                argout_5 = (static_cast<Device_5Impl *>(to_do.dev))->read_attributes_5(attr_names, Tango::DEV,
                                                                                       dummy_cl_id);
            else if (idl_vers == 4)
                argout_4 = (static_cast<Device_4Impl *>(to_do.dev))->read_attributes_4(attr_names, Tango::DEV,
                                                                                       dummy_cl_id);
            else if (idl_vers == 3)
                argout_3 = (static_cast<Device_3Impl *>(to_do.dev))->read_attributes_3(attr_names, Tango::DEV);
            else
                argout = to_do.dev->read_attributes(attr_names);

#ifdef _TG_WINDOWS_
            if (ctr_frequency != 0)
            {
                QueryPerformanceCounter(&after);

                needed_time.tv_sec = 0;
                needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
                to_do.needed_time = needed_time;
            }
            else
            {
                _ftime(&after_win);
                after_cmd.tv_sec = (unsigned long)after_win.time;
                after_cmd.tv_usec = (long)after_win.millitm  * 1000;

                after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
                time_diff(before_cmd,after_cmd,needed_time);
                to_do.needed_time = needed_time;
            }
#else
            gettimeofday(&after_cmd, NULL);
            after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
            time_diff(before_cmd, after_cmd, needed_time);
            to_do.needed_time = needed_time;
#endif
        }
        catch (Tango::DevFailed &e) {
            attr_failed = true;
#ifdef _TG_WINDOWS_
            if (ctr_frequency != 0)
            {
                QueryPerformanceCounter(&after);

                needed_time.tv_sec = 0;
                needed_time.tv_usec = (long)((double)(after.QuadPart - before.QuadPart) * ctr_frequency);
                to_do.needed_time = needed_time;
            }
            else
            {
                _ftime(&after_win);
                after_cmd.tv_sec = (unsigned long)after_win.time;
                after_cmd.tv_usec = (long)after_win.millitm * 1000;

                after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
                time_diff(before_cmd,after_cmd,needed_time);
                to_do.needed_time = needed_time;
            }
#else
            gettimeofday(&after_cmd, NULL);
            after_cmd.tv_sec = after_cmd.tv_sec - DELTA_T;
            time_diff(before_cmd, after_cmd, needed_time);
            to_do.needed_time = needed_time;
#endif

            save_except = new Tango::DevFailed(e);
        }

//
// Starting with IDl release 3, an attribute in error is not an exception any more. Re-create one.
// Don't forget that it is still possible to receive classical exception (in case of Monitor timeout for instance)
//

        if (idl_vers >= 3) {
            if (idl_vers >= 5) {
                if (nb_obj == 1) {
                    if ((attr_failed == false) && ((*argout_5)[0].err_list.length() != 0)) {
                        attr_failed = true;
                        save_except = new Tango::DevFailed((*argout_5)[0].err_list);
                        delete argout_5;
                    }
                } else {
                    for (size_t ctr = 0; ctr < nb_obj; ctr++) {
                        if ((attr_failed == false) && ((*argout_5)[ctr].err_list.length() != 0)) {
                            Tango::DevFailed *tmp_except = new Tango::DevFailed((*argout_5)[ctr].err_list);
                            map_except.insert(pair<size_t, Tango::DevFailed *>(ctr, tmp_except));
                        }
                    }
                }
            } else if (idl_vers == 4) {
                if (nb_obj == 1) {
                    if ((attr_failed == false) && ((*argout_4)[0].err_list.length() != 0)) {
                        attr_failed = true;
                        save_except = new Tango::DevFailed((*argout_4)[0].err_list);
                        delete argout_4;
                    }
                } else {
                    for (size_t ctr = 0; ctr < nb_obj; ctr++) {
                        if ((attr_failed == false) && ((*argout_4)[ctr].err_list.length() != 0)) {
                            Tango::DevFailed *tmp_except = new Tango::DevFailed((*argout_4)[ctr].err_list);
                            map_except.insert(pair<size_t, Tango::DevFailed *>(ctr, tmp_except));
                        }
                    }
                }
            } else {
                if ((attr_failed == false) && ((*argout_3)[0].err_list.length() != 0)) {
                    attr_failed = true;
                    save_except = new Tango::DevFailed((*argout_3)[0].err_list);
                    delete argout_3;
                }
            }
        }

//
// Events - for each event call the detect_and_push() method this method will fire events if there are clients
// registered and if there is an event (on_change, on_alarm or periodic)
// We also have to retrieve which kind of clients made the subscription (zmq or notifd) and send the event accordingly
//

        EventSupplier *event_supplier_nd = NULL;
        EventSupplier *event_supplier_zmq = NULL;

        for (size_t ctr = 0; ctr < nb_obj; ctr++) {
            Attribute &att = to_do.dev->get_device_attr()->get_attr_by_name(to_do.name[ctr].c_str());

            if (att.use_notifd_event() == true && event_supplier_nd == NULL)
                event_supplier_nd = Util::instance()->get_notifd_event_supplier();
            if (att.use_zmq_event() == true && event_supplier_zmq == NULL)
                event_supplier_zmq = Util::instance()->get_zmq_event_supplier();

            if ((event_supplier_nd != NULL) || (event_supplier_zmq != NULL)) {
                if (attr_failed == true) {
                    struct EventSupplier::SuppliedEventData ad;
                    ::memset(&ad, 0, sizeof(ad));

                    if (idl_vers > 4)
                        ad.attr_val_5 = &dummy_att5;
                    else if (idl_vers == 4)
                        ad.attr_val_4 = &dummy_att4;
                    else if (idl_vers == 3)
                        ad.attr_val_3 = &dummy_att3;
                    else
                        ad.attr_val = &dummy_att;

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                    SendEventType send_event;
                    if (event_supplier_nd != NULL)
                        send_event = event_supplier_nd->detect_and_push_events(to_do.dev, ad, save_except,
                                                                               to_do.name[ctr], &before_cmd);
                    if (event_supplier_zmq != NULL) {
                        if (event_supplier_nd != NULL) {
                            vector<string> f_names;
                            vector<double> f_data;
                            vector<string> f_names_lg;
                            vector<long> f_data_lg;

                            if (send_event.change == true)
                                event_supplier_zmq->push_event_loop(to_do.dev, CHANGE_EVENT, f_names, f_data,
                                                                    f_names_lg, f_data_lg, ad, att, save_except);
                            if (send_event.archive == true)
                                event_supplier_zmq->push_event_loop(to_do.dev, ARCHIVE_EVENT, f_names, f_data,
                                                                    f_names_lg, f_data_lg, ad, att, save_except);
                            if (send_event.periodic == true)
                                event_supplier_zmq->push_event_loop(to_do.dev, PERIODIC_EVENT, f_names, f_data,
                                                                    f_names_lg, f_data_lg, ad, att, save_except);
                        } else
                            event_supplier_zmq->detect_and_push_events(to_do.dev, ad, save_except, to_do.name[ctr],
                                                                       &before_cmd);
                    }
                } else {
                    struct EventSupplier::SuppliedEventData ad;
                    ::memset(&ad, 0, sizeof(ad));

                    if (idl_vers > 4)
                        ad.attr_val_5 = &((*argout_5)[ctr]);
                    else if (idl_vers == 4)
                        ad.attr_val_4 = &((*argout_4)[ctr]);
                    else if (idl_vers == 3)
                        ad.attr_val_3 = &((*argout_3)[ctr]);
                    else
                        ad.attr_val = &((*argout)[ctr]);

//
// Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
// two times. The detect_and_push_events() method returns true if the event is detected.
//

                    SendEventType send_event;

                    map<size_t, Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                    Tango::DevFailed *tmp_except;
                    if (ite2 == map_except.end())
                        tmp_except = save_except;
                    else
                        tmp_except = ite2->second;

                    if (event_supplier_nd != NULL)
                        send_event = event_supplier_nd->detect_and_push_events(to_do.dev, ad, tmp_except,
                                                                               to_do.name[ctr], &before_cmd);
                    if (event_supplier_zmq != NULL) {
                        if (event_supplier_nd != NULL) {
                            vector<string> f_names;
                            vector<double> f_data;
                            vector<string> f_names_lg;
                            vector<long> f_data_lg;

                            if (send_event.change == true)
                                event_supplier_zmq->push_event_loop(to_do.dev, CHANGE_EVENT, f_names, f_data,
                                                                    f_names_lg, f_data_lg, ad, att, tmp_except);
                            if (send_event.periodic == true)
                                event_supplier_zmq->push_event_loop(to_do.dev, PERIODIC_EVENT, f_names, f_data,
                                                                    f_names_lg, f_data_lg, ad, att, tmp_except);
                            if (send_event.archive == true)
                                event_supplier_zmq->push_event_loop(to_do.dev, ARCHIVE_EVENT, f_names, f_data,
                                                                    f_names_lg, f_data_lg, ad, att, tmp_except);
                        } else
                            event_supplier_zmq->detect_and_push_events(to_do.dev, ad, tmp_except, to_do.name[ctr],
                                                                       &before_cmd);
                    }
                }
            }
        }


//
// Insert result in polling buffer and simply forget this attribute if it is not possible to insert the result in
// polling buffer
// Different case according to the number of attributes which has been read by this poll.
// The PollObj object are supposed to store data coming from a read_attributes() executed for 1 attribute.
// Sonce Tango 9.0.8, it is not always the case. This is why we differantiate cases when we read several att
// in once call. Sin the last case, re-create memory layout as the one coming from read_attributes called for
// single attribute.
// This new feature is available only for devices with IDL 4 or more
//

        try {
            to_do.dev->get_poll_monitor().get_monitor();
            for (size_t ctr = 0; ctr < nb_obj; ctr++) {
                ite = to_do.dev->get_polled_obj_by_type_name(to_do.type, to_do.name[ctr]);
                if (attr_failed == false) {
                    if (nb_obj == 1) {
                        if (idl_vers >= 5)
                            (*ite)->insert_data(argout_5, before_cmd, needed_time);
                        else if (idl_vers == 4)
                            (*ite)->insert_data(argout_4, before_cmd, needed_time);
                        else if (idl_vers == 3)
                            (*ite)->insert_data(argout_3, before_cmd, needed_time);
                        else
                            (*ite)->insert_data(argout, before_cmd, needed_time);
                    } else {
                        if (idl_vers >= 5) {
                            map<size_t, Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                            if (ite2 == map_except.end()) {
                                Tango::AttributeValueList_5 *new_argout_5 = new Tango::AttributeValueList_5(1);
                                new_argout_5->length(1);
                                (*new_argout_5)[0].value.union_no_data(true);
                                robb_data((*argout_5)[ctr], (*new_argout_5)[0]);
                                copy_remaining((*argout_5)[ctr], (*new_argout_5)[0]);
                                (*new_argout_5)[0].data_type = (*argout_5)[ctr].data_type;
                                (*ite)->insert_data(new_argout_5, before_cmd, needed_time);
                            } else
                                (*ite)->insert_except(ite2->second, before_cmd, needed_time);
                        } else {
                            map<size_t, Tango::DevFailed *>::iterator ite2 = map_except.find(ctr);
                            if (ite2 == map_except.end()) {
                                Tango::AttributeValueList_4 *new_argout_4 = new Tango::AttributeValueList_4(1);
                                new_argout_4->length(1);
                                (*new_argout_4)[0].value.union_no_data(true);
                                robb_data((*argout_4)[ctr], (*new_argout_4)[0]);
                                copy_remaining((*argout_4)[ctr], (*new_argout_4)[0]);
                                (*ite)->insert_data(new_argout_4, before_cmd, needed_time);
                            } else
                                (*ite)->insert_except(ite2->second, before_cmd, needed_time);
                        }
                    }
                } else {
                    if (nb_obj == 1)
                        (*ite)->insert_except(save_except, before_cmd, needed_time);
                    else {
                        if (ctr != nb_obj - 1) {
                            Tango::DevFailed *dup_except = new Tango::DevFailed(save_except->errors);
                            (*ite)->insert_except(dup_except, before_cmd, needed_time);
                        } else
                            (*ite)->insert_except(save_except, before_cmd, needed_time);
                    }
                }
            }

            if (nb_obj != 1 && attr_failed == false) {
                if (idl_vers >= 5)
                    delete argout_5;
                else
                    delete argout_4;
            }

            to_do.dev->get_poll_monitor().rel_monitor();
        }
        catch (Tango::DevFailed &) {
            if (attr_failed == false) {
                if (idl_vers >= 5)
                    delete argout_5;
                else if (idl_vers == 4)
                    delete argout_4;
                else if (idl_vers == 3)
                    delete argout_3;
                else
                    delete argout;
            } else
                delete save_except;
            to_do.dev->get_poll_monitor().rel_monitor();
        }

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

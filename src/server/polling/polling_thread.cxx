//
// Created by ingvord on 11/1/16.
//

#include "polling_thread.hxx"
#include "poll_task.hxx"

#include <tango.h>
#include <tango/server/eventsupplier.h>
#include "polling_queue.hxx"

using namespace std;

Tango::polling::PollingThread::PollingThread(Tango::PollThread &engine) :
        thread_{"PollingThread", &PollingThread::run, this},
        engine_(engine) {}

void Tango::polling::PollingThread::run() {
    //TODO memory fence or somethig to fetch latest values from engine
    while (!engine_.polling_stop) {
        bool interrupted = thread_.sleep_for(compute_next_sleep());
        if (interrupted) return;


#ifdef _TG_WINDOWS_
        _ftime(&now_win);
			now.tv_sec = (unsigned long)now_win.time;
			now.tv_usec = (long)now_win.millitm * 1000;
#else
        gettimeofday(&now,NULL);
#endif
        now.tv_sec = now.tv_sec - DELTA_T;


        WorkItem tmp = engine_.works.top();
        engine_.works.pop();

        PollTask task{tmp};
        task.execute();

//
// For case where the polling thread itself modify the polling period of the object it already polls
//

        if (engine_.auto_upd.empty() == false) {
            for (size_t loop = 0; loop < engine_.auto_upd.size(); loop++) {
                vector<string>::iterator pos = remove(tmp.name.begin(), tmp.name.end(), engine_.auto_name[loop]);
                tmp.name.erase(pos, tmp.name.end());
            }

            if (tmp.name.empty() == false) {
                tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                engine_.works.push(tmp);
            }

            list<WorkItem>::iterator ite;
            vector<WorkItem>::iterator et_ite;

            for (size_t loop = 0; loop < engine_.auto_upd.size(); loop++) {
                int auto_upd_value = engine_.auto_upd[loop];
                auto work_item = engine_.works.find_if(
                        [auto_upd_value](const WorkItem &work_item) {
                            return work_item.dev == tmp.dev &&
                                   work_item.type == tmp.type &&
                                   work_item.update == auto_upd_value;
                        });

                if (work_item)
                    work_item->name.push_back(engine_.auto_name[loop]);
                else {
                    WorkItem new_tmp = engine_.new_work_item(tmp.dev,);
                    new_tmp.update = engine_.auto_upd[loop];
                    new_tmp.name.push_back(engine_.auto_name[loop]);

                    engine_.now = engine_.compute_new_date(engine_.now, local_cmd.new_upd);
                    new_tmp.wake_up_date = engine_.now;
                    engine_.works.push(new_tmp);
                }
            }

            engine_.auto_upd.clear();
            engine_.auto_name.clear();
        }

//
// Compute new polling date and insert work in list
//

        else {
            if (engine_.rem_upd.empty() == false) {
                for (size_t loop = 0; loop < engine_.rem_upd.size(); loop++) {
                    vector<string>::iterator pos = remove(tmp.name.begin(), tmp.name.end(), engine_.rem_name[loop]);
                    tmp.name.erase(pos, tmp.name.end());
                }

                if (tmp.name.empty() == false) {
                    tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                    engine_.works.push(tmp);
                }

                engine_.rem_upd.clear();
                engine_.rem_name.clear();
            } else {
                tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                engine_.works.push(tmp);
            }
        }

        engine_.tune_ctr--;



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
            if (need_two_tuning == true)
            {
                unsigned long nb_works = works.size();
                tune_ctr = (nb_works << 2);
                need_two_tuning = false;
            }
            else
                tune_ctr = POLL_LOOP_NB;
        }
    }
}

chrono::milliseconds Tango::polling::PollingThread::compute_next_sleep() {
    if (engine_.works.empty() == false) {
        //TODO use integral type to calculate time related values
        uint64_t next{};
        uint64_t diff{};
        uint64_t after_d = engine_.after.tv_sec + (engine_.after.tv_usec / 1000000);

        uint64_t sleep{};

        bool discard = false;
        u_int nb_late = 0;

        if (engine_.polling_bef_9 == false) {

//
// Compute for how many items the polling thread is late
//
            engine_.works.for_each([](const WorkItem& work_item){
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
                if (nb_late == engine_.works.size()) {
                    cout5 << "Setting discard to true because nb_late == works.size() --> " << nb_late << endl;
                    discard = true;
                } else {
                    if (engine_.previous_nb_late != 0) {
                        if (nb_late < engine_.previous_nb_late) {
                            engine_.previous_nb_late = nb_late;
                            cout5 << "Late but trying to catch up" << endl;
                        } else {
                            engine_.previous_nb_late = 0;
                            discard = true;
                        }
                    } else
                        engine_.previous_nb_late = nb_late;
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
            engine_.previous_nb_late = 0;

            auto item = engine_.works.top();
            next = (double) item.wake_up_date.tv_sec +
                   ((double) item.wake_up_date.tv_usec / 1000000);
            diff = next - after_d;

            if (diff < 0) {
                if (fabs(diff) < DISCARD_THRESHOLD)
                    sleep = -1;
                else {
                    while ((diff < 0) && (fabs(diff) > DISCARD_THRESHOLD)) {
                        cout5 << "Discard one elt !!!!!!!!!!!!!" << endl;
                        WorkItem& tmp = const_cast<WorkItem&>(engine_.works.top());
                        if (tmp.type == POLL_ATTR)
                            err_out_of_sync(tmp);

                        tmp.wake_up_date = engine_.compute_new_date(tmp.wake_up_date, tmp.update);
                        engine_.works.push(tmp);
                        engine_.works.pop();
                        engine_.tune_ctr--;

                        WorkItem &top = const_cast<WorkItem&>(engine_.works.top());
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
}

void Tango::polling::PollingThread::err_out_of_sync(WorkItem &to_do) {
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

            struct EventSupplier::SuppliedEventData ad{};
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

void Tango::polling::PollingThread::tune_list(bool from_needed, long min_delta) {
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

        polling::PollingQueue new_works{};
        new_works.push(works.front());

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



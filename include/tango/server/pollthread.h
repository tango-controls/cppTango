//=============================================================================
//
// file :               PollThread.h
//
// description :        Include for the PollThread object. This class implements
//                      the polling thread
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
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//=============================================================================

#ifndef _POLLTHREAD_H
#define _POLLTHREAD_H

#include <tango.h>
#include <tango/server/pollobj.h>
#include <tango/server/utils.h>

#include <list>
#include <future>
#include <queue>
#include <experimental/optional>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#endif

namespace Tango {
    namespace threading {
        class enhanced_thread;
    }

    namespace polling {
        class Command;

        class AddObjCommand;

        class AddTriggerCommand;

        class RemObjCommand;

        class RemExtTriggerCommand;

        class RemDevCommand;

        class UpdatePollPeriodCommand;

        class StartPollingCommand;

        class StopPollingCommand;

        class TriggerPollingCommand;

        class ExitCommand;

        class PollingThread;

        class PollingQueue;

        class EventSystem;
    }

    struct WorkItem {
        DeviceImpl *dev;            // The device pointer (servant)
        //TODO replace with const reference
        //TODO filter list so it contains only related to this item objects
        vector<PollObj *> *poll_list;        // The device poll list
        std::chrono::milliseconds wake_up_date;    // The next wake up date
        std::chrono::milliseconds update;            // The update period (mS)
        PollObjType type;            // Object type (command/attr)
        //TODO remove when poll_list is filtered
        vector<string> name;            // Object name(s)
        void *values;  //pointer to AttributeValueList of some sort, i.e. _3, _4, _5
        std::chrono::milliseconds start_time;
        std::chrono::nanoseconds needed_time;    // Time needed to execute action
        std::chrono::milliseconds stop_time;
        std::map<std::string, DevFailed*> errors;
    };

//=============================================================================
//
//			The PollThread class
//
// description :	Class to store all the necessary information for the
//			polling thread. It's run() method is the thread code
//
//=============================================================================

    class ZmqEventSupplier;
    class NotifdEventSupplier;
    class PollThread;

    using PollThreadPtr = std::unique_ptr<Tango::PollThread>;

    //TODO move in to corresponding headers

    using CommandPtr = std::unique_ptr<polling::Command>;

    using PollingQueuePtr = std::unique_ptr<polling::PollingQueue>;

    using PollingThreadPtr = std::unique_ptr<polling::PollingThread>;

    using EventSystemPtr = std::unique_ptr<polling::EventSystem>;

    class TangoMonitor;

    //TODO rename to PollEngine
    class PollThread {

        //TODO merge into this class
        friend class PollingThreadInfo;

    public:


        static const int kPollLoop;
        static constexpr std::chrono::milliseconds kDiscardThreshold{20};//TODO original double value =0.02


        //TODO
        vector<std::chrono::milliseconds> auto_upd;
        vector<string> auto_name;
        //TODO
        vector<std::chrono::milliseconds> rem_upd;
        vector<string> rem_name;


        /**
         * Constructs PollThread
         *
         * @param name - name
         * @param polling_as_before_9
         * @constructor
         */
        PollThread(string &&, bool, Tango::ZmqEventSupplier *zmq_event_supplier,
                   Tango::NotifdEventSupplier *notifd_event_supplier);

        static PollThreadPtr create_instance_ptr(std::string&&, bool);

        /**
         *
         *
         * @return running polling thread id or thread::id()
         */
        thread::id id();

        void execute_cmd(polling::Command &&);

        void start_polling(){
            polling_stop_.store(false);

            start_thread_if_required();
        }

        void stop_polling(){
            polling_stop_.store(true);
        }

        /*TODO const*/ polling::PollingThread& polling_thread() {
            return *thread_;
        }

        std::experimental::optional<WorkItem>
        find_work_item(DeviceImpl *, PollObjType, chrono::milliseconds update, string cmd_name);

        std::experimental::optional<WorkItem>
        find_work_item(DeviceImpl *, PollObjType, const std::string&);

        std::experimental::optional<WorkItem>
        find_trigger(DeviceImpl *, PollObjType, std::string);

        /**
         * Removes all work items (and triggers) related to the device
         *
         * //TODO remove param - simply clear the queue - as this is not needed if we use poll thread per device
         */
        void remove_work_items_by(DeviceImpl *);

        /**
         *
         * @return true if works has been changed
         */
        bool discard_late_items();

        //TODO inject algorithm?
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
        std::chrono::milliseconds compute_next_sleep(bool);

        //TODO do we need this?
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
        //chrono::time_point compute_new_date(chrono::time_point, chrono::milliseconds);

        void print_work_items();

        void add_work_item(WorkItem &);
        void add_trigger(WorkItem&);

        WorkItem new_work_item(DeviceImpl *, /*TODO const*/ PollObj &);

        polling::EventSystem& get_event_system();
        /**
         *
         * @return true if tune is required
         */
        void adjust_work_items(WorkItem &);
        //+----------------------------------------------------------------------------------------------------------------
        //
        // method :
        //		PollThread::tune_work_items_list
        //
        // description :
        //		This method tunes the work list.
        //----------------------------------------------------------------------------------------------------------------
        void tune_work_items_list();//TODO was from_needed, min_delta which were always true and 0

        void set_need_two_tuning(bool);

        void reset_tune_counter(){ tune_counter_ = 0;}

        bool empty();
    private:
        friend class polling::PollingThread;


        bool analyze_work_list();

        void start_thread_if_required();
    private://fields
        atomic_bool polling_stop_;

        long tune_counter_;
        bool need_two_tuning;
        string name_;
        u_int previous_nb_late;

        bool polling_bef_9;
        PollingThreadPtr thread_;
        PollingQueuePtr works;

        PollingQueuePtr ext_trig_works;

        EventSystemPtr event_system_;
    };



    std::experimental::optional<WorkItem> find_work_item(polling::PollingQueue&, DeviceImpl*, std::string, PollObjType);

//
// Three macros
//

#define T_DIFF(A, B, C) \
    long delta_sec = B.tv_sec - A.tv_sec; \
    if (delta_sec == 0) \
        C = B.tv_usec - A.tv_usec; \
    else \
    { \
        C = ((delta_sec - 1) * 1000000) + (1000000 - A.tv_usec) + B.tv_usec; \
    }

#define T_ADD(A, B) \
    A.tv_usec = A.tv_usec + B; \
    while (A.tv_usec > 1000000) \
    { \
        A.tv_sec++; \
        A.tv_usec = A.tv_usec - 1000000; \
    }

#define T_DEC(A, B) \
    A.tv_usec = A.tv_usec - B; \
    if (A.tv_usec < 0) \
    { \
        A.tv_sec--; \
        A.tv_usec = 1000000 + A.tv_usec; \
    }

} // End of Tango namespace

#endif /* _POLLTHREAD_ */

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

    extern map<thread::id, string> kThreadNameMap;

    class PollThCmd;

    namespace threading {
        template<typename Item>
        class asymmetric_unbound_blocking_queue;

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

    using CommandPtr = std::unique_ptr<polling::Command>;

    typedef std::unique_ptr<threading::asymmetric_unbound_blocking_queue<PollThCmd>> PollThCmdQueuePtr;
//=============================================================================
//
//			The PollThCmd structure
//
// description :	This structure is used to shared data between the polling
//			thread and the main thread.
//
//=============================================================================



    //TODO remove
    enum PollCmdType {
        POLL_TIME_OUT,
        POLL_COMMAND,
        POLL_TRIGGER
    };


    //TODO remove
    struct PollThCmd {
        bool cmd_pending;    // The new command flag
        bool trigger;        // The external trigger flag
        PollCmdCode cmd_code;        // The command code
        DeviceImpl *dev;            // The device pointer (servant)
        long index;            // Index in the device poll_list
        string name;            // Object name
        PollObjType type;            // Object type (cmd/attr)
        int new_upd;        // New update period (For upd period com.)
    };


    struct WorkItem {
        DeviceImpl *dev;            // The device pointer (servant)
        //TODO replace with const reference
        //TODO filter list so it contains only related to this item objects
        vector<PollObj *> *poll_list;        // The device poll list
        std::chrono::time_point wake_up_date;    // The next wake up date
        std::chrono::milliseconds update;            // The update period (mS)
        PollObjType type;            // Object type (command/attr)
        //TODO remove when poll_list is filtered
        vector<string> name;            // Object name(s)
        void *values;  //pointer to AttributeValueList of some sort, i.e. _3, _4, _5
        std::chrono::time_point start_time;
        std::chrono::nanoseconds needed_time;    // Time needed to execute action
        std::chrono::time_point stop_time;
        vector<DevFailed*> errors;

        WorkItem(DeviceImpl *dev, vector<PollObj *> *poll_list, const timeval &wake_up_date, int update,
                 PollObjType type,
                 const vector<string> &name, const timeval &needed_time);
    };


//=============================================================================
//
//			The PollThread class
//
// description :	Class to store all the necessary information for the
//			polling thread. It's run() method is the thread code
//
//=============================================================================

    class TangoMonitor;

    //TODO rename to PollEngine
    class PollThread {

        //TODO merge into this class
        friend class PollingThreadInfo;

    public:
        PollThread(TangoMonitor &, string &&name, bool polling_as_before_tango_9);

        /**
         *
         *
         * @return running polling thread id or thread::id()
         */
        thread::id id();

        void execute_cmd(polling::Command &&);

        std::experimental::optional<WorkItem>
        find_work_item(DeviceImpl *, PollObjType, long update);

        std::experimental::optional<WorkItem> remove_work_item(DeviceImpl*,std::string, PollObjType);
        std::experimental::optional<WorkItem> remove_trigger(DeviceImpl*,std::string, PollObjType);
    private:
        //TODO redesign API so that these friends are no longer required, i.e. use public API
        friend class polling::AddObjCommand;

        friend class polling::AddTriggerCommand;

        friend class polling::RemObjCommand;

        friend class polling::RemExtTriggerCommand;

        friend class polling::RemDevCommand;

        friend class polling::UpdatePollPeriodCommand;

        friend class polling::StartPollingCommand;

        friend class polling::StopPollingCommand;

        friend class polling::TriggerPollingCommand;

        friend class polling::ExitCommand;

        friend class polling::PollingThread;

    public:
        //TODO inject algorithm?
        std::chrono::milliseconds compute_next_sleep();

        timeval compute_new_date(timeval, int);

        void print_work_items();

        void add_or_push(WorkItem &);

        WorkItem new_work_item(DeviceImpl *, /*TODO const*/ PollObj &);

        polling::EventSystem& get_event_system();
        /**
         *
         */
        void adjust_work_items(WorkItem&);


        void set_need_two_tuning(bool);

        void reset_tune_counter(){ tune_ctr = 0;}

        void set_time();
    protected:
        polling::PollingQueue &works;
        polling::PollingQueue &ext_trig_works;
#ifdef _TG_WINDOWS_
        struct _timeb		now_win;
        struct _timeb		after_win;
        double				ctr_frequency;
#endif
        struct timeval now;
        struct timeval after;
        atomic_bool polling_stop;

    private:
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
        void tune_list(bool from_needed, long min_delta);

        long tune_ctr;
        bool need_two_tuning;
        vector<long> auto_upd;
        vector<string> auto_name;
        vector<long> rem_upd;
        vector<string> rem_name;
        u_int previous_nb_late;
        bool polling_bef_9;

        ClntIdent dummy_cl_id;

        threading::enhanced_thread &thread_;
        string name_;

        polling::EventSystem& event_system_;
    };

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

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

#ifdef _TG_WINDOWS_
	#include <sys/types.h>
	#include <sys/timeb.h>
#endif

namespace Tango
{

    extern map<thread::id, string> kThreadNameMap;

    class PollThCmd;

    namespace threading {
        template<typename Item>
        class asymmetric_unbound_blocking_queue;
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



    enum PollCmdType
    {
        POLL_TIME_OUT,
        POLL_COMMAND,
        POLL_TRIGGER
    };


    struct PollThCmd
{
	bool			cmd_pending;	// The new command flag
	bool			trigger;		// The external trigger flag
	PollCmdCode		cmd_code;		// The command code
	DeviceImpl		*dev;			// The device pointer (servant)
	long			index;			// Index in the device poll_list
	string			name;			// Object name
	PollObjType		type;			// Object type (cmd/attr)
	int				new_upd;		// New update period (For upd period com.)
    PollCmdType     cmd_type;       //TODO replace with polymorphism
};


struct WorkItem
{
	DeviceImpl			*dev;			// The device pointer (servant)
    //TODO replace with const reference
	vector<PollObj *> 	*poll_list;		// The device poll list
	struct timeval		wake_up_date;	// The next wake up date
	int 				update;			// The update period (mS)
	PollObjType			type;			// Object type (command/attr)
	vector<string>		name;			// Object name(s)
	struct timeval		needed_time;	// Time needed to execute action

    WorkItem(DeviceImpl *dev, vector<PollObj *> *poll_list, const timeval &wake_up_date, int update, PollObjType type,
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

    //TODO make inner of Device
class PollThread
{
    friend class PollingThreadInfo;
public:
	PollThread(PollThCmd &, TangoMonitor &, string &&name);
	void run();
    /**
     * Starts this thread
     */
	void start();
    thread::id id(){
        return id_;
    }
    void add_command(PollThCmd&&);
    void execute_cmd(polling::Command &cmd);
    void set_local_cmd(PollThCmd &cmd) {local_cmd = cmd;}
	void set_polling_bef_9(bool _v) {polling_bef_9 = _v;}
private:
    friend class polling::AddObjCommand;
    friend class polling::AddTriggerCommand;
    friend class polling::RemObjCommand;
    friend class polling::RemExtTriggerCommand;
    friend class polling::RemDevCommand;
    friend class polling::UpdatePollPeriodCommand;
    friend class polling::StartPollingCommand;
    friend class polling::StopPollingCommand;
protected:
	CommandPtr get_command(long);
	void one_more_poll();
	void one_more_trigg();
	void compute_new_date(struct timeval &,int);
	void compute_sleep_time();
	void time_diff(struct timeval &,struct timeval &,struct timeval &);
	void poll_cmd(WorkItem &);
	void poll_attr(WorkItem &);

	void print_list();
	void insert_in_list(WorkItem &);
	void add_insert_in_list(WorkItem &);
	void tune_list(bool,long);
	void err_out_of_sync(WorkItem &);

    template <typename T> void robb_data(T &,T &);
    template <typename T> void copy_remaining(T &,T &);

    WorkItem new_work_item(DeviceImpl*, /*TODO const*/ PollObj&);

	PollThCmd			&shared_cmd;
	TangoMonitor		&p_mon;

	list<WorkItem>		works;
	vector<WorkItem>	ext_trig_works;

	PollThCmd			local_cmd;

#ifdef _TG_WINDOWS_
	struct _timeb		now_win;
	struct _timeb		after_win;
	double				ctr_frequency;
#endif
	struct timeval		now;
	struct timeval		after;
	atomic_long			sleep;
	atomic_bool			polling_stop;

private:
	CORBA::Any			in_any;
	DevVarStringArray	attr_names;
	AttributeValue		dummy_att;
	AttributeValue_3	dummy_att3;
	AttributeValue_4 	dummy_att4;
	AttributeValue_5	dummy_att5;
	long				tune_ctr;
	bool				need_two_tuning;
	vector<long>		auto_upd;
	vector<string>      auto_name;
	vector<long>        rem_upd;
	vector<string>      rem_name;
	u_int               previous_nb_late;
	bool                polling_bef_9;

	ClntIdent 			dummy_cl_id;
	CppClntIdent 		cci;

	thread thread_;
    thread::id id_;
	bool interrupted_;
    string name_;

        PollThCmdQueuePtr queue_;
        std::future<void> polling_future_;
};

//
// Three macros
//

#define T_DIFF(A,B,C) \
	long delta_sec = B.tv_sec - A.tv_sec; \
	if (delta_sec == 0) \
		C = B.tv_usec - A.tv_usec; \
	else \
	{ \
		C = ((delta_sec - 1) * 1000000) + (1000000 - A.tv_usec) + B.tv_usec; \
	}

#define T_ADD(A,B) \
	A.tv_usec = A.tv_usec + B; \
	while (A.tv_usec > 1000000) \
	{ \
		A.tv_sec++; \
		A.tv_usec = A.tv_usec - 1000000; \
	}

#define T_DEC(A,B) \
	A.tv_usec = A.tv_usec - B; \
	if (A.tv_usec < 0) \
	{ \
		A.tv_sec--; \
		A.tv_usec = 1000000 + A.tv_usec; \
	}

} // End of Tango namespace

#endif /* _POLLTHREAD_ */

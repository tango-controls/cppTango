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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
#include <pollobj.h>

#include <list>

#ifdef _TG_WINDOWS_
	#include <sys/types.h>
	#include <sys/timeb.h>
#endif

namespace Tango
{

//=============================================================================
//
//			The PollThCmd structure
//
// description :	This structure is used to shared data between the polling
//			thread and the main thread.
//
//=============================================================================

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
};


struct WorkItem
{
	DeviceImpl			*dev;			// The device pointer (servant)
	vector<PollObj *> 	*poll_list;		// The device poll list
	struct timeval		wake_up_date;	// The next wake up date
	int 				update;			// The update period (mS)
	PollObjType			type;			// Object type (command/attr)
	string				name;			// Object name
	struct timeval		needed_time;	// Time needed to execute action
};

enum PollCmdType
{
	POLL_TIME_OUT,
	POLL_COMMAND,
	POLL_TRIGGER
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

class PollThread: public omni_thread
{
public:
	PollThread(PollThCmd &,TangoMonitor &,bool);

	void *run_undetached(void *);
	void start() {start_undetached();}
	void execute_cmd();
	void set_local_cmd(PollThCmd &cmd) {local_cmd = cmd;}

protected:
	PollCmdType get_command(long);
	void one_more_poll();
	void one_more_trigg();
	void compute_new_date(struct timeval &,int);
	void compute_sleep_time();
	void time_diff(struct timeval &,struct timeval &,struct timeval &);
	void poll_cmd(WorkItem &);
	void poll_attr(WorkItem &);
	void eve_heartbeat();
	void store_subdev();

	void print_list();
	void insert_in_list(WorkItem &);
	void add_random_delay(struct timeval &);
	void tune_list(bool,long);
	void err_out_of_sync(WorkItem &);

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
	long				sleep;
	bool				polling_stop;

private:
	CORBA::Any			in_any;
	DevVarStringArray	attr_names;
	AttributeValue		dummy_att;
	AttributeValue_3	dummy_att3;
	AttributeValue_4 	dummy_att4;
	long				tune_ctr;
	bool				need_two_tuning;
	long				auto_upd;
	bool				send_heartbeat;

	ClntIdent 			dummy_cl_id;
	CppClntIdent 		cci;

public:
	static DeviceImpl 	*dev_to_del;
	static string	   	name_to_del;
	static PollObjType	type_to_del;
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

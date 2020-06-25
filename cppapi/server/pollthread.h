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
//
//=============================================================================

#ifndef _POLLTHREAD_H
#define _POLLTHREAD_H

#include <tango.h>
#include <pollobj.h>
#include <utils.h>
#include <tango_optional.h>
#include <poll_clock.h>

#include <list>
#include <utility>

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
	bool cmd_pending;               // The new command flag
	bool trigger;                   // The external trigger flag
	PollCmdCode cmd_code;           // The command code
	DeviceImpl* dev;                // The device pointer (servant)
	long index;	                    // Index in the device poll_list
	std::string name;               // Object name
	PollObjType type;               // Object type (cmd/attr)
	PollClock::duration new_upd;    // New update period (For upd period com.)
};


struct WorkItem
{
	DeviceImpl* dev;                    // The device pointer (servant)
	std::vector<PollObj*>* poll_list;   // The device poll list
	PollClock::time_point wake_up_date; // The next wake up date
	PollClock::duration update;         // The update period (mS)
	PollObjType type;                   // Object type (command/attr)
	std::vector<std::string> name;      // Object name(s)
	PollClock::duration needed_time;    // Time needed to execute action
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
	void set_polling_bef_9(bool _v) {polling_bef_9 = _v;}

protected:
	PollCmdType get_command();
	void one_more_poll();
	void one_more_trigg();
	void compute_sleep_time();
	void poll_cmd(WorkItem &);
	void poll_attr(WorkItem &);
	void eve_heartbeat();
	void store_subdev();
	void auto_unsub();

	void print_list();
	void insert_in_list(WorkItem &);
	void add_insert_in_list(WorkItem &);
	void tune_list(bool);
	void err_out_of_sync(WorkItem &);

    template <typename T> void robb_data(T &,T &);
    template <typename T> void copy_remaining(T &,T &);

	PollThCmd			&shared_cmd;
	TangoMonitor		&p_mon;

	std::list<WorkItem>		works;
	std::vector<WorkItem>	ext_trig_works;

	PollThCmd			local_cmd;

	PollClock::time_point now;
	PollClock::time_point after;
	tango_optional<PollClock::duration> sleep;

	bool				polling_stop;

private:
	CORBA::Any			in_any;
	DevVarStringArray	attr_names;
	AttributeValue		dummy_att;
	AttributeValue_3	dummy_att3;
	AttributeValue_4 	dummy_att4;
	AttributeValue_5	dummy_att5;
	long				tune_ctr;
	bool				need_two_tuning;
	bool				send_heartbeat;
	u_int				heartbeat_ctr;
	u_int               previous_nb_late;
	bool                polling_bef_9;

	std::vector<std::pair<PollClock::duration, std::string>> auto_upd;
	std::vector<std::pair<PollClock::duration, std::string>> rem_upd;

	ClntIdent 			dummy_cl_id;
	CppClntIdent 		cci;

public:
	static DeviceImpl 	*dev_to_del;
	static std::string	   	name_to_del;
	static PollObjType	type_to_del;
};

} // End of Tango namespace

#endif /* _POLLTHREAD_ */

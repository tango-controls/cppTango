//=============================================================================
//
// file :               DServer.h
//
// description :        Include for the DServer class. This class implements
//                      all the commands which are available for device
//			of the DServer class. There is one device of the
//			DServer class for each device server process
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
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

#ifndef _DSERVER_H
#define _DSERVER_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The DServer class
//
// description :	Class to implement all data members and commands for
//			device of the DServer class
//
//=============================================================================

typedef Tango::DeviceClass *(*Cpp_creator_ptr)(const char *);
typedef void (*ClassFactoryFuncPtr)(DServer *);


class DServer: public TANGO_BASE_CLASS
{
public :
	DServer(DeviceClass *,const char *,const char *,Tango::DevState,const char *);
	~DServer();

	Tango::DevVarStringArray *query_class();
	Tango::DevVarStringArray *query_device();
	Tango::DevVarStringArray *query_sub_device();
	void kill();
	void restart(string &);
	void restart_server();
	Tango::DevVarStringArray *query_class_prop(string &);
	Tango::DevVarStringArray *query_dev_prop(string &);

	Tango::DevVarStringArray *polled_device();
	Tango::DevVarStringArray *dev_poll_status(string &);
	void add_obj_polling(const Tango::DevVarLongStringArray *,bool with_db_upd = true,int delta_ms = 0);
	void upd_obj_polling_period(const Tango::DevVarLongStringArray *,bool with_db_upd = true);
	void rem_obj_polling(const Tango::DevVarStringArray *,bool with_db_upd = true);
	void stop_polling();
	void start_polling();
	void start_polling(PollingThreadInfo *);
	void add_event_heartbeat();
	void rem_event_heartbeat();

	void lock_device(const Tango::DevVarLongStringArray *);
	Tango::DevLong un_lock_device(const Tango::DevVarLongStringArray *);
	void re_lock_devices(const Tango::DevVarStringArray *);
	Tango::DevVarLongStringArray *dev_lock_status(Tango::ConstDevString);

	Tango::DevLong event_subscription_change(const Tango::DevVarStringArray *);
	Tango::DevVarLongStringArray *zmq_event_subscription_change(const Tango::DevVarStringArray *);
	void event_confirm_subscription(const Tango::DevVarStringArray *);

	void delete_devices();

#ifdef TANGO_HAS_LOG4TANGO
	void add_logging_target (const Tango::DevVarStringArray *argin);
	void remove_logging_target (const Tango::DevVarStringArray *argin);
	Tango::DevVarStringArray* get_logging_target (const std::string& dev_name);
	void set_logging_level (const Tango::DevVarLongStringArray *argin);
	Tango::DevVarLongStringArray* get_logging_level (const Tango::DevVarStringArray *argin);
	void stop_logging (void);
	void start_logging (void);
#endif

	string &get_process_name() {return process_name;}
	string &get_personal_name() {return instance_name;}
	string &get_instance_name() {return instance_name;}
	string &get_full_name() {return full_name;}
	string &get_fqdn() {return fqdn;}
	bool get_heartbeat_started() {return heartbeat_started;}
	void set_heartbeat_started(bool val) {heartbeat_started = val;}

	vector<DeviceClass *> &get_class_list() {return class_list;}
	virtual void init_device();

	unsigned long get_poll_th_pool_size() {return polling_th_pool_size;}
	void set_poll_th_pool_size(unsigned long val) {polling_th_pool_size = val;}
	bool get_opt_pool_usage() {return optimize_pool_usage;}
	vector<string> get_poll_th_conf() {return polling_th_pool_conf;}

	void check_lock_owner(DeviceImpl *,const char *,const char *);
	void check_upd_authorized(DeviceImpl *,int,PollObjType,string &);

	TANGO_IMP_EXP static void register_class_factory(ClassFactoryFuncPtr f_ptr) {class_factory_func_ptr = f_ptr;}
	void _add_class(DeviceClass *dc) {this->add_class(dc);}
	void _create_cpp_class(const char *c1,const char *c2) {this->create_cpp_class(c1,c2);}

	void mcast_event_for_att(string &,string &,vector<string> &);
	void mem_event_par(map<string, vector<EventPar> > &);
	void apply_event_par(map<string,vector<EventPar> > &);

	void mem_devices_interface(map<string,DevIntr> &);
	void changed_devices_interface(map<string,DevIntr> &);

    bool is_polling_bef_9_def() {return polling_bef_9_def;}
    bool get_polling_bef_9() {return polling_bef_9;}

	friend class ZmqEventSupplier;

protected :
	string							process_name;
	string							instance_name;
	string							full_name;
	string 							fqdn;

	vector<DeviceClass *>			class_list;

	time_t							last_heartbeat;
	time_t                          last_heartbeat_zmq;
	bool							heartbeat_started;

	unsigned long					polling_th_pool_size;
	vector<string>					polling_th_pool_conf;
	bool							optimize_pool_usage;

	static ClassFactoryFuncPtr 		class_factory_func_ptr;

private:
#if ((defined _TG_WINDOWS_) && (defined TANGO_HAS_DLL) && !(defined _TANGO_LIB))
	__declspec(dllexport) void class_factory();
#else
	void class_factory();
#endif
	void add_class(DeviceClass *);
	void create_cpp_class(const char *,const char *);
	void get_dev_prop(Tango::Util *);
    void event_subscription(string &,string &,string &,string &,string &,ChannelType,string &,int &,int &,DeviceImpl *,int l=0);
	void get_event_misc_prop(Tango::Util *);
	bool is_event_name(string &);
	bool is_ip_address(string &);

	bool			from_constructor;
	vector<string>	mcast_event_prop;

	DevLong         mcast_hops;
	DevLong         mcast_rate;
	DevLong         mcast_ivl;
	DevLong         zmq_pub_event_hwm;
	DevLong         zmq_sub_event_hwm;

	bool            polling_bef_9_def;
	bool            polling_bef_9;
};

class KillThread: public omni_thread
{
public:

	void *run_undetached(void *);
	void start() {start_undetached();}
};

class ServRestartThread: public omni_thread
{
public:
	ServRestartThread(DServer *dev):omni_thread(dev) {}

	void run(void *);
};

struct Pol
{
	PollObjType 	type;
	long			upd;
	string 			name;
};


/******************************************************************************
 *
 *			Some inline methods
 *
 ******************************************************************************/

inline bool DServer::is_event_name(string &str)
{
	if (count(str.begin(),str.end(),'/') != 3)
		return false;
	if (count(str.begin(),str.end(),'.') != 1)
		return false;
	return true;
}

inline bool DServer::is_ip_address(string &str)
{
	if (count(str.begin(),str.end(),'.') != 3)
		return false;
	return true;
}

} // End of namespace Tango

#endif

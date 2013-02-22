//////////////////////////////////////////////////////////////////
//
// ApiUtil.h - include file for TANGO device api class ApiUtil
//
//
// Copyright (C) :      2012
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
// $Revision: 20437 $
//
///////////////////////////////////////////////////////////////

#ifndef _APIUTIL_H
#define _APIUTIL_H


/****************************************************************************************
 * 																						*
 * 					The ApiUtil class													*
 * 					-----------------													*
 * 																						*
 ***************************************************************************************/

/**
 * Miscellaneous utility methods usefull in a Tango client.
 *
 * This class is a singleton. Therefore, it is not necessary to create it. It will be automatically done. A static
 * method allows a user to retrieve the instance
 *
 * $Author: taurel $
 * $Revision: 1 $
 *
 * @headerfile tango.h
 * @ingroup Client
 */

class ApiUtil
{
public:
/**
 * Retrieve the ApiUtil instance
 *
 * Return the ApiUtil singleton instance
 *
 * @return The singleton instance
 */
	TANGO_IMP_EXP static ApiUtil *instance();
/**
 * Destroy the ApiUtil instance
 *
 * Destroy the ApiUtil singleton instance.
 */
	TANGO_IMP_EXP static inline void cleanup()
	{if (_instance != NULL){delete _instance;_instance=NULL;}}
/**
 * Get environment variable
 *
 * Get environment variable. On Unixes OS, this call tries to get the variable in the caller environment then
 * in a file @e .tangorc in the user home directory and finally in a file @e /etc/tangorc. On Windows, this call looks
 * in the user environment then in a file stored in %TANGO_HOME%/tangorc. This method returns 0 of the
 * environment variable is found. Otherwise, it returns -1.
 *
 * @param [in] name The environment variable name
 * @param [out] value The environment variable value
 * @return Set to -1 if the environment varaibel is not found
 */
	TANGO_IMP_EXP static int get_env_var(const char *name,string &value);
/**
 * Get pending asynchronous requets number
 *
 * Return number of asynchronous pending requests (any device).
 *
 * @param [in] ty Asynchronous request type
 * @return Pending asynchronous request number
 */
	size_t pending_asynch_call(asyn_req_type ty)
	{if (ty==POLLING)return asyn_p_table->get_request_nb();
	 else if (ty==CALL_BACK)return asyn_p_table->get_cb_request_nb();
	 else return (asyn_p_table->get_request_nb()+asyn_p_table->get_cb_request_nb());}
/**
 * Fire callback methods for asynchronous request(s)
 *
 * Fire callback methods for all (any device) asynchronous requests (command and attribute) with already
 * arrived replied. Returns immediately if there is no replies already arrived or if there is no asynchronous
 * requests.
 */
	void get_asynch_replies();
/**
 * Fire callback methods for asynchronous request(s) with timeout
 *
 * Fire callback methods for all (any device) asynchronous requests (command and attributes) with already
 * arrived replied. Wait and block the caller for timeout milliseconds if they are some device asynchronous
 * requests which are not yet arrived. Returns immediately if there is no asynchronous request. If timeout is
 * set to 0, the call waits until all the asynchronous requests sent has received a reply.
 *
 * @param [in] timeout The timeout value
 */
	void get_asynch_replies(long timeout);
/**
 * Set asynchronous callback sub-model
 *
 * Set the asynchronous callback sub-model between the pull and push sub-model. See chapter 4.5 to read
 * the definition of these sub-model
 * By default, all Tango client using asynchronous callback model are in pull sub-model. This call must be
 * used to switch to the push sub-model. NOTE that in push sub-model, a separate thread is spawned to deal
 * with server replies.
 *
 * @param [in] csm The asynchronous callback sub-model
 */
	void set_asynch_cb_sub_model(cb_sub_model csm);
/**
 * Get asynchronous callback sub-model
 *
 * Get the asynchronous callback sub-model
 *
 * @return The asynchronous callback sub-model
 */
	cb_sub_model get_asynch_cb_sub_model() {return auto_cb;}

/// @privatesection

	CORBA::ORB_ptr get_orb() {return CORBA::ORB::_duplicate(_orb);}
	void set_orb(CORBA::ORB_ptr orb_in) {_orb = orb_in;}
	void create_orb();
	int	get_db_ind();
	int	get_db_ind(string &host,int port);
	vector<Database *> &get_db_vect() {return db_vect;}
	bool in_server() {return in_serv;}
	void in_server(bool serv) {in_serv = serv;}

	TangoSys_Pid get_client_pid() {return cl_pid;}
	void clean_locking_threads(bool clean=true);

	bool is_lock_exit_installed() {omni_mutex_lock guard(lock_th_map);return exit_lock_installed;}
	void set_lock_exit_installed(bool in) {omni_mutex_lock guard(lock_th_map);exit_lock_installed = in;}

	bool need_reset_already_flag() {return reset_already_executed_flag;}
	void need_reset_already_flag(bool in) {reset_already_executed_flag = in;}

	TANGO_IMP_EXP static inline bool _is_instance_null()
	{return _instance == NULL;}

//
// Utilities methods
//

	int get_user_connect_timeout() {return user_connect_timeout;}

	DevLong get_user_sub_hwm() {return user_sub_hwm;}
	void set_event_buffer_hwm(DevLong val) {if (user_sub_hwm == -1)user_sub_hwm=val;}

	void get_ip_from_if(vector<string> &);
	void print_error_message(const char *);

//
// EventConsumer related methods
//

	void create_notifd_event_consumer();
	void create_zmq_event_consumer();

	bool is_notifd_event_consumer_created() {return notifd_event_consumer != NULL;}
	NotifdEventConsumer *get_notifd_event_consumer();

	bool is_zmq_event_consumer_created() {return zmq_event_consumer != NULL;}
	ZmqEventConsumer *get_zmq_event_consumer();

//
// Asynchronous methods
//

	AsynReq	*get_pasyn_table() {return asyn_p_table;}

//
// Conv, between AttributeValuexxx and DeviceAttribute
//

	static void attr_to_device(const AttributeValue *,const AttributeValue_3 *,long,DeviceAttribute *);
	static void attr_to_device(const AttributeValue_4 *,long,DeviceAttribute *);

	static void device_to_attr(const DeviceAttribute &,AttributeValue_4 &);
	static void device_to_attr(const DeviceAttribute &,AttributeValue &,string &);

protected:
/// @privatesection
	ApiUtil();
	virtual ~ApiUtil();

	vector<Database *>			db_vect;
	omni_mutex					the_mutex;
	CORBA::ORB_ptr				_orb;
	bool						in_serv;

	cb_sub_model				auto_cb;
	CbThreadCmd					cb_thread_cmd;
	CallBackThread				*cb_thread_ptr;

	AsynReq						*asyn_p_table;

public:
/// @privatesection
	omni_mutex					lock_th_map;
	map<string,LockingThread>	lock_threads;

private:
    class ApiUtilExt
    {
    public:
        ApiUtilExt() {};
    };

	TANGO_IMP static ApiUtil 	*_instance;
	static omni_mutex			inst_mutex;
	bool						exit_lock_installed;
	bool						reset_already_executed_flag;

#ifdef HAS_UNIQUE_PTR
    unique_ptr<ApiUtilExt>      ext;
#else
	ApiUtilExt					*ext; 		// Class extension
#endif

    NotifdEventConsumer         *notifd_event_consumer;
    TangoSys_Pid		        cl_pid;
    int					        user_connect_timeout;
    ZmqEventConsumer            *zmq_event_consumer;
    vector<string>              host_ip_adrs;
    DevLong                     user_sub_hwm;
};

#endif /* _APIUTIL_H */

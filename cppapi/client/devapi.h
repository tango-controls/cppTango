//////////////////////////////////////////////////////////////////
//
// devapi.h - include file for TANGO device api
//
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013
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
///////////////////////////////////////////////////////////////

#ifndef _DEVAPI_H
#define _DEVAPI_H

#include <tango.h>
#include <tango_const.h>
#include <apiexcept.h>
#include <cbthread.h>
#include <lockthread.h>
#include <readers_writers_lock.h>

#include <bitset>

using namespace std;

namespace Tango {

/** @defgroup Client Client classes */

//
// forward declarations
//

class DeviceData;
class DeviceAttribute;
class DbDevice;
class DbAttribute;
class DbDatum;
class DbDevImportInfo;
class Database;
class AsynReq;
class NotifdEventConsumer;
class ZmqEventConsumer;
class CallBack;
class AttributeProxy;
class TangoMonitor;

//
// Some typedef
//

typedef vector<DbDatum> DbData;

typedef union
{
	TangoSys_Pid	LockerPid;
	unsigned long	UUID[4];
}LockerId;

struct LockerInfo
{
	LockerLanguage	ll;
	LockerId		li;
	string			locker_host;
	string			locker_class;
};

struct LockingThread
{
	TangoMonitor	*mon;
	LockThCmd		*shared;
	LockThread		*l_thread;
};

/**
 * Base structure for command information
 *
 * @headerfile tango.h
 */
typedef struct _DevCommandInfo
{
	string 		cmd_name;           ///< Command name
	long 		cmd_tag;            ///< Command tag
	long 		in_type;            ///< Input parameter date type
	long 		out_type;           ///< Output parameter data type
	string 		in_type_desc;       ///< Input parameter description
	string 		out_type_desc;      ///< Ouptput parameter description

///@privatesection
	bool operator==(const _DevCommandInfo &);
}DevCommandInfo;


struct AttributeDimension
{
	long dim_x;
	long dim_y;
};

/**
 * Command information data extension
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _CommandInfo : public DevCommandInfo
{
	Tango::DispLevel disp_level;    ///< Command display level

///@privatesection
	bool operator==(const _CommandInfo &);
}CommandInfo;

typedef vector<CommandInfo> CommandInfoList;

struct _DeviceInfo
{
	string 	dev_class;
	string 	server_id;
	string 	server_host;
	long 	server_version;
	string 	doc_url;
	string 	dev_type;
};

typedef _DeviceInfo DeviceInfo;

/**
 * Base structure for Attribute configuration
 *
 * @headerfile tango.h
 */
typedef struct _DeviceAttributeConfig
{
	string 			name;               ///< Name
	AttrWriteType 	writable;           ///< Writable type (Read, Write,...)
	AttrDataFormat 	data_format;        ///< Data format (Scalar, Spectrum,...)
	int 			data_type;          ///< Data type
	int 			max_dim_x;          ///< Max dim X
	int 			max_dim_y;          ///< Max dim Y
	string 			description;        ///< Description
	string 			label;              ///< Label
	string 			unit;               ///< Unit
	string 			standard_unit;      ///< Standard unit
	string 			display_unit;       ///< Display unit
	string 			format;             ///< Format
	string 			min_value;          ///< Min value
	string 			max_value;          ///< Max value
	string 			min_alarm;          ///< Min alarm
	string 			max_alarm;          ///< Max alarm
	string 			writable_attr_name; ///< Writable att. name
	vector<string> 	extensions;         ///< For future extensions

/// @privatesection
	bool operator==(const _DeviceAttributeConfig &);
}DeviceAttributeConfig;

/**
 * Attribute configuration data extension
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _AttributeInfo : public DeviceAttributeConfig
{
	Tango::DispLevel disp_level;        ///< Display level

/// @privatesection
	friend ostream &operator<<(ostream &,_AttributeInfo &);
	bool operator==(const _AttributeInfo &);
}AttributeInfo;

/**
 * Attribute alarms configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _AttributeAlarmInfo
{
	string			min_alarm;      ///< Min alarm level
	string			max_alarm;      ///< max alarm level
	string 			min_warning;    ///< Min warning level
	string			max_warning;    ///< Max warning level
	string			delta_t;        ///< Delta t RDS
	string			delta_val;      ///< Delta val RDS
	vector<string>	extensions;     ///< Future extensions

/// @privatesection
	bool operator==(const _AttributeAlarmInfo &);
}AttributeAlarmInfo;

/**
 * Attribute change event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _ChangeEventInfo
{
	string			rel_change;     ///< Relative change threshold
	string			abs_change;     ///< Absolute change threshold
	vector<string>	extensions;     ///< Future extensions
}ChangeEventInfo;

/**
 * Attribute periodic event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _PeriodicEventInfo
{
	string			period;         ///< Event period
	vector<string>	extensions;     ///< Future extensions
}PeriodicEventInfo;

/**
 * Attribute archive event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _ArchiveEventInfo
{
	string			archive_rel_change;     ///< Archive relative change threshold
	string			archive_abs_change;     ///< Archive abosolute change threshold
	string			archive_period;         ///< Archive event period
	vector<string>	extensions;             ///< Future exetnsions
}ArchiveEventInfo;

/**
 * Attribute event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _AttributeEventInfo
{
	ChangeEventInfo		ch_event;       ///< Attribute change event info
	PeriodicEventInfo	per_event;      ///< Attribute periodic event info
	ArchiveEventInfo	arch_event;     ///< Attribute archive event info

/// @privatesection
	bool operator==(const _AttributeEventInfo &);
}AttributeEventInfo;

/**
 * Possible memorized attribute type
 *
 * @ingroup Client
 * @headerfile tango.h
 */
typedef enum _AttrMemorizedType
{
	NOT_KNOWN,        		///< Device too old to  get this info
	NONE,      				///< Attribute not memorized
	MEMORIZED,      		///< Memorized attribute
	MEMORIZED_WRITE_INIT	///< Memorized attribute with memorized value writen at initialization
}AttrMemorizedType;


/**
 * Extended attribute configuration data
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef struct _AttributeInfoEx : public AttributeInfo
{
	string				root_attr_name;		///< Root attribute name
	AttrMemorizedType	memorized;			///< Attribute memorization type
	vector<string>		enum_labels;		///< Enumerated attribute labels
	AttributeAlarmInfo 	alarms;             ///< Attribute alarms
	AttributeEventInfo	events;             ///< Attribute events configuration
	vector<string>		sys_extensions;     ///< Future extensions

/// @privatesection
	_AttributeInfoEx & operator=(const AttributeConfig_2 *);
	_AttributeInfoEx & operator=(const AttributeConfig_3 *);
	_AttributeInfoEx & operator=(const AttributeConfig_5 *);

	friend ostream &operator<<(ostream &,_AttributeInfoEx &);
	bool operator==(const _AttributeInfoEx &);
}AttributeInfoEx;

typedef vector<AttributeInfo> AttributeInfoList;
typedef vector<AttributeInfoEx> AttributeInfoListEx;

//
// Can't use CALLBACK (without _) in the following enum because it's a
// pre-defined type on Windows....
//

/**
 * Possible asynchronous request type
 *
 * @ingroup Client
 * @headerfile tango.h
 */
enum asyn_req_type
{
	POLLING,        ///< Polling mode request
	CALL_BACK,      ///< Callback mode request
	ALL_ASYNCH      ///< All request
};

/**
 * Possible callback mode
 *
 * @ingroup Client
 * @headerfile tango.h
 */
enum cb_sub_model
{
	PUSH_CALLBACK,  ///< Callback push model
	PULL_CALLBACK   ///< Callback pull model
};

//
// Some define
//

#define 	CONNECTION_OK		1
#define 	CONNECTION_NOTOK	0

#define		PROT_SEP			"://"
#define		TACO_PROTOCOL		"taco"
#define		TANGO_PROTOCOL		"tango"

#define		MODIFIER			'#'
#define		DBASE_YES			"dbase=yes"
#define		DBASE_NO			"dbase=no"
#define		MODIFIER_DBASE_NO	"#dbase=no"

#define		HOST_SEP			':'
#define		PORT_SEP			'/'
#define		DEV_NAME_FIELD_SEP	'/'
#define		RES_SEP				"->"
#define		DEVICE_SEP			'/'

#define		FROM_IOR			"IOR"
#define		NOT_USED			"Unused"


/****************************************************************************************
 * 																						*
 * 					The ApiUtil class													*
 * 					-----------------													*
 * 																						*
 ***************************************************************************************/

#include "ApiUtil.h"


/****************************************************************************************
 * 																						*
 * 					The DeviceData class												*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/


#include "DeviceData.h"

/****************************************************************************************
 * 																						*
 * 					The DeviceAttribute class											*
 * 					-------------------------											*
 * 																						*
 ***************************************************************************************/


#include "DeviceAttribute.h"


/****************************************************************************************
 * 																						*
 * 					The xxxDataHistory classes											*
 * 					--------------------------											*
 * 																						*
 ***************************************************************************************/

/**
 * Fundamental type for receiving data from device command polling buffers
 *
 * This is the fundamental type for receiving data from device command polling buffers. This class inherits
 * from the Tango::DeviceData class. One instance of this class is created for each command result history.
 * Within this class, you find the command result data or the exception parameters, a flag indicating if the
 * command has failed when it was invoked by the device server polling thread and the date when the command
 * was executed. For history calls, it is not possible to returns command error as exception. See chapter
 * on Advanced Features in the
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a>
 * for all details regarding device polling.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */
class DeviceDataHistory: public DeviceData
{

public :
///@privatesection
//
// constructor methods
//

	DeviceDataHistory();
	DeviceDataHistory(int, int *,DevCmdHistoryList *);
	DeviceDataHistory(const DeviceDataHistory &);
	DeviceDataHistory & operator=(const DeviceDataHistory &);
#ifdef HAS_RVALUE
	DeviceDataHistory(DeviceDataHistory &&);
	DeviceDataHistory &operator=(DeviceDataHistory &&);
#endif

	~DeviceDataHistory();

///@publicsection
/**
 * Check if the record was a failure
 *
 * Returns a boolean set to true if the record in the polling buffer was a failure
 *
 * @return A boolean set to true if the record was a failure
 */
	bool has_failed() {return fail;}
/**
 * Get record polling date
 *
 * Returns the date when the device server polling thread has executed the command.
 *
 * @return The record polling date
 */
	TimeVal &get_date() {return time;}
/**
 * Get record error stack
 *
 * Return the error stack recorded by the device server polling thread in case of the command failed when it
 * was invoked.
 *
 * @return The record error stack
 */
	const DevErrorList &get_err_stack() {return err.in();}
/**
 * Print a DeviceDataHistory instance
 *
 * Is an utility function to easily print the contents of a DeviceDataHistory object. This function knows all
 * types which could be inserted in a DeviceDataHistory object and print them accordingly. It also prints date
 * and error stack in case the command returned an error.
 * @code
 * DeviceProxy *dev = new DeviceProxy(“...”);
 * int hist_depth = 4;
 * vector<DeviceDataHistory> *hist;
 *
 * hist = dev->command_history(“MyCommand”,hist_depth);
 *
 * for (int i = 0;i < hist_depth;i++)
 * {
 *    cout << (*hist)[i] << endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] ddh The instance to be printed
 */
	friend ostream &operator<<(ostream &str,DeviceDataHistory &ddh);

///@privatesection
// Three following methods for compatibility with older release

	bool failed() {return fail;}
	void failed(bool val) {fail = val;}
	void set_date(TimeVal &tv) {time = tv;}
	TimeVal &date() {return time;}
	const DevErrorList &errors() {return err.in();}
	void errors(DevErrorList_var &del) {err = del;}

protected:
///@privatesection
	bool 				fail;
	TimeVal 			time;
	DevErrorList_var 	err;

	DevCmdHistoryList 	*seq_ptr;
	int 				*ref_ctr_ptr;

private:
    class DeviceDataHistoryExt
    {
    public:
        DeviceDataHistoryExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DeviceDataHistoryExt>    ext_hist;
#else
	DeviceDataHistoryExt	            *ext_hist;		// Class extension
#endif
};

typedef vector<DeviceDataHistory> DeviceDataHistoryList;

/**
 * Fundamental type for receiving data from device attribute polling buffers
 *
 * This is the fundamental type for receiving data from device attribute polling buffers. This class inherits
 * from the Tango::DeviceAttribute class. One instance of this class is created for each attribute result history.
 * Within this class, you find the attribute result data or the exception parameters and a flag indicating if the
 * attribute has failed when it was invoked by the device server polling thread. For history calls, it is not
 * possible to returns attribute error as exception. See chapter on Advanced Features in the
 * <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>Tango book</a> for all details regarding
 * device polling.
 *
 * $Author$
 * $Revision$
 *
 * @headerfile tango.h
 * @ingroup Client
 */
class DeviceAttributeHistory: public DeviceAttribute
{

public :
///@privatesection
//
// constructor methods
//

	DeviceAttributeHistory();
	DeviceAttributeHistory(int, DevAttrHistoryList_var &);
	DeviceAttributeHistory(int, DevAttrHistoryList_3_var &);
	DeviceAttributeHistory(const DeviceAttributeHistory &);
	DeviceAttributeHistory & operator=(const DeviceAttributeHistory &);
#ifdef HAS_RVALUE
	DeviceAttributeHistory(DeviceAttributeHistory &&);
	DeviceAttributeHistory &operator=(DeviceAttributeHistory &&);
#endif

	~DeviceAttributeHistory();
///@publicsection
/**
 * Check if the record was a failure
 *
 * Returns a boolean set to true if the record in the polling buffer was a failure
 *
 * @return A boolean set to true if the record was a failure
 */
	bool has_failed() {return fail;}
/**
 * Print a DeviceAttributeHistory instance
 *
 * Is an utility function to easily print the contents of a DeviceAttributeHistory object. This function knows
 * all types which could be inserted in a DeviceAttributeHistory object and print them accordingly. It also
 * in case the attribute returned an error.
 * @code
 * DeviceProxy *dev = new DeviceProxy(“...”);
 * int hist_depth = 4;
 * vector<DeviceAttributeHistory> *hist;

 * hist = dev->attribute_history(“MyAttribute”,hist_depth);

 * for (int i = 0;i < hist_depth;i++)
 * {
 *    cout << (*hist)[i] << endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] dah The instance to be printed
 */
 	friend ostream &operator<<(ostream &str,DeviceAttributeHistory &dah);

///@privatesection
// Three following methods for compatibility with older release

	bool failed() {return fail;}
	void failed(bool val) {fail = val;}
	TimeVal &date() {return time;}
//	const DevErrorList &errors() {return err;}

protected:
///@privatesection
	bool 				fail;
	char 				compatibility_padding[16];

private:
    class DeviceAttributeHistoryExt
    {
    public:
        DeviceAttributeHistoryExt() {};
    };

#ifdef HAS_UNIQUE_PTR
    unique_ptr<DeviceAttributeHistoryExt>   ext_hist;
#else
	DeviceAttributeHistoryExt	            *ext_hist;	// Class extension
#endif
};


/****************************************************************************************
 * 																						*
 * 					The Connection class												*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/

#include "Connection.h"

/****************************************************************************************
 * 																						*
 * 					The DeviceProxy class												*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/

#include "DeviceProxy.h"

/****************************************************************************************
 * 																						*
 * 					The AttributeProxy class											*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/

#include "AttributeProxy.h"

/****************************************************************************************
 * 																						*
 * 					The DummyDeviceProxy class											*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/

class DummyDeviceProxy: public Tango::Connection
{
public:
	DummyDeviceProxy():Tango::Connection(true) {};

	virtual string get_corba_name(bool) {string str;return str;}
	virtual string build_corba_name() {string str;return str;}
	virtual int get_lock_ctr() {return 0;}
	virtual void set_lock_ctr(int) {};

	virtual string dev_name() {string str;return str;}

	int get_env_var(const char *cc,string &str_ref) {return Tango::Connection::get_env_var(cc,str_ref);}
};



///
///					Some inline methods
///					-------------------
///

inline ApiUtil *ApiUtil::instance()
{
	omni_mutex_lock lo(inst_mutex);

	if (_instance == NULL)
		_instance = new ApiUtil();
	return _instance;
}

inline long Connection::add_asyn_request(CORBA::Request_ptr req,TgRequest::ReqType req_type)
{
	omni_mutex_lock guard(asyn_mutex);
	long id = ApiUtil::instance()->get_pasyn_table()->store_request(req,req_type);
	pasyn_ctr++;
	return id;
}

inline void Connection::remove_asyn_request(long id)
{
	omni_mutex_lock guard(asyn_mutex);

	ApiUtil::instance()->get_pasyn_table()->remove_request(id);
	pasyn_ctr--;
}

inline void Connection::add_asyn_cb_request(CORBA::Request_ptr req,CallBack *cb,Connection *con,TgRequest::ReqType req_type)
{
	omni_mutex_lock guard(asyn_mutex);
	ApiUtil::instance()->get_pasyn_table()->store_request(req,cb,con,req_type);
	pasyn_cb_ctr++;
}

inline void Connection::remove_asyn_cb_request(Connection *con,CORBA::Request_ptr req)
{
	omni_mutex_lock guard(asyn_mutex);
	ApiUtil::instance()->get_pasyn_table()->remove_request(con,req);
	pasyn_cb_ctr--;
}

inline long Connection::get_pasyn_cb_ctr()
{
	long ret;
	asyn_mutex.lock();
	ret = pasyn_cb_ctr;
	asyn_mutex.unlock();
	return ret;
}

inline void Connection::dec_asynch_counter(asyn_req_type ty)
{
	omni_mutex_lock guard(asyn_mutex);
	if (ty==POLLING)
		pasyn_ctr--;
	else if (ty==CALL_BACK)
		pasyn_cb_ctr--;
}

inline void DeviceProxy::check_connect_adm_device()
{
	omni_mutex_lock guard(adm_dev_mutex);
	if (adm_device == NULL)
		connect_to_adm_device();
}

//
// For Tango 8 ZMQ event system
//

inline int DeviceProxy::subscribe_event (const string &attr_name, EventType event, CallBack *callback)
{
    vector<string> filt;
	return subscribe_event (attr_name,event,callback,filt,false);
}

inline int DeviceProxy::subscribe_event (const string &attr_name, EventType event,
                                 CallBack *callback,bool stateless)
{
    vector<string> filt;
    return subscribe_event(attr_name,event,callback,filt,stateless);
}

inline int DeviceProxy::subscribe_event (const string &attr_name, EventType event,
                                 int event_queue_size,bool stateless)
{
    vector<string> filt;
    return subscribe_event(attr_name,event,event_queue_size,filt,stateless);
}

///
///					Some macros
///					-----------
///

#define READ_ATT_EXCEPT(NAME_CHAR) \
		catch (Tango::ConnectionFailed &e) \
		{ \
			TangoSys_OMemStream desc; \
			desc << "Failed to read_attribute on device " << device_name; \
			desc << ", attribute " << NAME_CHAR << ends; \
			ApiConnExcept::re_throw_exception(e,(const char*)"API_AttributeFailed", \
                        	desc.str(), (const char*)"DeviceProxy::read_attribute()"); \
		} \
		catch (Tango::DevFailed &e) \
		{ \
			TangoSys_OMemStream desc; \
			desc << "Failed to read_attribute on device " << device_name; \
			desc << ", attribute " << NAME_CHAR << ends; \
			Except::re_throw_exception(e,(const char*)"API_AttributeFailed", \
                        	desc.str(), (const char*)"DeviceProxy::read_attribute()"); \
		} \
		catch (CORBA::TRANSIENT &trans) \
		{ \
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","read_attribute"); \
		} \
		catch (CORBA::OBJECT_NOT_EXIST &one) \
		{ \
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch || one.minor() == 0) \
			{ \
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","read_attribute"); \
			} \
			else \
			{ \
				set_connection_state(CONNECTION_NOTOK); \
				TangoSys_OMemStream desc; \
				desc << "Failed to read_attribute on device " << device_name << ends; \
				ApiCommExcept::re_throw_exception(one, \
							      (const char*)"API_CommunicationFailed", \
                        				      desc.str(), \
							      (const char*)"DeviceProxy::read_attribute()"); \
			} \
		} \
		catch (CORBA::COMM_FAILURE &comm) \
		{ \
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply) \
			{ \
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","read_attribute"); \
			} \
			else \
			{ \
				set_connection_state(CONNECTION_NOTOK); \
				TangoSys_OMemStream desc; \
				desc << "Failed to read_attribute on device " << device_name << ends; \
				ApiCommExcept::re_throw_exception(comm, \
							      (const char*)"API_CommunicationFailed", \
                        				      desc.str(), \
							      (const char*)"DeviceProxy::read_attribute()"); \
			} \
		} \
		catch (CORBA::SystemException &ce) \
        { \
			set_connection_state(CONNECTION_NOTOK); \
			TangoSys_OMemStream desc; \
			desc << "Failed to read_attribute on device " << device_name << ends; \
			ApiCommExcept::re_throw_exception(ce, \
						      (const char*)"API_CommunicationFailed", \
                        			      desc.str(), \
						      (const char*)"DeviceProxy::read_attribute()"); \
		}


#define	COPY_BASE_CONFIG(A) \
	(*dev_attr_config)[i].name = A[i].name; \
	(*dev_attr_config)[i].writable = A[i].writable; \
	(*dev_attr_config)[i].data_format = A[i].data_format; \
	(*dev_attr_config)[i].data_type = A[i].data_type; \
	(*dev_attr_config)[i].max_dim_x = A[i].max_dim_x; \
	(*dev_attr_config)[i].max_dim_y = A[i].max_dim_y; \
	(*dev_attr_config)[i].description = A[i].description; \
	(*dev_attr_config)[i].label = A[i].label; \
	(*dev_attr_config)[i].unit = A[i].unit; \
	(*dev_attr_config)[i].standard_unit = A[i].standard_unit; \
	(*dev_attr_config)[i].display_unit = A[i].display_unit; \
	(*dev_attr_config)[i].format = A[i].format; \
	(*dev_attr_config)[i].min_value = A[i].min_value; \
	(*dev_attr_config)[i].max_value = A[i].max_value; \
	(*dev_attr_config)[i].writable_attr_name = A[i].writable_attr_name; \
	(*dev_attr_config)[i].extensions.resize(A[i].extensions.length()); \
	for (size_t j=0; j<A[i].extensions.length(); j++) \
	{ \
		(*dev_attr_config)[i].extensions[j] = A[i].extensions[j]; \
	}


#define	COPY_ALARM_CONFIG(A) \
	(*dev_attr_config)[i].alarms.min_alarm = A[i].att_alarm.min_alarm; \
	(*dev_attr_config)[i].alarms.max_alarm = A[i].att_alarm.max_alarm; \
	(*dev_attr_config)[i].alarms.min_warning = A[i].att_alarm.min_warning; \
	(*dev_attr_config)[i].alarms.max_warning = A[i].att_alarm.max_warning; \
	(*dev_attr_config)[i].alarms.delta_t = A[i].att_alarm.delta_t; \
	(*dev_attr_config)[i].alarms.delta_val = A[i].att_alarm.delta_val; \
	(*dev_attr_config)[i].alarms.extensions.resize(A[i].att_alarm.extensions.length()); \
	for (size_t j=0; j<A[i].att_alarm.extensions.length(); j++) \
	{ \
		(*dev_attr_config)[i].alarms.extensions[j] = A[i].att_alarm.extensions[j]; \
	}


#define	COPY_EVENT_CONFIG(A) \
	(*dev_attr_config)[i].events.ch_event.rel_change = A[i].event_prop.ch_event.rel_change; \
	(*dev_attr_config)[i].events.ch_event.abs_change = A[i].event_prop.ch_event.abs_change; \
	(*dev_attr_config)[i].events.ch_event.extensions.resize(A[i].event_prop.ch_event.extensions.length()); \
	for (size_t j=0; j<A[i].event_prop.ch_event.extensions.length(); j++) \
	{ \
		(*dev_attr_config)[i].events.ch_event.extensions[j] = A[i].event_prop.ch_event.extensions[j]; \
	} \
\
	(*dev_attr_config)[i].events.per_event.period = A[i].event_prop.per_event.period; \
	(*dev_attr_config)[i].events.per_event.extensions.resize(A[i].event_prop.per_event.extensions.length()); \
	for (size_t j=0; j<A[i].event_prop.per_event.extensions.length(); j++) \
	{ \
		(*dev_attr_config)[i].events.per_event.extensions[j] = A[i].event_prop.per_event.extensions[j]; \
	} \
\
	(*dev_attr_config)[i].events.arch_event.archive_rel_change = A[i].event_prop.arch_event.rel_change; \
	(*dev_attr_config)[i].events.arch_event.archive_abs_change = A[i].event_prop.arch_event.abs_change; \
	(*dev_attr_config)[i].events.arch_event.archive_period = A[i].event_prop.arch_event.period; \
	(*dev_attr_config)[i].events.arch_event.extensions.resize(A[i].event_prop.arch_event.extensions.length()); \
	for (size_t j=0; j<A[i].event_prop.arch_event.extensions.length(); j++) \
	{ \
		(*dev_attr_config)[i].events.arch_event.extensions[j] = A[i].event_prop.arch_event.extensions[j]; \
	}


///
/// 				Small utility classes
///					---------------------


class AutoConnectTimeout
{
public:
	AutoConnectTimeout(unsigned int to) {omniORB::setClientConnectTimeout((CORBA::ULong)to);}
	~AutoConnectTimeout() {omniORB::setClientConnectTimeout(0);}
};

} // End of Tango namespace

#endif /* _DEVAPI_H */

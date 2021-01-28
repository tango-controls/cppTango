//=====================================================================================================================
//
// devapi.h - include file for TANGO device api
//
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
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//====================================================================================================================

#ifndef _DEVAPI_H
#define _DEVAPI_H

#include <tango.h>
#include <tango_const.h>
#include <apiexcept.h>
#include <cbthread.h>
#include <lockthread.h>
#include <readers_writers_lock.h>

#include <bitset>

#ifdef TANGO_USE_USING_NAMESPACE
  using namespace std;
#endif

namespace Tango
{

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

typedef std::vector<DbDatum> DbData;

typedef union
{
	TangoSys_Pid	LockerPid;
	unsigned long	UUID[4];
}LockerId;

struct LockerInfo
{
	LockerLanguage	ll;
	LockerId		li;
	std::string			locker_host;
	std::string			locker_class;
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
#ifdef GEN_DOC
typedef struct DevCommandInfo
#else
typedef struct _DevCommandInfo
#endif
{
	std::string 		cmd_name;           ///< The command name
	long 		cmd_tag;            ///< The command tag
	long 		in_type;            ///< Input parameter data type
	long 		out_type;           ///< Output parameter data type
	std::string 		in_type_desc;       ///< Input parameter description
	std::string 		out_type_desc;      ///< Ouptput parameter description

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
#ifdef GEN_DOC
typedef struct CommandInfo : public DevCommandInfo
#else
typedef struct _CommandInfo : public DevCommandInfo
#endif
{
	Tango::DispLevel disp_level;    ///< The command display level

/// @privatesection
	friend std::ostream &operator<<(std::ostream &, const _CommandInfo &);
	bool operator==(const _CommandInfo &);
}CommandInfo;

/**
 * A vector of CommandInfo structure
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef std::vector<CommandInfo> CommandInfoList;

struct _DeviceInfo
{
	std::string 	dev_class;
	std::string 	server_id;
	std::string 	server_host;
	long 	server_version;
	std::string 	doc_url;
	std::string 	dev_type;
};

typedef _DeviceInfo DeviceInfo;

/**
 * Base structure for Attribute configuration
 *
 * @headerfile tango.h
 */
#ifdef GEN_DOC
typedef struct DeviceAttributeConfig
#else
typedef struct _DeviceAttributeConfig
#endif
{
	std::string 			name;               ///< Name
	AttrWriteType 	writable;           ///< Writable type (Read, Write,...)
	AttrDataFormat 	data_format;        ///< Data format (Scalar, Spectrum,...)
	int 			data_type;          ///< Data type
	int 			max_dim_x;          ///< Max dim X
	int 			max_dim_y;          ///< Max dim Y
	std::string 			description;        ///< Description
	std::string 			label;              ///< Label
	std::string 			unit;               ///< Unit
	std::string 			standard_unit;      ///< Standard unit
	std::string 			display_unit;       ///< Display unit
	std::string 			format;             ///< Format
	std::string 			min_value;          ///< Min value
	std::string 			max_value;          ///< Max value
	std::string 			min_alarm;          ///< Min alarm
	std::string 			max_alarm;          ///< Max alarm
	std::string 			writable_attr_name; ///< Writable att. name
	std::vector<std::string> 	extensions;         ///< For future extensions

/// @privatesection
	bool operator==(const _DeviceAttributeConfig &);
}DeviceAttributeConfig;

/**
 * Attribute configuration data extension
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct AttributeInfo : public DeviceAttributeConfig
#else
typedef struct _AttributeInfo : public DeviceAttributeConfig
#endif
{
	Tango::DispLevel disp_level;        ///< Display level

/// @privatesection
	friend std::ostream &operator<<(std::ostream &, const _AttributeInfo &);
	bool operator==(const _AttributeInfo &);
}AttributeInfo;

/**
 * Attribute alarms configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct AttributeAlarmInfo
#else
typedef struct _AttributeAlarmInfo
#endif
{
	std::string			min_alarm;      ///< Min alarm level
	std::string			max_alarm;      ///< max alarm level
	std::string 			min_warning;    ///< Min warning level
	std::string			max_warning;    ///< Max warning level
	std::string			delta_t;        ///< Delta t RDS
	std::string			delta_val;      ///< Delta val RDS
	std::vector<std::string>	extensions;     ///< Future extensions

/// @privatesection
	bool operator==(const _AttributeAlarmInfo &);
}AttributeAlarmInfo;

/**
 * Attribute change event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct ChangeEventInfo
#else
typedef struct _ChangeEventInfo
#endif
{
	std::string			rel_change;     ///< Relative change threshold
	std::string			abs_change;     ///< Absolute change threshold
	std::vector<std::string>	extensions;     ///< Future extensions
}ChangeEventInfo;

/**
 * Attribute periodic event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct PeriodicEventInfo
#else
typedef struct _PeriodicEventInfo
#endif
{
	std::string			period;         ///< Event period
	std::vector<std::string>	extensions;     ///< Future extensions
}PeriodicEventInfo;

/**
 * Attribute archive event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct ArchiveEventInfo
#else
typedef struct _ArchiveEventInfo
#endif
{
	std::string			archive_rel_change;     ///< Archive relative change threshold
	std::string			archive_abs_change;     ///< Archive abosolute change threshold
	std::string			archive_period;         ///< Archive event period
	std::vector<std::string>	extensions;             ///< Future exetnsions
}ArchiveEventInfo;

/**
 * Attribute event configuration
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct AttributeEventInfo
#else
typedef struct _AttributeEventInfo
#endif
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
	NONE,      				///< The attribute is not memorized
	MEMORIZED,      		///< Memorized attribute
	MEMORIZED_WRITE_INIT	///< Memorized attribute with memorized value writen at initialization
}AttrMemorizedType;


/**
 * Extended attribute configuration data
 *
 * @headerfile tango.h
 * @ingroup Client
 */
#ifdef GEN_DOC
typedef struct AttributeInfoEx: public AttributeInfo
#else
typedef struct _AttributeInfoEx : public AttributeInfo
#endif
{
	std::string				root_attr_name;		///< Root attribute name (in case of forwarded attribute)
	AttrMemorizedType	memorized;			///< The attribute memorization type
	std::vector<std::string>		enum_labels;		///< Enumerated attribute labels
	AttributeAlarmInfo 	alarms;             ///< The attribute alarms
	AttributeEventInfo	events;             ///< The attribute events configuration
	std::vector<std::string>		sys_extensions;     ///< Future extensions

/// @privatesection
	_AttributeInfoEx & operator=(const AttributeConfig_2 *);
	_AttributeInfoEx & operator=(const AttributeConfig_3 *);
	_AttributeInfoEx & operator=(const AttributeConfig_5 *);

	friend std::ostream &operator<<(std::ostream &, const _AttributeInfoEx &);
	bool operator==(const _AttributeInfoEx &);
}AttributeInfoEx;

typedef std::vector<AttributeInfo> AttributeInfoList;

/**
 * vector of AttributeInfoEx structure
 *
 * @headerfile tango.h
 * @ingroup Client
 */
typedef std::vector<AttributeInfoEx> AttributeInfoListEx;

/**
 * Base structure for pipe information
 *
 * @headerfile tango.h
 */
#ifdef GEN_DOC
typedef struct PipeInfo
#else
typedef struct _PipeInfo
#endif
{
	std::string 					name;          		///< Pipe name
	std::string					description;		///< Pipe description
	std::string 					label;       		///< Pipe label
	Tango::DispLevel 		disp_level;        	///< Display level
	Tango::PipeWriteType 	writable;           ///< Writable type (Read, Read-Write)
	std::vector<std::string> 			extensions;         ///< For future extensions

///@privatesection
	friend std::ostream &operator<<(std::ostream &, const _PipeInfo &);
//	bool operator==(const _PipeInfo &);
}PipeInfo;

// TODO: Pipe -> Change the type for writable to replace PIPE_READ, PIPE_READ_WRITE by READ, READ_WRITE (IDL limitation)

typedef std::vector<PipeInfo> PipeInfoList;

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
 * 					The DevicePipe class												*
 * 					--------------------												*
 * 																						*
 ***************************************************************************************/


#include "DevicePipe.h"

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
	DeviceDataHistory(DeviceDataHistory &&);
	DeviceDataHistory &operator=(DeviceDataHistory &&);

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
 * std::vector<DeviceDataHistory> *hist;
 *
 * hist = dev->command_history(“MyCommand”,hist_depth);
 *
 * for (int i = 0;i < hist_depth;i++)
 * {
 *    cout << (*hist)[i] << std::endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] ddh The instance to be printed
 */
	friend std::ostream &operator<<(std::ostream &str, const DeviceDataHistory &ddh);

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

    std::unique_ptr<DeviceDataHistoryExt>    ext_hist;
};

typedef std::vector<DeviceDataHistory> DeviceDataHistoryList;

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
	DeviceAttributeHistory(DeviceAttributeHistory &&);
	DeviceAttributeHistory &operator=(DeviceAttributeHistory &&);

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
 * std::vector<DeviceAttributeHistory> *hist;

 * hist = dev->attribute_history(“MyAttribute”,hist_depth);

 * for (int i = 0;i < hist_depth;i++)
 * {
 *    cout << (*hist)[i] << std::endl;
 * }
 * delete hist;
 * @endcode
 *
 * @param [in] str The printing stream
 * @param [in] dah The instance to be printed
 */
	friend std::ostream &operator<<(std::ostream &str, const DeviceAttributeHistory &dah);

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

    std::unique_ptr<DeviceAttributeHistoryExt>   ext_hist;
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

	virtual std::string get_corba_name(bool) {std::string str;return str;}
	virtual std::string build_corba_name() {std::string str;return str;}
	virtual int get_lock_ctr() {return 0;}
	virtual void set_lock_ctr(int) {};

	virtual std::string dev_name() {std::string str;return str;}

	int get_env_var(const char *cc,std::string &str_ref) {return Tango::Connection::get_env_var(cc,str_ref);}
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

inline int DeviceProxy::subscribe_event (const std::string &attr_name, EventType event, CallBack *callback)
{
    std::vector<std::string> filt;
    return subscribe_event (attr_name,event,callback,filt,false);
}

inline int DeviceProxy::subscribe_event (const std::string &attr_name, EventType event,
                                 CallBack *callback,bool stateless)
{
    std::vector<std::string> filt;
    return subscribe_event(attr_name,event,callback,filt,stateless);
}

inline int DeviceProxy::subscribe_event (const std::string &attr_name, EventType event,
                                 int event_queue_size,bool stateless)
{
    std::vector<std::string> filt;
    return subscribe_event(attr_name,event,event_queue_size,filt,stateless);
}

///
///					Some macros
///					-----------
///

#define READ_ATT_EXCEPT(NAME_CHAR,OBJ) \
		catch (Tango::ConnectionFailed &e) \
		{ \
			TangoSys_OMemStream desc; \
			desc << "Failed to read_attribute on device " << device_name; \
			desc << ", attribute " << NAME_CHAR << std::ends; \
			TANGO_RETHROW_API_EXCEPTION(ApiConnExcept, e, API_AttributeFailed, desc.str()); \
		} \
		catch (Tango::DevFailed &e) \
		{ \
			TangoSys_OMemStream desc; \
			desc << "Failed to read_attribute on device " << device_name; \
			desc << ", attribute " << NAME_CHAR << std::ends; \
			TANGO_RETHROW_EXCEPTION(e, API_AttributeFailed, desc.str()); \
		} \
		catch (CORBA::TRANSIENT &trans) \
		{ \
			TRANSIENT_NOT_EXIST_EXCEPT(trans,"DeviceProxy","read_attribute",OBJ); \
		} \
		catch (CORBA::OBJECT_NOT_EXIST &one) \
		{ \
			if (one.minor() == omni::OBJECT_NOT_EXIST_NoMatch || one.minor() == 0) \
			{ \
				TRANSIENT_NOT_EXIST_EXCEPT(one,"DeviceProxy","read_attribute",OBJ); \
			} \
			else \
			{ \
				set_connection_state(CONNECTION_NOTOK); \
				TangoSys_OMemStream desc; \
				desc << "Failed to read_attribute on device " << device_name << std::ends; \
				TANGO_RETHROW_API_EXCEPTION(ApiCommExcept, one, API_CommunicationFailed, desc.str()); \
			} \
		} \
		catch (CORBA::COMM_FAILURE &comm) \
		{ \
			if (comm.minor() == omni::COMM_FAILURE_WaitingForReply) \
			{ \
				TRANSIENT_NOT_EXIST_EXCEPT(comm,"DeviceProxy","read_attribute",OBJ); \
			} \
			else \
			{ \
				set_connection_state(CONNECTION_NOTOK); \
				TangoSys_OMemStream desc; \
				desc << "Failed to read_attribute on device " << device_name << std::ends; \
				TANGO_RETHROW_API_EXCEPTION(ApiCommExcept, comm, API_CommunicationFailed, desc.str()); \
			} \
		} \
		catch (CORBA::SystemException &ce) \
		{ \
			set_connection_state(CONNECTION_NOTOK); \
			TangoSys_OMemStream desc; \
			desc << "Failed to read_attribute on device " << device_name << std::ends; \
			TANGO_RETHROW_API_EXCEPTION(ApiCommExcept, ce, API_CommunicationFailed, desc.str()); \
		}


#define	COPY_BASE_CONFIG(A,B) \
	A[i].name = B[i].name; \
	A[i].writable = B[i].writable; \
	A[i].data_format = B[i].data_format; \
	A[i].data_type = B[i].data_type; \
	A[i].max_dim_x = B[i].max_dim_x; \
	A[i].max_dim_y = B[i].max_dim_y; \
	A[i].description = B[i].description; \
	A[i].label = B[i].label; \
	A[i].unit = B[i].unit; \
	A[i].standard_unit = B[i].standard_unit; \
	A[i].display_unit = B[i].display_unit; \
	A[i].format = B[i].format; \
	A[i].min_value = B[i].min_value; \
	A[i].max_value = B[i].max_value; \
	A[i].writable_attr_name = B[i].writable_attr_name; \
	A[i].extensions.resize(B[i].extensions.length()); \
	for (size_t j=0; j<B[i].extensions.length(); j++) \
	{ \
		A[i].extensions[j] = B[i].extensions[j]; \
	}


#define	COPY_ALARM_CONFIG(A,B) \
	A[i].alarms.min_alarm = B[i].att_alarm.min_alarm; \
	A[i].alarms.max_alarm = B[i].att_alarm.max_alarm; \
	A[i].alarms.min_warning = B[i].att_alarm.min_warning; \
	A[i].alarms.max_warning = B[i].att_alarm.max_warning; \
	A[i].alarms.delta_t = B[i].att_alarm.delta_t; \
	A[i].alarms.delta_val = B[i].att_alarm.delta_val; \
	A[i].alarms.extensions.resize(B[i].att_alarm.extensions.length()); \
	for (size_t j=0; j<B[i].att_alarm.extensions.length(); j++) \
	{ \
		A[i].alarms.extensions[j] = B[i].att_alarm.extensions[j]; \
	}


#define	COPY_EVENT_CONFIG(A,B) \
	A[i].events.ch_event.rel_change = B[i].event_prop.ch_event.rel_change; \
	A[i].events.ch_event.abs_change = B[i].event_prop.ch_event.abs_change; \
	A[i].events.ch_event.extensions.resize(B[i].event_prop.ch_event.extensions.length()); \
	for (size_t j=0; j<B[i].event_prop.ch_event.extensions.length(); j++) \
	{ \
		A[i].events.ch_event.extensions[j] = B[i].event_prop.ch_event.extensions[j]; \
	} \
\
	A[i].events.per_event.period = B[i].event_prop.per_event.period; \
	A[i].events.per_event.extensions.resize(B[i].event_prop.per_event.extensions.length()); \
	for (size_t j=0; j<B[i].event_prop.per_event.extensions.length(); j++) \
	{ \
		A[i].events.per_event.extensions[j] = B[i].event_prop.per_event.extensions[j]; \
	} \
\
	A[i].events.arch_event.archive_rel_change = B[i].event_prop.arch_event.rel_change; \
	A[i].events.arch_event.archive_abs_change = B[i].event_prop.arch_event.abs_change; \
	A[i].events.arch_event.archive_period = B[i].event_prop.arch_event.period; \
	A[i].events.arch_event.extensions.resize(B[i].event_prop.arch_event.extensions.length()); \
	for (size_t j=0; j<B[i].event_prop.arch_event.extensions.length(); j++) \
	{ \
		A[i].events.arch_event.extensions[j] = B[i].event_prop.arch_event.extensions[j]; \
	}


///
/// 				Small utility classes
///					---------------------


class AutoConnectTimeout
{
public:
	AutoConnectTimeout(unsigned int to) {omniORB::setClientConnectTimeout((CORBA::ULong)to);}
	~AutoConnectTimeout() {omniORB::setClientConnectTimeout(NARROW_CLNT_TIMEOUT);}
};

} // End of Tango namespace

#endif /* _DEVAPI_H */

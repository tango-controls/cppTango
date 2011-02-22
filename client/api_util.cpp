static const char *RcsId = "$Id$\n$Name$";

//
// cpp 	- C++ source code file for TANGO api class ApiUtil
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- May 2002
//
// $Log$
// Revision 3.11  2007/04/16 14:55:16  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.10  2007/03/06 08:20:45  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.9  2007/02/16 10:38:52  taurel
// - Implementing a new types of event on the Attribute configuration
//
// Revision 3.8  2005/05/10 13:51:21  taurel
// - Minor changes for Windows compiler
//
// Revision 3.7  2005/05/04 11:52:18  taurel
// - Changes for 32<-->64 bits data exchange
// - Fix a bug in the DeviceAttribute::has_failed() method (devapi.h file)
//
// Revision 3.6  2005/04/15 11:35:05  taurel
// - Changes to support Tango on 64 bits computer
// - Support for Linux 2.6 kernel with NPTL (Signal management)
//
// Revision 3.5  2005/01/13 08:36:36  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.4.2.5  2004/11/26 13:44:06  taurel
// - Fix some bug in method for printing one attribute config object
// - DeviceData and DeviceAttribute default mode is to throw exception when trying to extract something from an empty instance
// - Fix two small memory leaks in filedatabase
//
// Revision 3.4.2.4  2004/10/27 05:58:10  taurel
// - Some minor changes to compile on all our supported platforms
//
// Revision 3.4.2.3  2004/10/22 11:23:16  taurel
// Added warning alarm
// Change attribute config. It now includes alarm and event parameters
// Array attribute property now supported
// subscribe_event throws exception for change event if they are not correctly configured
// Change in the polling thread: The event heartbeat has its own work in the work list
// Also add some event_unregister
// Fix order in which classes are destructed
// Fix bug in asynchronous mode (PUSH_CALLBACK). The callback thread ate all the CPU
// Change in the CORBA info call for the device type
//
// Revision 3.4.2.2  2004/09/27 09:07:48  taurel
// - Changes to allow reading state and/or status as attributes
//
// Revision 3.4.2.1  2004/09/15 06:44:43  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.4  2004/07/07 08:39:55  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.3  2003/08/21 07:21:24  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.2.2.7  2004/04/07 11:20:16  taurel
// - Add some import/export declaration for Windows DLL
//
// Revision 3.2.2.6  2004/04/02 14:53:17  taurel
// - Changes for ApiUtil::cleanup() method
//
// Revision 3.2.2.5  2004/03/19 15:21:32  taurel
// - Work on the ApiUtil destructor method which was not adapted to event.
//   I don't think it is fully operational as it is commited now
//
// Revision 3.2.2.4  2004/02/27 16:48:06  taurel
// - Updated DeviceImpl and Util classes sizes for compatibility
//
// Revision 3.2.2.3  2004/01/20 08:30:07  taurel
// -First commit after merge with the event branch and work on the AttributeProxy class
// - Fix bug in the stream "clear()" method usage when used with gcc 3.3
//
// Revision 3.2.2.2  2003/10/16 11:58:17  taurel
// - Fix some memory leaks in DeviceProxy::is_polled() and in
// Database::get_device_exported() methods
// - Add some bug fixes in Database::get_device_attribute_property(),
// Database::put_class_attribute_property()
//
// Revision 3.2.2.1  2003/09/18 14:07:41  taurel
// Fixes some bugs:
//  - Bug fix in DeviceProxy copy constructor and assignement operator
//  - Change the way how DeviceProxy::write_attribute() is coded
//  - Added DeviceAttribute ctors from "const char *"
//  - Split "str().c_str()" in two lines of code. It was the reason of some
//    problems using Windows VC6
//
// Revision 3.2  2003/05/28 14:42:55  taurel
// Add (conditionaly) autoconf generated include file
//
// Revision 3.1  2003/04/02 12:22:07  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
// Revision 3.0.2.3  2003/04/16 16:26:44  andy_gotz
// fixed some bugs in detect_and_push() for events with zero value; switched off KeepALive thread for demo on SuSE 8.1
//
// Revision 3.0.2.2  2003/04/15 19:01:52  andy_gotz
// added heartbeat on client and server side; cleaned up cout's
//
// Revision 3.0.2.1  2003/04/08 13:12:36  andy_gotz
// first version of TANGO events
//
// Revision 3.0  2003/03/25 16:30:40  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 2.5  2003/03/20 08:54:53  taurel
// Updated to support asynchronous calls
//
// Revision 2.4  2003/01/09 12:00:33  taurel
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and ApiUtil::~ApiUtil() methods
// - Replace some ORB * by ORB_ptr
// - Use CORBA::ORB::is_nil() instead of comparing to NULL
//
// Revision 2.3  2002/12/16 11:58:36  taurel
// - Change the underlying ORB fom ORBacus to omniORB
// - New method get_device_list() in Util class
// - Util::get_class_list() takes DServer device into account
// - Util::get_device_by_name() takes DSErver device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to ebnable CORBA to frre memory allocated for the attribute
//
// Revision 2.2  2002/10/14 09:32:43  taurel
// Fix bugs in devapi_base.cpp file :
// - In read_attribute and read_attributes method of the DeviceProxy class
//   Do not create sequence the same way if the call is local or remote.
// - Add reconnection in the Connection::set_timeout_millis method
// - Add flags to the Connection::set_timeout_millis method
// - Fix bug in the DeviceProxy constructor when device is not marked as exported
//   in the database. The constructor was not stateless in this case.
//
// Revision 2.1  2002/08/12 12:43:23  taurel
// Fix bug in DeviceProxy::write_attributes method when writing several
// attributes in one call. (File devapi_base.cpp)
//
// Revision 2.0  2002/06/28 13:43:07  taurel
// Lot of changes since last releases :
// 	- Database object managed as a singleton per control system
// 	- Support all tango device naming syntax (using TANGO_HOST env.
//  	  variable, without env variable and non database device)
// 	- No more copy during read_attribute and command_inout
// 	- Added some missing methods
// 	- Build an exception class hierarchy
// 	- Added correct management of device time-out
// 	- Support all Tango device interface release 2 features
// 	  (data/attribute comming from polling buffer, polling related methods,
// 	   command/attribute result history)
//
//
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
                                                    
namespace Tango
{

ApiUtil *ApiUtil::_instance = NULL;


//+----------------------------------------------------------------------------
//
// method : 		ApiUtil::ApiUtil()
// 
// description : 	Constructor of the ApiUtil class.
//
//-----------------------------------------------------------------------------

ApiUtil::ApiUtil()
{
	_orb = CORBA::ORB::_nil();
	
//
// Check if it is created from a device server
//

	if (Util::_constructed == true)
		in_serv = true;
	else
		in_serv = false;

//
// Create the extension class
//

	ext = new ApiUtilExt();
		
//
// Create the Asynchronous polling request Id generator
//

	UniqIdent *ui = new UniqIdent();
	
//
// Create the table used to store asynchronous polling requests
//

	asyn_p_table = new AsynReq(ui);
	
//
// Set the asynchronous callback mode to "ON_CALL"
//

	auto_cb = PULL_CALLBACK;
	cb_thread_ptr = NULL;
}

//+----------------------------------------------------------------------------
//
// method : 		ApiUtil::~ApiUtil()
// 
// description : 	Destructor of the ApiUtil class.
//
//-----------------------------------------------------------------------------

ApiUtil::~ApiUtil()
{
//
// Release Asyn stuff
//

	delete asyn_p_table;
	
	if (cb_thread_ptr != NULL)
	{
		cb_thread_cmd.stop_thread();
		cb_thread_ptr->join(0);
	}

	bool event_was_used = false;
	if (ext != NULL)
	{
		if (ext->event_consumer != NULL)
		{
			event_was_used = true;
			leavefunc();
			EventConsumer::cleanup();
		}
		delete ext;
	}

//
// Delete the database object
//

	for (unsigned int i = 0;i < db_vect.size();i++)
	{
		delete db_vect[i];
	}
				
//
// Properly shutdown the ORB
//

	if ((in_serv == false) && (CORBA::is_nil(_orb) == false))
	{
		if (event_was_used == false)
		{
			try
			{
				_orb->destroy();
			}
			catch (...)
			{
			}
		}
		CORBA::release(_orb);
	}
	

}

//-----------------------------------------------------------------------------
//
// ApiUtil::create_orb() - create a CORBA orb object
//
//-----------------------------------------------------------------------------

void ApiUtil::create_orb()
{
	int _argc;
	char **_argv;

//
// pass dummy arguments to init() because we don't have access to
// argc and argv
//

	_argc = 1;
	_argv = (char**)malloc(sizeof(char*));
	_argv[0] = (char*)"dummy";

//
// Init the ORB
//		

	const char *options[][2] = {
			{"clientCallTimeOutPeriod",CLNT_TIMEOUT_STR},
			{"giopMaxMsgSize",MAX_TRANSFER_SIZE},
			{0,0}};
				
	_orb = CORBA::ORB_init(_argc,_argv,"omniORB4",options);
	
	free(_argv);
}

//-----------------------------------------------------------------------------
//
// ApiUtil::get_db_ind() - Retrieve a Tango database object created from the
//			   TANGO_HOST environment variable
//
//-----------------------------------------------------------------------------

int ApiUtil::get_db_ind()
{
	for (unsigned int i = 0;i < db_vect.size();i++)
	{
		if (db_vect[i]->get_from_env_var() == true)
			return i;
	}
	
//
// The database object has not been found, create it
//

	db_vect.push_back(new Database());
	
	return (db_vect.size() - 1);
	
}

int ApiUtil::get_db_ind(string &host,int port)
{
	for (unsigned int i = 0;i < db_vect.size();i++)
	{
		if (db_vect[i]->get_db_port_num() == port)
		{
			if (db_vect[i]->get_db_host() == host)
				return i;
		}
	}
	
//
// The database object has not been found, create it
//

	db_vect.push_back(new Database(host,port));
	
	return (db_vect.size() - 1);
	
}


//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::get_asynch_replies()
// 
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. Fire callback for replies alaredy
//			arrived
//
//-----------------------------------------------------------------------------

void ApiUtil::get_asynch_replies()
{

//
// First get all replies from ORB buffers
//

	try
	{
		while (_orb->poll_next_response() == true)
		{
			CORBA::Request_ptr req;
			_orb->get_next_response(req);
				
//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

			TgRequest &tg_req = asyn_p_table->get_request(req);

			tg_req.arrived = true;
			asyn_p_table->mark_as_arrived(req);
			
//		
// Process request
//

			switch (tg_req.req_type)
			{
			case TgRequest::CMD_INOUT :
				tg_req.dev->Cb_Cmd_Request(req,tg_req.cb_ptr);
				break;
				
			case TgRequest::READ_ATTR :
				tg_req.dev->Cb_ReadAttr_Request(req,tg_req.cb_ptr);
				break;
				
			case TgRequest::WRITE_ATTR :
			case TgRequest::WRITE_ATTR_SINGLE :
				tg_req.dev->Cb_WriteAttr_Request(req,tg_req.cb_ptr);
				break;
			}
			
			tg_req.dev->dec_asynch_counter(CALL_BACK);
			asyn_p_table->remove_request(tg_req.dev,req);
		}				
	}
	catch (CORBA::BAD_INV_ORDER &e)
	{
		if (e.minor() != omni::BAD_INV_ORDER_RequestNotSentYet)
			throw;
	}
	
//
// For all replies already there
//

	multimap<Connection *,TgRequest>::iterator pos;
	multimap<Connection *,TgRequest> &req_table = asyn_p_table->get_cb_dev_table();

	for (pos = req_table.begin();pos != req_table.end();++pos)
	{
		if (pos->second.arrived == true)
		{
			switch (pos->second.req_type)
			{
			case TgRequest::CMD_INOUT :
				pos->first->Cb_Cmd_Request(pos->second.request,pos->second.cb_ptr);
				break;
				
			case TgRequest::READ_ATTR :
				pos->first->Cb_ReadAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;
				
			case TgRequest::WRITE_ATTR :
			case TgRequest::WRITE_ATTR_SINGLE :
				pos->first->Cb_WriteAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;
			}
			pos->first->dec_asynch_counter(CALL_BACK);
			asyn_p_table->remove_request(pos->first,pos->second.request);
		}
		
	}	
	
}


//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::get_asynch_replies()
// 
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. Fire callback for replies alaredy
//			arrived
//
// argin(s) :		call_timeout : The timeout vaalue in mS
//
//-----------------------------------------------------------------------------

void ApiUtil::get_asynch_replies(long call_timeout)
{
//
// For all replies already there
//

	multimap<Connection *,TgRequest>::iterator pos;
	multimap<Connection *,TgRequest> &req_table = asyn_p_table->get_cb_dev_table();

	for (pos = req_table.begin();pos != req_table.end();++pos)
	{
		if (pos->second.arrived == true)
		{
			switch (pos->second.req_type)
			{
			case TgRequest::CMD_INOUT :
				pos->first->Cb_Cmd_Request(pos->second.request,pos->second.cb_ptr);
				break;
				
			case TgRequest::READ_ATTR :
				pos->first->Cb_ReadAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;
				
			case TgRequest::WRITE_ATTR :
			case TgRequest::WRITE_ATTR_SINGLE :
				pos->first->Cb_WriteAttr_Request(pos->second.request,pos->second.cb_ptr);
				break;
			}
			pos->first->dec_asynch_counter(CALL_BACK);
			asyn_p_table->remove_request(pos->first,pos->second.request);
		}
		
	}
	
//
// If they are requests already sent but without being replied yet
//

	if (asyn_p_table->get_cb_request_nb() != 0)
	{	
		CORBA::Request_ptr req;
		
		if (call_timeout != 0)
		{
//
// A timeout has been specified. Wait if there are still request without
// replies but not more than the specified timeout. Leave method if the
// timeout is not arrived but there is no more request without reply
//

			long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
			struct timespec to_wait,inter;		
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 20000000;
#endif				
				
			while ((nb > 0) && (asyn_p_table->get_cb_request_nb() != 0))
			{
#ifdef _TG_WINDOWS_
				Sleep(20);
#else
				nanosleep(&to_wait,&inter);
#endif
				nb--;
			
				if (_orb->poll_next_response() == true)
				{
					_orb->get_next_response(req);
					
//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

					TgRequest &tg_req = asyn_p_table->get_request(req);

					tg_req.arrived = true;
					asyn_p_table->mark_as_arrived(req);
					
//		
// Is it a request for our device, process it ?
//

					switch (tg_req.req_type)
					{
					case TgRequest::CMD_INOUT :
						tg_req.dev->Cb_Cmd_Request(req,tg_req.cb_ptr);
						break;
				
					case TgRequest::READ_ATTR :
						tg_req.dev->Cb_ReadAttr_Request(req,tg_req.cb_ptr);
						break;
				
					case TgRequest::WRITE_ATTR :
					case TgRequest::WRITE_ATTR_SINGLE :
						tg_req.dev->Cb_WriteAttr_Request(req,tg_req.cb_ptr);
						break;
					}
			
					tg_req.dev->dec_asynch_counter(CALL_BACK);
					asyn_p_table->remove_request(tg_req.dev,req);
				}
			}
			
//
// Throw exception if the timeout has expired but there are still request
// without replies
//
			
			if ((nb == 0) && (asyn_p_table->get_cb_request_nb() != 0))
			{	
				TangoSys_OMemStream desc;
				desc << "Still some reply(ies) for asynchronous callback call(s) to be received" << ends;
				ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"ApiUtil::get_asynch_replies");		
			}	
		}
		else
		{
//
// If timeout is set to 0, this means wait until all the requests sent to this
// device has sent their replies
//

			while (asyn_p_table->get_cb_request_nb() != 0)
			{					
				_orb->get_next_response(req);
								
//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

				TgRequest &tg_req = asyn_p_table->get_request(req);

				tg_req.arrived = true;
				asyn_p_table->mark_as_arrived(req);
				
//		
// Process the reply
//

				switch (tg_req.req_type)
				{
				case TgRequest::CMD_INOUT :
					tg_req.dev->Cb_Cmd_Request(req,tg_req.cb_ptr);
					break;
				
				case TgRequest::READ_ATTR :
					tg_req.dev->Cb_ReadAttr_Request(req,tg_req.cb_ptr);
					break;
				
				case TgRequest::WRITE_ATTR :
				case TgRequest::WRITE_ATTR_SINGLE :
					tg_req.dev->Cb_WriteAttr_Request(req,tg_req.cb_ptr);
					break;
				}
			
				tg_req.dev->dec_asynch_counter(CALL_BACK);
				asyn_p_table->remove_request(tg_req.dev,req);
			}
		}
	}
			
}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::set_asynch_cb_sub_model()
// 
// description : 	Set the callback automatic mode (Fired by dedicated call
//			or automatically fired by a separate thread)
//
// argin(s) :		mode : The new callback mode
//
//-----------------------------------------------------------------------------

void ApiUtil::set_asynch_cb_sub_model(cb_sub_model mode)
{

	if (auto_cb == PULL_CALLBACK)
	{
		if (mode == PUSH_CALLBACK)
		{
		
//
// In this case, delete the old object in case it is needed, create a new 
// thread and start it
//

			if (cb_thread_ptr != NULL)
			{
				delete cb_thread_ptr;
				cb_thread_ptr = NULL;
			}
			cb_thread_cmd.start_thread();
			
			cb_thread_ptr = new CallBackThread(cb_thread_cmd,asyn_p_table);
			cb_thread_ptr->start();
			auto_cb = PUSH_CALLBACK;
		}
	}
	else
	{
		if (mode == PULL_CALLBACK)
		{
		
//
// Ask the thread to stop and to exit
//

			cb_thread_cmd.stop_thread();
			auto_cb = PULL_CALLBACK;
			asyn_p_table->signal();
		}
	}
	
}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::create_event_consumer()
// 
// description : 	Create the event consumer. This will automatically
//			start a new thread which is waiting in a CORBA::run()
//			indefintely for events. It will then trigger the events.
//
// argin(s) :		none
//
//-----------------------------------------------------------------------------

void ApiUtil::create_event_consumer()
{
	ext->event_consumer = EventConsumer::create();

        //cout << "ApiUtil::create_event_consumer(): start keep alive thread\n";
        //EventConsumerKeepAliveThread *keap_alive = new EventConsumerKeepAliveThread();

}

//-----------------------------------------------------------------------------
//
// method : 		ApiUtil::attr_to_device()
// 
// description : 	Create the event consumer. This will automatically
//			start a new thread which is waiting in a CORBA::run()
//			indefintely for events. It will then trigger the events.
//
// argin(s) :		none
//
//-----------------------------------------------------------------------------

void ApiUtil::attr_to_device(const AttributeValue *attr_value,
			     const AttributeValue_3 *attr_value_3,
			     long vers,DeviceAttribute *dev_attr)
{

	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	const DevVarFloatArray *tmp_seq_fl;
	CORBA::Float *tmp_fl;
	const DevVarBooleanArray *tmp_seq_boo;
	CORBA::Boolean *tmp_boo;
	const DevVarUShortArray *tmp_seq_ush;
	CORBA::UShort *tmp_ush;
	const DevVarCharArray *tmp_seq_uch;
	CORBA::Octet *tmp_uch;
	const DevVarLong64Array *tmp_seq_64;
	CORBA::LongLong *tmp_lolo;
	const DevVarULongArray *tmp_seq_ulo;
	CORBA::ULong *tmp_ulo;
	const DevVarULong64Array *tmp_seq_u64;
	CORBA::ULongLong *tmp_ulolo;
	const DevVarStateArray *tmp_seq_state;
	Tango::DevState *tmp_state;
		
	CORBA::ULong max,len;

	if (vers >= 3)
	{	
		dev_attr->name = attr_value_3->name;
		dev_attr->quality = attr_value_3->quality;
		dev_attr->time = attr_value_3->time;
		dev_attr->dim_x = attr_value_3->r_dim.dim_x;
		dev_attr->dim_y = attr_value_3->r_dim.dim_y;
		dev_attr->ext->w_dim_x = attr_value_3->w_dim.dim_x;
		dev_attr->ext->w_dim_y = attr_value_3->w_dim.dim_y;
		dev_attr->ext->err_list = new DevErrorList(attr_value_3->err_list);
	}
	else
	{
		dev_attr->name = attr_value->name;
		dev_attr->quality = attr_value->quality;
		dev_attr->time = attr_value->time;
		dev_attr->dim_x = attr_value->dim_x;
		dev_attr->dim_y = attr_value->dim_y;
	}
	
	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		CORBA::TypeCode_var ty;
		if (vers >= 3)
			ty = attr_value_3->value.type();
		else
			ty = attr_value->value.type();
			
		if (ty->kind() == CORBA::tk_enum)
		{
			attr_value_3->value >>= dev_attr->d_state;
			dev_attr->d_state_filled = true;
			return;
		}
			
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();			
		switch (ty_seq->kind())
		{
			case CORBA::tk_long:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_lo;
				else		
					attr_value->value >>= tmp_seq_lo;
				max = tmp_seq_lo->maximum();
				len = tmp_seq_lo->length();
				if (tmp_seq_lo->release() == true)
				{
					tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
					dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
				}
				else
				{
					tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
					dev_attr->LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
				}
				break;
	
			case CORBA::tk_longlong:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_64;
				else		
					attr_value->value >>= tmp_seq_64;
				max = tmp_seq_64->maximum();
				len = tmp_seq_64->length();
				if (tmp_seq_64->release() == true)
				{
					tmp_lolo = (const_cast<DevVarLong64Array *>(tmp_seq_64))->get_buffer((CORBA::Boolean)true);
					dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,true);
				}
				else
				{
					tmp_lolo = const_cast<CORBA::LongLong *>(tmp_seq_64->get_buffer());
					dev_attr->ext->Long64Seq = new DevVarLong64Array(max,len,tmp_lolo,false);
				}
				break;
				
			case CORBA::tk_short:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_sh;
				else
					attr_value->value >>= tmp_seq_sh;
				max = tmp_seq_sh->maximum();
				len = tmp_seq_sh->length();
				if (tmp_seq_sh->release() == true)
				{
					tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
					dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
				}
				else
				{
					tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
					dev_attr->ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
				}
				break;

			case CORBA::tk_double:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_db;
				else
					attr_value->value >>= tmp_seq_db;
				max = tmp_seq_db->maximum();
				len = tmp_seq_db->length();
				if (tmp_seq_db->release() == true)
				{
					tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
					dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
				}
				else
				{
					tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
					dev_attr->DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
				}
				break;
	
			case CORBA::tk_string:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_str;
				else
					attr_value->value >>= tmp_seq_str;
				max = tmp_seq_str->maximum();
				len = tmp_seq_str->length();
				if (tmp_seq_str->release() == true)
				{
					tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
					dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,true);
				}
				else
				{
					tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
					dev_attr->StringSeq = new DevVarStringArray(max,len,tmp_str,false);
				}
				break;

			case CORBA::tk_float:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_fl;
				else		
					attr_value->value >>= tmp_seq_fl;
				max = tmp_seq_fl->maximum();
				len = tmp_seq_fl->length();
				if (tmp_seq_fl->release() == true)
				{
					tmp_fl = (const_cast<DevVarFloatArray *>(tmp_seq_fl))->get_buffer((CORBA::Boolean)true);
					dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,true);
				}
				else
				{
					tmp_fl = const_cast<CORBA::Float *>(tmp_seq_fl->get_buffer());
					dev_attr->FloatSeq = new DevVarFloatArray(max,len,tmp_fl,false);
				}
				break;
			
			case CORBA::tk_boolean:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_boo;
				else		
					attr_value->value >>= tmp_seq_boo;
				max = tmp_seq_boo->maximum();
				len = tmp_seq_boo->length();
				if (tmp_seq_boo->release() == true)
				{
					tmp_boo = (const_cast<DevVarBooleanArray *>(tmp_seq_boo))->get_buffer((CORBA::Boolean)true);
					dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,true);
				}
				else
				{
					tmp_boo = const_cast<CORBA::Boolean *>(tmp_seq_boo->get_buffer());
					dev_attr->BooleanSeq = new DevVarBooleanArray(max,len,tmp_boo,false);
				}
				break;

			case CORBA::tk_ushort:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_ush;
				else		
					attr_value->value >>= tmp_seq_ush;
				max = tmp_seq_ush->maximum();
				len = tmp_seq_ush->length();
				if (tmp_seq_ush->release() == true)
				{
					tmp_ush = (const_cast<DevVarUShortArray *>(tmp_seq_ush))->get_buffer((CORBA::Boolean)true);
					dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,true);
				}
				else
				{
					tmp_ush = const_cast<CORBA::UShort *>(tmp_seq_ush->get_buffer());
					dev_attr->UShortSeq = new DevVarUShortArray(max,len,tmp_ush,false);
				}
				break;
			
			case CORBA::tk_octet:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_uch;
				else		
					attr_value->value >>= tmp_seq_uch;
				max = tmp_seq_uch->maximum();
				len = tmp_seq_uch->length();
				if (tmp_seq_uch->release() == true)
				{
					tmp_uch = (const_cast<DevVarCharArray *>(tmp_seq_uch))->get_buffer((CORBA::Boolean)true);
					dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,true);
				}
				else
				{
					tmp_uch = const_cast<CORBA::Octet *>(tmp_seq_uch->get_buffer());
					dev_attr->UCharSeq = new DevVarCharArray(max,len,tmp_uch,false);
				}
				break;
				
			case CORBA::tk_ulong:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_ulo;
				else		
					attr_value->value >>= tmp_seq_ulo;
				max = tmp_seq_ulo->maximum();
				len = tmp_seq_ulo->length();
				if (tmp_seq_ulo->release() == true)
				{
					tmp_ulo = (const_cast<DevVarULongArray *>(tmp_seq_ulo))->get_buffer((CORBA::Boolean)true);
					dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,true);
				}
				else
				{
					tmp_ulo = const_cast<CORBA::ULong *>(tmp_seq_ulo->get_buffer());
					dev_attr->ext->ULongSeq = new DevVarULongArray(max,len,tmp_ulo,false);
				}
				break;
				
			case CORBA::tk_ulonglong:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_u64;
				else		
					attr_value->value >>= tmp_seq_u64;
				max = tmp_seq_u64->maximum();
				len = tmp_seq_u64->length();
				if (tmp_seq_u64->release() == true)
				{
					tmp_ulolo = (const_cast<DevVarULong64Array *>(tmp_seq_u64))->get_buffer((CORBA::Boolean)true);
					dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,true);
				}
				else
				{
					tmp_ulolo = const_cast<CORBA::ULongLong *>(tmp_seq_u64->get_buffer());
					dev_attr->ext->ULong64Seq = new DevVarULong64Array(max,len,tmp_ulolo,false);
				}
				break;
				
			case CORBA::tk_enum:
				if (vers >= 3)
					attr_value_3->value >>= tmp_seq_state;
				else		
					attr_value->value >>= tmp_seq_state;
				max = tmp_seq_state->maximum();
				len = tmp_seq_state->length();
				if (tmp_seq_state->release() == true)
				{
					tmp_state = (const_cast<DevVarStateArray *>(tmp_seq_state))->get_buffer((CORBA::Boolean)true);
					dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,true);
				}
				else
				{
					tmp_state = const_cast<Tango::DevState *>(tmp_seq_state->get_buffer());
					dev_attr->ext->StateSeq = new DevVarStateArray(max,len,tmp_state,false);
				}
				break;
		}
	}	
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			AttributeInfo class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,AttributeInfo &p)
{


//
// Print all these properties
//
	
	o_str << "Attribute name = " << p.name << endl;
	o_str << "Attribute data_type = ";
	switch (p.data_type)
	{
	case Tango::DEV_SHORT :
		o_str << "Tango::DevShort" << endl;
		break;
		
	case Tango::DEV_LONG :
		o_str << "Tango::DevLong" << endl;
		break;
		
	case Tango::DEV_DOUBLE :
		o_str << "Tango::DevDouble" << endl;
		break;
		
	case Tango::DEV_STRING :
		o_str << "Tango::DevString" << endl;
		break;
		
	case Tango::DEV_FLOAT :
		o_str << "Tango::DevFloat" << endl;
		break;
		
	case Tango::DEV_USHORT :
		o_str << "Tango::DevUShort" << endl;
		break;
		
	case Tango::DEV_UCHAR :
		o_str << "Tango::DevUChar" << endl;
		break;
		
	case Tango::DEV_BOOLEAN :
		o_str << "Tango::DevBoolean" << endl;
		break;
		
	case Tango::DEV_STATE :
		o_str << "Tango::DevState" << endl;
		break;
	}
	
	o_str << "Attribute data_format = ";
	switch (p.data_format)
	{
	case Tango::SCALAR :
		o_str << "scalar" << endl;
		break;
		
	case Tango::SPECTRUM :
		o_str << "spectrum, max_dim_x = " << p.max_dim_x << endl;
		break;
		
	case Tango::IMAGE :
		o_str << "image, max_dim_x = " << p.max_dim_x << ", max_dim_y = " << p.max_dim_y << endl;
		break;
	}

	if (p.writable == static_cast<unsigned char>(true))
		o_str << "Attribute is writable" << endl;
	else
		o_str << "Attribute is not writable" << endl;
	o_str << "Attribute label = " << p.label << endl;
	o_str << "Attribute description = " << p.description << endl;
	o_str << "Attribute unit = " << p.unit;
	o_str << ", standard unit = " << p.standard_unit;
	o_str << ", display unit = " << p.display_unit << endl;
	o_str << "Attribute format = " << p.format << endl;
	o_str << "Attribute min alarm = " << p.min_alarm << endl;
	o_str << "Attribute max alarm = " << p.max_alarm << endl;
	o_str << "Attribute min value = " << p.min_value << endl;
	o_str << "Attribute max value = " << p.max_value << endl;	
	o_str << "Attribute writable_attr_name = " << p.writable_attr_name;
	
	return o_str;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	=
// 
// description : 	Assignement operator for the AttributeInfoEx class
//					from a AttributeConfig_2 pointer
//
//--------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(AttributeConfig_2 *att_2)
{
	name = att_2->name;
	writable = att_2->writable;
	data_format = att_2->data_format;
	data_type = att_2->data_type;
	max_dim_x = att_2->max_dim_x;
	max_dim_y = att_2->max_dim_y;
	description = att_2->description;
	label = att_2->label;
	unit = att_2->unit;
	standard_unit = att_2->standard_unit;
	display_unit = att_2->display_unit;
	format = att_2->format;
	min_value = att_2->min_value;
	max_value = att_2->max_value;
	min_alarm = att_2->min_alarm;
	max_alarm = att_2->max_alarm;
	writable_attr_name = att_2->writable_attr_name;
	extensions.resize(att_2->extensions.length());
	for (unsigned int j=0; j<att_2->extensions.length(); j++)
	{
		extensions[j] = att_2->extensions[j];
	}
	disp_level = att_2->level;
	
	return *this;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	=
// 
// description : 	Assignement operator for the AttributeInfoEx class
//					from a AttributeConfig_3 pointer
//
//--------------------------------------------------------------------------

AttributeInfoEx &AttributeInfoEx::operator=(AttributeConfig_3 *att_3)
{
	name = att_3->name;
	writable = att_3->writable;
	data_format = att_3->data_format;
	data_type = att_3->data_type;
	max_dim_x = att_3->max_dim_x;
	max_dim_y = att_3->max_dim_y;
	description = att_3->description;
	label = att_3->label;
	unit = att_3->unit;
	standard_unit = att_3->standard_unit;
	display_unit = att_3->display_unit;
	format = att_3->format;
	min_value = att_3->min_value;
	max_value = att_3->max_value;
	min_alarm = att_3->att_alarm.min_alarm;
	max_alarm = att_3->att_alarm.max_alarm;
	writable_attr_name = att_3->writable_attr_name;
	extensions.resize(att_3->sys_extensions.length());
	for (unsigned int j=0; j<att_3->sys_extensions.length(); j++)
	{
		extensions[j] = att_3->sys_extensions[j];
	}
	disp_level = att_3->level;
	
	alarms.min_alarm = att_3->att_alarm.min_alarm;
	alarms.max_alarm = att_3->att_alarm.max_alarm;
	alarms.min_warning = att_3->att_alarm.min_warning;
	alarms.max_warning = att_3->att_alarm.max_warning;
	alarms.delta_t = att_3->att_alarm.delta_t;
	alarms.delta_val = att_3->att_alarm.delta_val;
	alarms.extensions.resize(att_3->att_alarm.extensions.length());
	for (unsigned int j=0; j<att_3->att_alarm.extensions.length(); j++)
	{
		alarms.extensions[j] = att_3->att_alarm.extensions[j];
	}

	events.ch_event.abs_change = att_3->event_prop.ch_event.abs_change;		
	events.ch_event.rel_change = att_3->event_prop.ch_event.rel_change;
	events.ch_event.extensions.resize(att_3->event_prop.ch_event.extensions.length());
	for (unsigned int j=0; j<att_3->event_prop.ch_event.extensions.length(); j++)
	{
		events.ch_event.extensions[j] = att_3->event_prop.ch_event.extensions[j];
	}

	events.per_event.period = att_3->event_prop.per_event.period;		
	events.per_event.extensions.resize(att_3->event_prop.per_event.extensions.length());
	for (unsigned int j=0; j<att_3->event_prop.per_event.extensions.length(); j++)
	{
		events.per_event.extensions[j] = att_3->event_prop.per_event.extensions[j];
	}
	
	events.arch_event.archive_abs_change = att_3->event_prop.arch_event.abs_change;		
	events.arch_event.archive_rel_change = att_3->event_prop.arch_event.rel_change;
	events.arch_event.archive_period = att_3->event_prop.arch_event.period;
	events.arch_event.extensions.resize(att_3->event_prop.arch_event.extensions.length());
	for (unsigned int j=0; j<att_3->event_prop.arch_event.extensions.length(); j++)
	{
		events.arch_event.extensions[j] = att_3->event_prop.arch_event.extensions[j];
	}
		
	return *this;
}

//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
// 
// description : 	Friend function to ease printing instance of the
//			AttributeInfo class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,AttributeInfoEx &p)
{


//
// Print all these properties
//
	
	o_str << "Attribute name = " << p.name << endl;
	o_str << "Attribute data_type = ";
	switch (p.data_type)
	{
	case Tango::DEV_SHORT :
		o_str << "Tango::DevShort" << endl;
		break;
		
	case Tango::DEV_LONG :
		o_str << "Tango::DevLong" << endl;
		break;
		
	case Tango::DEV_DOUBLE :
		o_str << "Tango::DevDouble" << endl;
		break;
		
	case Tango::DEV_STRING :
		o_str << "Tango::DevString" << endl;
		break;
		
	case Tango::DEV_FLOAT :
		o_str << "Tango::DevFloat" << endl;
		break;
		
	case Tango::DEV_USHORT :
		o_str << "Tango::DevUShort" << endl;
		break;
		
	case Tango::DEV_UCHAR :
		o_str << "Tango::DevUChar" << endl;
		break;
		
	case Tango::DEV_BOOLEAN :
		o_str << "Tango::DevBoolean" << endl;
		break;
		
	case Tango::DEV_STATE :
		o_str << "Tango::DevState" << endl;
		break;
	}
	
	o_str << "Attribute data_format = ";
	switch (p.data_format)
	{
	case Tango::SCALAR :
		o_str << "scalar" << endl;
		break;
		
	case Tango::SPECTRUM :
		o_str << "spectrum, max_dim_x = " << p.max_dim_x << endl;
		break;
		
	case Tango::IMAGE :
		o_str << "image, max_dim_x = " << p.max_dim_x << ", max_dim_y = " << p.max_dim_y << endl;
		break;
	}

	if (p.writable == static_cast<unsigned char>(true))
		o_str << "Attribute is writable" << endl;
	else
		o_str << "Attribute is not writable" << endl;
	o_str << "Attribute label = " << p.label << endl;
	o_str << "Attribute description = " << p.description << endl;
	o_str << "Attribute unit = " << p.unit;
	o_str << ", standard unit = " << p.standard_unit;
	o_str << ", display unit = " << p.display_unit << endl;
	o_str << "Attribute format = " << p.format << endl;
	o_str << "Attribute min value = " << p.min_value << endl;
	o_str << "Attribute max value = " << p.max_value << endl;	
	o_str << "Attribute writable_attr_name = " << p.writable_attr_name << endl;

	unsigned int i;	
	for (i = 0;i < p.extensions.size();i++)
		o_str << "Attribute extensions " << i + 1 << " = " << p.extensions[i] << endl;

	o_str << "Attribute alarm : min alarm = ";
	p.alarms.min_alarm.empty() == true ? o_str << "Not specified" : o_str << p.alarms.min_alarm;
	o_str << endl;
		
	o_str << "Attribute alarm : max alarm = ";
	p.alarms.max_alarm.empty() == true ? o_str << "Not specified" : o_str << p.alarms.max_alarm;
	o_str << endl;
		
	o_str << "Attribute warning alarm : min warning = ";
	p.alarms.min_warning.empty() == true ? o_str << "Not specified" : o_str << p.alarms.min_warning;
	o_str << endl;	

	o_str << "Attribute warning alarm : max warning = ";
	p.alarms.max_warning.empty() == true ? o_str << "Not specified" : o_str << p.alarms.max_warning;
	o_str << endl;		

	o_str << "Attribute rds alarm : delta time = ";
	p.alarms.delta_t.empty() == true ? o_str << "Not specified" : o_str << p.alarms.delta_t;
	o_str << endl;
		
	o_str << "Attribute rds alarm : delta value = ";
	p.alarms.delta_val.empty() == true ? o_str << "Not specified" : o_str << p.alarms.delta_val;
	o_str << endl;
		
	for (i = 0;i < p.alarms.extensions.size();i++)
		o_str << "Attribute alarm extensions " << i + 1 << " = " << p.alarms.extensions[i] << endl;

	o_str << "Attribute event : change event absolute change = ";
	p.events.ch_event.abs_change.empty() == true ? o_str << "Not specified" : o_str << p.events.ch_event.abs_change;
	o_str << endl;
		
	o_str << "Attribute event : change event relative change = ";
	p.events.ch_event.rel_change.empty() == true ? o_str << "Not specified" : o_str << p.events.ch_event.rel_change;
	o_str << endl;
		
	for (i = 0;i < p.events.ch_event.extensions.size();i++)
		o_str << "Attribute alarm : change event extensions " << i + 1 << " = " << p.events.ch_event.extensions[i] << endl;

	o_str << "Attribute event : periodic event period = ";
	p.events.per_event.period.empty() == true ? o_str << "Not specified" : o_str << p.events.per_event.period;
	o_str << endl;
		
	for (i = 0;i < p.events.per_event.extensions.size();i++)
		o_str << "Attribute alarm : periodic event extensions " << i + 1 << " = " << p.events.per_event.extensions[i] << endl;
					
	o_str << "Attribute event : archive event absolute change = ";
	p.events.arch_event.archive_abs_change.empty() == true ? o_str << "Not specified" : o_str << p.events.arch_event.archive_abs_change;
	o_str << endl;
		
	o_str << "Attribute event : archive event relative change = ";
	p.events.arch_event.archive_rel_change.empty() == true ? o_str << "Not specified" : o_str << p.events.arch_event.archive_rel_change;
	o_str << endl;
		
	o_str << "Attribute event : archive event period = ";
	p.events.arch_event.archive_period.empty() == true ? o_str << "Not specified" : o_str << p.events.arch_event.archive_period;
	o_str << endl;
		
	for (i = 0;i < p.events.arch_event.extensions.size();i++)
	{
		if (i == 0)
			o_str << endl;
		o_str << "Attribute alarm : archive event extensions " << i + 1 << " = " << p.events.arch_event.extensions[i];
		if (i != p.events.arch_event.extensions.size() - 1)
			o_str << endl;
	}
	
	return o_str;
}
	
} // End of tango namespace

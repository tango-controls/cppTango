static const char *RcsId = "$Id$\n$Name$";

//
// cpp 	- C++ source code file for TANGO api class ApiUtil
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- May 2002
//
// $Log$
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
// Create the Asynchronous poling request Id generator
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
#ifndef WIN32
			struct timespec to_wait,inter;		
			to_wait.tv_sec = 0;
			to_wait.tv_nsec = 20000000;
#endif				
				
			while ((nb > 0) && (asyn_p_table->get_cb_request_nb() != 0))
			{
#ifdef WIN32
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
	
} // End of tango namespace

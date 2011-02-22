static const char *RcsId = "$Id$\n$Name$";

//
// cpp 	- C++ source code file for TANGO api Connection and DeviceProxy
//	  classes (asynchronous polling related methods)
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- August 2002
//
// Copyright (C) :      2002,2003,2004,2005,2006,2007,2008,2009,2010
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
// $Log$
// Revision 3.29  2010/06/25 07:16:34  taurel
// - Also protect the asynchronous DeviceProxy::read_attributes() methods
// against multiple times the same attribute in att name list
//
// Revision 3.28  2010/04/27 07:38:03  taurel
// - Merge with the bugfixes branch
//
// Revision 3.27  2009/12/18 14:51:01  taurel
// - Safety commit before christmas holydays
// - Many changes to make the DeviceProxy, Database and AttributeProxy
// classes thread safe (good help from the helgrind tool from valgrind)
//
// Revision 3.26  2009/09/01 14:02:50  taurel
// - Still some HP-UX code !!
//
// Revision 3.25  2009/03/27 13:05:10  taurel
// - Fix bug due to new Attribute format data member in AttributeValue4
// structure
//
// Revision 3.24  2009/03/27 12:19:33  taurel
// - Fix bug in read_attribute for IDL 4
//
// Revision 3.23  2009/03/13 09:32:27  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.22  2009/02/19 08:48:52  taurel
// - Add cancel asynchronous request calls to the DeviceProxy class
//
// Revision 3.21  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.20  2008/12/17 09:54:44  taurel
// - First implementation of attributes sent on the wire using IDL Union
// instead of IDL Any
//
// Revision 3.19  2008/10/13 15:04:37  taurel
// - Fix bug in command_inout_reply(). SourceForge bug nb 2157328
//
// Revision 3.18  2008/10/06 15:02:17  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.17  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.16  2008/09/23 14:38:29  taurel
// - Commit after the end of DevEncoded data type implementation
// - The new test suite is also now running fine
//
// Revision 3.15  2008/09/01 14:25:47  taurel
// - Fix some bugs in locking implementation
// - The Tango::Callback class dtor is now defined as virtual as it should be
//
// Revision 3.14  2008/06/14 11:28:07  taurel
// - DevEncoded attribute data type implementation work going on
//
// Revision 3.13  2008/05/20 12:42:30  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.12  2008/03/25 15:47:25  taurel
// - Fix some warnings for gcc 4.2 (Thank's to F Picca from Soleil)
// Revision 3.10.2.2  2008/05/20 06:14:19  taurel
// - Last commit before merge with trunk
//
// Revision 3.11  2008/03/11 14:36:45  taurel
// - Apply patches from Frederic Picca about compilation with gcc 4.2
// Revision 3.10.2.1  2008/02/07 15:56:58  taurel
// - First implementation of the Controlled Access done
//
// Revision 3.10  2007/04/20 14:38:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.9  2007/04/16 14:55:16  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.8  2007/02/14 14:12:17  taurel
// - Fix bug in asynchronous calls reconnection
//
// Revision 3.7  2005/01/13 08:36:37  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.6.2.1  2004/09/15 06:44:43  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.6  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.5  2003/08/21 07:22:02  taurel
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
// Revision 3.4  2003/07/03 07:37:56  taurel
// - Change in Tango IDL file : Implement a new way to tranfer data for read_attribute and write_attribute CORBA operation
// - Handle this new IDL release in DeviceProxy class
// - New exception methods in DeviceAttribute class
// - New way to get data out of DeviceAttribute object
// - Fix bugs in DeviceProxy copy constructor and assignement operator
// - Change some method names in DeviceDataHistory and DeviceAttributeHistory classes
// - Change the implementation of the DeviceProxy::write_attribute() method to avoid DeviceAttribute copying
// - Clean-up how a server is killed via a CTRL-C or a dserver device kill command
// - Add a server_cleanup() method in the Util class
// - Win32 : Update debug menu in the server graphical window to support logging feature
// - Win32 : Display library CVS tag in the "Help->About" sub-window
//
// Revision 3.3.2.1  2003/12/10 16:07:39  taurel
// Last commit before merging with the event branch.
//
// Revision 3.3  2003/05/28 14:42:56  taurel
// Add (conditionaly) autoconf generated include file
//
// Revision 3.2  2003/05/16 08:48:02  taurel
// Many changes for release 3.0.1. The most important ones are :
// - Timeout are backs
// - Multiple db servers (change in TANGO_HOST syntax)
// - Added methods to print DeviceData, DeviceDataHistory, DeviceAttribute and DeviceAttributeHistory instances
// - Attributes name stored in blackbox
// - Remove check if a class is created without any device
// - It's now possible to create a DeviceProxy from its alias name
// - Command, attribute names are case insensitive
// - Change parameters of some DeviceProxy logging methods
// - Change parameters of DeviceProxy asynchronous replies calls
// - New serialization model in device server (no serialization model)
// - Win32 (2000) device server service does not exit at loggoff anymore
// - Miscellaneous bug fixes
//
// Revision 3.1  2003/04/02 12:22:07  taurel
// Miscellaneous changes to :
//  - Modify TANGO_HOST env. variable syntax to be able to have several db servers
//  - Asynchronous calls are now done on device_2 or device depending on the device IDL version
//  - Timeout are bcks (omniORB 4.0.1)
//  - Fix bug when killing a device server via the kill command of the admin device
//    (Solaris only)
//  - Cleanup device server code to have only one ORB and one db connection within a server
//
// Revision 3.0  2003/03/25 16:30:37  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 1.3  2003/03/20 16:09:17  taurel
// Use device instead of device_2 fields of the DeviceProxy class for
// asynchronous calls (still for Java device)
//
// Revision 1.2  2003/03/20 15:02:31  taurel
// Remove test to refuse asynchronous calls to device supporting only
// IDL relase 1 (Java device)
//
// Revision 1.1  2003/03/20 08:56:12  taurel
// New file to support asynchronous calls
//
//

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif


#include <tango.h>

#define _dyn_attr
#ifdef _TG_WINDOWS_
	#include <sys/timeb.h>
#else
	#include <sys/time.h>
#endif 
                                                    
namespace Tango
{

//-----------------------------------------------------------------------------
//
// method : 		Connection::command_inout_asyn()
// 
// description : 	Send a command to a Tango device asynchrnously.
//			The client is not blocked until the command is executed
//
// argin(s) :		command : The command name
//			data_in : Command input data
//			faf : Fire And Forget flag
//
// return :		The asynchronous call identifier
// 
//-----------------------------------------------------------------------------


long Connection::command_inout_asynch(const char *command, DeviceData &data_in, bool faf)
{
//
// Throw exception if caller not allowed to write_attribute
//
 
	if (access == ACCESS_READ)
	{
		int db_num;
		ApiUtil *au = ApiUtil::instance();
		
		if (get_from_env_var() == true)
			db_num = au->get_db_ind();
		else
			db_num = au->get_db_ind(get_db_host(),get_db_port_num());

		vector<Database *> & v_d = au->get_db_vect();
		Database *db = v_d[db_num];

//
// If the command is not allowed, throw exception
// Also throw exception if it was not possible to get the list
// of allowed commands from the control access service
//
	
		string d_name = dev_name();
		string cmd(command);
		
		if (db->is_command_allowed(d_name,cmd) == false)
		{
			DevErrorList &e = db->get_access_except_errors();
			if (e.length() != 0)
			{
				DevFailed df(e);
				throw df;
			}
			
			TangoSys_OMemStream desc;
			desc << "Command_inout_asynch on device " << dev_name() << " for command ";
			desc << command << " is not authorized" << ends;
	
			NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
									  	  (const char *)"Connection::command_inout_asynch()");
		}
	}
		
//
// Reconnect to device in case it is needed
//

	try
	{
		check_and_reconnect();
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"Connection::command_inout_asynch()");
	}
			
//
// Create the request object
//

	CORBA::Request_ptr request;
	
	if (version >= 4)
		request = ext->device_4->_request("command_inout_4");
	else if (version >=2)
		request = device_2->_request("command_inout_2");
	else	
		request = device->_request("command_inout");
	
	request->add_in_arg() <<= command;
	request->add_in_arg() <<= data_in.any.in();
	
	if (version >= 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		request->add_in_arg() <<= source;
		request->add_in_arg() <<= ci;
	}
	else if (version >= 2)
		request->add_in_arg() <<= source;
	
	request->set_return_type(CORBA::_tc_any);
	request->exceptions()->add(Tango::_tc_DevFailed);

//
// If its a fire and forget (faf) call, send it and forget. Otherwise, send it
// and store the request in the global asynchronous polling requests table
//
	
	long id = 0;

	if (faf == false)
	{
		id = add_asyn_request(request,TgRequest::CMD_INOUT);
		request->send_deferred();
	}
	else
	{
		request->send_oneway();
		CORBA::release(request);
	}
	
	return id;
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::command_inout_asyn()
// 
// description : 	Send a command to a Tango device asynchrnously.
//			The client is not blocked until the command is executed
//			These methods are only user helper methods which call
//			the official command_inout_asyn() method
//
// argin(s) :		command : The command name
//			data_in : Command input data
//			faf : Fire And Forget flag
//
// return :		The asynchronous call identifier
// 
//-----------------------------------------------------------------------------


long Connection::command_inout_asynch(string &command, DeviceData &data_in, bool faf)
{
	return command_inout_asynch(command.c_str(),data_in,faf);
}

long Connection::command_inout_asynch(const char *command,bool faf)
{
	DeviceData data_in;
	return command_inout_asynch(command,data_in,faf);
}

long Connection::command_inout_asynch(string &command,bool faf)
{
	DeviceData data_in;
	return command_inout_asynch(command.c_str(),data_in,faf);
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::command_inout_reply()
// 
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//
// argin(s) :		id : The asynchrnous call identifier
//
// return :		The command output data
// 
//-----------------------------------------------------------------------------

DeviceData Connection::command_inout_reply(long id)
{

	DeviceData data_out;
//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if (req.req_type != TgRequest::CMD_INOUT)
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::command_inout_reply");
	}
	
//
// Reply arrived ? Throw exception if not yet arrived
//

	if (req.request->poll_response() == false)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << dev_name();
		desc << ": Reply for asynchronous call (id = " << id;
		desc << ") is not yet arrived" << ends;
		ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       desc.str(),
						       (const char *)"Connection::command_inout_reply");
	}

//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req.request->env();
	if (!CORBA::is_nil(env) && (env->exception() == NULL))
	{
	
//
// Get received value
//

		const CORBA::Any *received;
		CORBA::Any &dii_any = req.request->return_value();
		dii_any >>= received;
		CORBA::Any *server_any = new CORBA::Any(*received);
		
		data_out.any = server_any;			
	}
	else
	{
	
//
// Retrieve exception and re-throw it.
//
	
		CORBA::Exception *ex_ptr = env->exception();

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
		
		CORBA::TRANSIENT *tra;
		if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
		{
			if (tra->minor() == omni::TRANSIENT_CallTimedout)
			{		
				CORBA::NVList_ptr req_arg = req.request->arguments();
				const char *cmd;
				CORBA::NamedValue_ptr nv = req_arg->item(0);
				*(nv->value()) >>= cmd;
				char *tmp = CORBA::string_dup(cmd);

				char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);

				TangoSys_OMemStream desc;
				desc << "Timeout (" << timeout << " mS) exceeded on device " << dev_name();
				desc << ", command " << tmp << ends;
				CORBA::string_free(tmp);
		
				remove_asyn_request(id);	

				ApiCommExcept::re_throw_exception(cb_excep_mess,
					  (const char *)"API_DeviceTimedOut",
					  desc.str(),
					  (const char *)"Connection::command_inout_reply()");
			}
		}
		
		
		CORBA::UnknownUserException *unk_ex;
		if ((unk_ex = CORBA::UnknownUserException::_downcast(ex_ptr)) != NULL)
		{
		
//
// It is a UserUnknownException exception. This means that the
// server has sent a DevFailed exception
//

			const Tango::DevFailed *serv_ex;
			unk_ex->exception() >>= serv_ex;
			Tango::DevFailed ex(*serv_ex);

			CORBA::NVList_ptr req_arg = req.request->arguments();
			const char *cmd;
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			*(nv->value()) >>= cmd;
			char *tmp = CORBA::string_dup(cmd);

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout_asynch on device " << dev_name();
			desc << ", command " << tmp << ends;
			CORBA::string_free(tmp);
		
			remove_asyn_request(id);
		
			Except::re_throw_exception(ex,
					   (const char*)"API_CommandFailed",
					   desc.str(),
					   (const char*)"Connection::command_inout_reply()");
				

		}

		CORBA::SystemException *sys_ex;
		if ((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL)
		{

			set_connection_state(CONNECTION_NOTOK);
		
//
// Re-throw nearly all CORBA system exceptions
//

			CORBA::NVList_ptr req_arg = req.request->arguments();
			const char *cmd;
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			*(nv->value()) >>= cmd;
			char *tmp = CORBA::string_dup(cmd);

			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);
			
//
// Check if the exception was a connection exception
// In this case, try to ping the device.
// If successfull, just returns otherwise, throw the first exception
//

			string ex(cb_excep_mess);
			string::size_type pos = ex.find("TRANSIENT_ConnectFailed");
			if (pos != string::npos)
			{
				try
				{
					DeviceProxy *dev_ptr = static_cast<DeviceProxy *>(this);
					dev_ptr->ping();

//
// If we arrive here, this means we have to do a re-try with a synchronous
// request
//

					DeviceData dd_out = redo_synch_cmd(req);

//
// Remove request from request global table.
//

					CORBA::string_free(tmp);

					remove_asyn_request(id);
				
					return dd_out;
					
				}
				catch (Tango::DevFailed &) {}
			}

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout_asynch on device " << dev_name();
			desc << ", command " << tmp << ends;
			CORBA::string_free(tmp);
		
			remove_asyn_request(id);
		
			ApiCommExcept::re_throw_exception(cb_excep_mess,
						  (const char*)"API_CommunicationFailed",
						  desc.str(),
						  (const char*)"Connection::command_inout_reply()");
				

		}		
	}
			
//
// Remove request from request global table.
//

	remove_asyn_request(id);
				
	return data_out;
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::command_inout_reply()
// 
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method blocks if the reply is
//			not yet arrived for a max of timeout mS. 
//			An exception is thrown in case of reply not yet arrived.
//
// argin(s) :		call_timeout : The max time to block the calling process
//			if timeout is 0, this means wait until the reply is
//			there
//			id : The asynchrnous call identifier
//
// return :		The command output data
// 
//-----------------------------------------------------------------------------

DeviceData Connection::command_inout_reply(long id,long call_timeout)
{
//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if (req.req_type != TgRequest::CMD_INOUT)
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::command_inout_reply");
	}
		
//
// Use CORBA get_response call if the call timeout is specified as 0
// (If the response is not already there).
// Otherwise, uses a loop. Switch process to sleeping state for 20 ms
// between each test to check if the replies has arrived.
//

	if (call_timeout == 0)
	{
		if (req.request->poll_response() == false)
		{
			req.request->get_response();
		}
	}
	else
	{
		long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef _TG_WINDOWS_
			Sleep(20);
#else
			nanosleep(&to_wait,&inter);
#endif
			if (req.request->poll_response() == true)
			{
				break;
			}	
		}
		
		if (i == nb)
		{
			TangoSys_OMemStream desc;
			desc << "Device " << dev_name();
			desc << ": Reply for asynchronous call (id = " << id;
			desc << ") is not yet arrived" << ends;
			ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"Connection::command_inout_reply");		
		}
	}
		
//
// Check if the reply is an exception
//

	DeviceData data_out;
	CORBA::Environment_ptr env = req.request->env();
	if (!CORBA::is_nil(env) && (env->exception() == NULL))
	{
		
//
// IT's not an exception, therefore get received value
//

		const CORBA::Any *received;
		CORBA::Any &dii_any = req.request->return_value();
		dii_any >>= received;
		CORBA::Any *server_any = new CORBA::Any(*received);
			
		data_out.any = server_any;			
	}
	else
	{
		
//
// Retrieve exception and re-throw it.
//

		CORBA::Exception *ex_ptr = env->exception();

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
		
		CORBA::TRANSIENT *tra;
		if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
		{
			if (tra->minor() == omni::TRANSIENT_CallTimedout)
			{		
				CORBA::NVList_ptr req_arg = req.request->arguments();
				const char *cmd;
				CORBA::NamedValue_ptr nv = req_arg->item(0);
				*(nv->value()) >>= cmd;
				char *tmp = CORBA::string_dup(cmd);

				char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
				TangoSys_OMemStream desc;
				desc << "Timeout (" << timeout << " mS) exceeded on device " << dev_name();
				desc << ", command " << tmp << ends;
				CORBA::string_free(tmp);
				
				remove_asyn_request(id);
					
				ApiCommExcept::re_throw_exception(cb_excep_mess,
						  (const char *)"API_DeviceTimedOut",
						  desc.str(),
						  (const char *)"Connection::command_inout_reply()");
			}
		}
			
		CORBA::UnknownUserException *unk_ex;
		if ((unk_ex = CORBA::UnknownUserException::_downcast(ex_ptr)) != NULL)
		{
		
//
// It is a UserUnknownException exception. This means that the
// server has sent a DevFailed exception
//

			const Tango::DevFailed *serv_ex;
			unk_ex->exception() >>= serv_ex;
			Tango::DevFailed ex(*serv_ex);

			CORBA::NVList_ptr req_arg = req.request->arguments();
			const char *cmd;
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			*(nv->value()) >>= cmd;
			char *tmp = CORBA::string_dup(cmd);

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout_asynch on device " << dev_name();
			desc << ", command " << tmp << ends;
			CORBA::string_free(tmp);
			
			remove_asyn_request(id);;
			
			Except::re_throw_exception(ex,
						   (const char*)"API_CommandFailed",
                        			   desc.str(),
						   (const char*)"Connection::command_inout_reply()");
					
	
		}

		CORBA::SystemException *sys_ex;
		if ((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL)
		{

			set_connection_state(CONNECTION_NOTOK);
			
//
// Re-throw all CORBA system exceptions
//

			CORBA::NVList_ptr req_arg = req.request->arguments();
			const char *cmd;
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			*(nv->value()) >>= cmd;
			char *tmp = CORBA::string_dup(cmd);

			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout_asynch on device " << dev_name();
			desc << ", command " << tmp << ends;
			CORBA::string_free(tmp);
			
			remove_asyn_request(id);
			
			ApiCommExcept::re_throw_exception(cb_excep_mess,
						          (const char*)"API_CommunicationFailed",
                        			          desc.str(),
						          (const char*)"Connection::command_inout_reply()");
					
	
		}		
	}
				
//
// Remove request from request global table.
//

	remove_asyn_request(id);
				
	return data_out;
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attributes_asynch()
// 
// description : 	Read Tango device attributes asynchrnously.
//			The client is not blocked until the attributes are read
//
// argin(s) :		attr_names : The attribute name(s)
//
// return :		The asynchronous call identifier
// 
//-----------------------------------------------------------------------------


long DeviceProxy::read_attributes_asynch(vector<string> &attr_names)
{
//
// Reconnect to device in case it is needed
//

	try
	{
		check_and_reconnect();
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute read_attributes_asynch on device " << dev_name() << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"DeviceProxy::read_attributes_asynch()");
	}

//
// Check that the caller did not give two times the same attribute
//

	same_att_name(attr_names,"DeviceProxy::read_attributes_asynch");
	
//
// Create the request object
//

	Tango::DevVarStringArray names;
	long nb_names = attr_names.size();
	names.length(nb_names);
	for (int i = 0;i < nb_names;i++)
		names[i] = attr_names[i].c_str();
		
	CORBA::Request_ptr request;
	if (version == 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());		
		request = Connection::ext->device_4->_request("read_attributes_4");
		request->add_in_arg() <<= names;
		request->add_in_arg() <<= source;
		request->add_in_arg() <<= ci;
		request->set_return_type(Tango::_tc_AttributeValueList_4);		
	}
	else if (version == 3)
	{
		request = Connection::ext->device_3->_request("read_attributes_3");
		request->add_in_arg() <<= names;
		request->add_in_arg() <<= source;
		request->set_return_type(Tango::_tc_AttributeValueList_3);
	}
	else if (version == 2)
	{
		request = device_2->_request("read_attributes_2");
		request->add_in_arg() <<= names;
		request->add_in_arg() <<= source;
		request->set_return_type(Tango::_tc_AttributeValueList);
	}
	else
	{
		request = device->_request("read_attributes");
		request->add_in_arg() <<= names;
		request->set_return_type(Tango::_tc_AttributeValueList);
	}
	request->exceptions()->add(Tango::_tc_DevFailed);

//
// Send it and store the request in the global asynchronous polling requests
// table
//
	
	long id = 0;
	
	id = add_asyn_request(request,TgRequest::READ_ATTR);
	request->send_deferred();
	
	return id;
}

long DeviceProxy::read_attribute_asynch(string &name)
{
	vector<string> tmp_names(1,name);
	return read_attributes_asynch(tmp_names);
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attributes_reply()
// 
// description : 	Try to obtain data returned by attributes asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//
// argin(s) :		id : The asynchrnous call identifier
//
// return :		The attributes data
// 
//-----------------------------------------------------------------------------

vector<DeviceAttribute> *DeviceProxy::read_attributes_reply(long id)
{
//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if (req.req_type != TgRequest::READ_ATTR)
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::read_attributes_reply");
	}
	
//
// Reply arrived ? Throw exception is not yet arrived
//

	if (req.request->poll_response() == false)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << dev_name();
		desc << ": Reply for asynchronous call (id = " << id;
		desc << ") is not yet arrived" << ends;
		ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       desc.str(),
						       (const char *)"DeviceProxy::read_attributes_reply");
	}
	else
	{
		
//
// Check if the reply is an exception
//

		CORBA::Environment_ptr env = req.request->env();
		if (!CORBA::is_nil(env) && (env->exception() != NULL))
		{
			read_attr_except(req.request,id,MULTIPLE);
			
//
// If we arrive here, this means the exception was there due to a server
// shutdown but it is now back into operation
// Try to redo the call synchronously
//

			vector<DeviceAttribute> *a_ptr;
			a_ptr = redo_synch_reads_call(req);
			
//
// Remove request from request global table.
//

			remove_asyn_request(id);
		
			return a_ptr;
		}
		
		vector<DeviceAttribute> *dev_attr = new(vector<DeviceAttribute>);	
		
//
// Get received value
//

		const Tango::AttributeValueList *received;
		const Tango::AttributeValueList_3 *received_3;
		const Tango::AttributeValueList_4 *received_4;
		CORBA::Any &dii_any = req.request->return_value();
		unsigned long nb_received;

		if (version < 3)
		{
			dii_any >>= received;
			nb_received = received->length();
		}
		else if (version == 3)
		{
			dii_any >>= received_3;
			nb_received = received_3->length();
		}
		else
		{
			dii_any >>= received_4;
			nb_received = received_4->length();
		}
		dev_attr->resize(nb_received);
				
		for (unsigned long i=0; i < nb_received; i++)
		{
			if (version >= 3)
			{	
				if (version == 3)
					ApiUtil::attr_to_device(NULL,&((*received_3)[i]),version,&((*dev_attr)[i]));
				else
					ApiUtil::attr_to_device(&((*received_4)[i]),version,&((*dev_attr)[i]));
				
//
// Add an error in the error stack in case there is one
//
		
				long nb_except = (*dev_attr)[i].ext->err_list.in().length();
				if (nb_except != 0)
				{
					TangoSys_OMemStream desc;
					desc << "Failed to read_attributes on device " << device_name;
					desc << ", attribute " << (*dev_attr)[i].name << ends;
		
					(*dev_attr)[i].ext->err_list.inout().length(nb_except + 1);
					(*dev_attr)[i].ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
					(*dev_attr)[i].ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::read_attribute()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
					string st = desc.str();
					(*dev_attr)[i].ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
					char *tmp_str = desc.str();
					(*dev_attr)[i].ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
					delete[] tmp_str;
#endif
					(*dev_attr)[i].ext->err_list[nb_except].severity = Tango::ERR;
				} 
			}
			else
			{
				ApiUtil::attr_to_device(&((*received)[i]),
							NULL,version,&((*dev_attr)[i]));
			}	
		}
				
//
// Remove request from request global table.
//

		remove_asyn_request(id);
		
		return dev_attr;
	}
	
	return NULL;
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attribute_reply()
// 
// description : 	Try to obtain data returned by an attribute asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//
// argin(s) :		id : The asynchronous call identifier
//
// return :		The attribute data
// 
//-----------------------------------------------------------------------------

DeviceAttribute *DeviceProxy::read_attribute_reply(long id)
{

//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if (req.req_type != TgRequest::READ_ATTR)
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::read_attribute_reply");
	}
	
//
// Reply arrived ? Throw exception is not yet arrived
//

	if (req.request->poll_response() == false)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << dev_name();
		desc << ": Reply for asynchronous call (id = " << id;
		desc << ") is not yet arrived" << ends;
		ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       desc.str(),
						       (const char *)"DeviceProxy::read_attribute_reply");
	}
	else
	{
		
//
// Check if the reply is an exception
//

		CORBA::Environment_ptr env = req.request->env();
		if (!CORBA::is_nil(env) && (env->exception() != NULL))
		{
			read_attr_except(req.request,id,SIMPLE);
			
//
// If we arrive here, this means the exception was there due to a server
// shutdown but it is now back into operation
// Try to redo the call synchronously
//

			DeviceAttribute *a_ptr;
			a_ptr = redo_synch_read_call(req);
			
//
// Remove request from request global table.
//

			remove_asyn_request(id);
		
			return a_ptr;
		}
		
		DeviceAttribute *dev_attr = new DeviceAttribute;	
		
//
// Get received value
//

		const Tango::AttributeValueList *received;
		const Tango::AttributeValueList_3 *received_3;
		const Tango::AttributeValueList_4 *received_4;
		
		CORBA::Any &dii_any = req.request->return_value();
		
		if (version < 3)
			dii_any >>= received;
		else if (version == 3)
			dii_any >>= received_3;
		else
			dii_any >>= received_4;

		if (version >= 3)
		{
			if (version == 3)
				ApiUtil::attr_to_device(NULL,&((*received_3)[0]),version,dev_attr);
			else
				ApiUtil::attr_to_device(&((*received_4)[0]),version,dev_attr);
				
//
// Add an error in the error stack in case there is one
//
		
			long nb_except = dev_attr->ext->err_list.in().length();
			if (nb_except != 0)
			{
				TangoSys_OMemStream desc;
				desc << "Failed to read_attributes on device " << device_name;
				desc << ", attribute " << dev_attr->name << ends;
		
				dev_attr->ext->err_list.inout().length(nb_except + 1);
				dev_attr->ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
				dev_attr->ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::read_attribute_reply()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				dev_attr->ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
				char *tmp_str = desc.str();
				dev_attr->ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
				delete[] tmp_str;
#endif
				dev_attr->ext->err_list[nb_except].severity = Tango::ERR;
			} 
		}
		else
		{
			ApiUtil::attr_to_device(&((*received)[0]),
						NULL,version,dev_attr);
		}
				
//
// Remove request from request global table.
//

		remove_asyn_request(id);
		
		return dev_attr;
	}
	return NULL;
}


//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attributes_reply()
// 
// description : 	Try to obtain data returned by a read attributes asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//
// argin(s) :		call_timeout : The max time to block the calling process
//			if timeout is 0, this means wait until the reply is
//			there
//			id : The asynchrnous call identifier
//
// return :		The command output data
// 
//-----------------------------------------------------------------------------

vector<DeviceAttribute> *DeviceProxy::read_attributes_reply(long id,long call_timeout)
{
//
// Retrieve request object
//

	TgRequest req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if (req.req_type != TgRequest::READ_ATTR)
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::read_attributes_reply");
	}

//
// Use CORBA get_response call if the call timeout is specified as 0
// (If the response is not already there).
// Otherwise, uses a loop. Switch process to sleeping state for 20 ms
// between each test to check if the replies has arrived.
//

	if (call_timeout == 0)
	{
		if (req.request->poll_response() == false)
		{
			req.request->get_response();
		}
	}
	else
	{	
		long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef _TG_WINDOWS_
			Sleep(20);
#else
			nanosleep(&to_wait,&inter);
#endif
			if (req.request->poll_response() == true)
			{
				break;
			}
		}
		
		if (i == nb)
		{
			TangoSys_OMemStream desc;
			desc << "Device " << device_name;
			desc << ": Reply for asynchronous call (id = " << id;
			desc << ") is not yet arrived" << ends;
			ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"DeviceProxy::read_attributes_reply");		
		}
	}

//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req.request->env();
	if (!CORBA::is_nil(env) && (env->exception() != NULL))
	{
		read_attr_except(req.request,id,MULTIPLE);
		
//
// If we arrive here, this means the exception was there due to a server
// shutdown but it is now back into operation
// Try to redo the call synchronously
//

		vector<DeviceAttribute> *a_ptr;
		a_ptr = redo_synch_reads_call(req);
			
//
// Remove request from request global table.
//

		remove_asyn_request(id);
		
		return a_ptr;
	}


	vector<DeviceAttribute> *dev_attr = new(vector<DeviceAttribute>);	
		
//
// Get received value
//

	const Tango::AttributeValueList *received;
	const Tango::AttributeValueList_3 *received_3;
	const Tango::AttributeValueList_4 *received_4;
	unsigned long nb_received;
	
	CORBA::Any &dii_any = req.request->return_value();
	if (version < 3)
	{
		dii_any >>= received;
		nb_received = received->length();
	}
	else if (version == 3)
	{
		dii_any >>= received_3;
		nb_received = received_3->length();
	}
	else
	{
		dii_any >>= received_4;
		nb_received = received_4->length();
	}

	dev_attr->resize(nb_received);
							
	for (unsigned long i=0; i < nb_received; i++)
	{
		if (version >= 3)
		{	
			if (version == 3)
				ApiUtil::attr_to_device(NULL,&((*received_3)[i]),version,&((*dev_attr)[i]));
			else
				ApiUtil::attr_to_device(&((*received_4)[i]),version,&((*dev_attr)[i]));

			
//
// Add an error in the error stack in case there is one
//
		
			long nb_except = (*dev_attr)[i].ext->err_list.in().length();
			if (nb_except != 0)
			{
				TangoSys_OMemStream desc;
				desc << "Failed to read_attributes on device " << device_name;
				desc << ", attribute " << (*dev_attr)[i].name << ends;
		
				(*dev_attr)[i].ext->err_list.inout().length(nb_except + 1);
				(*dev_attr)[i].ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
				(*dev_attr)[i].ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::read_attributes_reply()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				(*dev_attr)[i].ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
				char *tmp_str = desc.str();
				(*dev_attr)[i].ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
				delete[] tmp_str;
#endif
				(*dev_attr)[i].ext->err_list[nb_except].severity = Tango::ERR;
			} 
		}
		else
		{
			ApiUtil::attr_to_device(&((*received)[i]),NULL,version,&((*dev_attr)[i]));
		}
	}
				
//
// Remove request from request global table.
//

	remove_asyn_request(id);	
						
	return dev_attr;
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attribute_reply()
// 
// description : 	Try to obtain data returned by a read attribute asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//			This call is for one attribute only
//
// argin(s) :		call_timeout : The max time to block the calling process
//			if timeout is 0, this means wait until the reply is
//			there
//			id : The asynchrnous call identifier
//
// return :		The command output data
// 
//-----------------------------------------------------------------------------


DeviceAttribute *DeviceProxy::read_attribute_reply(long id,long call_timeout)
{
//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if (req.req_type != TgRequest::READ_ATTR)
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::read_attribute_reply");
	}
	
//
// Use CORBA get_response call if the call timeout is specified as 0
// (If the response is not already there).
// Otherwise, uses a loop. Switch process to sleeping state for 20 ms
// between each test to check if the replies has arrived.
//

	if (call_timeout == 0)
	{
		if (req.request->poll_response() == false)
		{
			req.request->get_response();
		}
	}
	else
	{
		long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef _TG_WINDOWS_
			Sleep(20);
#else
			nanosleep(&to_wait,&inter);
#endif
			if (req.request->poll_response() == true)
			{
				break;
			}
		}
	
		if (i == nb)
		{
			TangoSys_OMemStream desc;
			desc << "Device " << device_name;
			desc << ": Reply for asynchronous call (id = " << id;
			desc << ") is not yet arrived" << ends;
			ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"DeviceProxy::read_attributes_reply");		
		}
	}

//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req.request->env();
	if (!CORBA::is_nil(env) && (env->exception() != NULL))
	{
		read_attr_except(req.request,id,SIMPLE);
		
//
// If we arrive here, this means the exception was there due to a server
// shutdown but it is now back into operation
// Try to redo the call synchronously
//

		DeviceAttribute *a_ptr;
		a_ptr = redo_synch_read_call(req);
			
//
// Remove request from request global table.
//

		remove_asyn_request(id);
		
		return a_ptr;
	}


	DeviceAttribute *dev_attr = new DeviceAttribute;	
		
//
// Get received value
//

	const Tango::AttributeValueList *received;
	const Tango::AttributeValueList_3 *received_3;
	const Tango::AttributeValueList_4 *received_4;
	
	CORBA::Any &dii_any = req.request->return_value();

	if (version < 3)
		dii_any >>= received;
	else if (version == 3)
		dii_any >>= received_3;
	else
		dii_any >>= received_4;
		
	if (version >= 3)
	{	
		if (version == 3)
			ApiUtil::attr_to_device(NULL,&((*received_3)[0]),version,dev_attr);
		else
			ApiUtil::attr_to_device(&((*received_4)[0]),version,dev_attr);
				
//
// Add an error in the error stack in case there is one
//
		
		long nb_except = dev_attr->ext->err_list.in().length();
		if (nb_except != 0)
		{
			TangoSys_OMemStream desc;
			desc << "Failed to read_attributes on device " << device_name;
			desc << ", attribute " << dev_attr->name << ends;
		
			dev_attr->ext->err_list.inout().length(nb_except + 1);
			dev_attr->ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
			dev_attr->ext->err_list[nb_except].origin = CORBA::string_dup("DeviceProxy::read_attribute_reply()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			dev_attr->ext->err_list[nb_except].desc = CORBA::string_dup(st.c_str());
#else
			char *tmp_str = desc.str();
			dev_attr->ext->err_list[nb_except].desc = CORBA::string_dup(tmp_str);
			delete[] tmp_str;
#endif
			dev_attr->ext->err_list[nb_except].severity = Tango::ERR;
		} 
	}
	else
	{
		ApiUtil::attr_to_device(&((*received)[0]),NULL,version,dev_attr);
	}
				
//
// Remove request from request global table.
//

	remove_asyn_request(id);
	
	return dev_attr;
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attr_except()
// 
// description : 	Analyse the exception returned by the DII call and 
//			send a Tango::DevFailed exception to the caller
//
// argin(s) :		req : The CORBA request object
//			id : The asynchrnous call identifier
//			type : The request type
// 
//-----------------------------------------------------------------------------


void DeviceProxy::read_attr_except(CORBA::Request_ptr req,long id,read_attr_type type)
{
	CORBA::Environment_ptr env = req->env();
	CORBA::Exception *ex_ptr = env->exception();

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
			
	CORBA::TRANSIENT *tra;
	if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
	{
		if (tra->minor() == omni::TRANSIENT_CallTimedout)
		{		
			CORBA::NVList_ptr req_arg = req->arguments();
			const Tango::DevVarStringArray *names;
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			*(nv->value()) >>= names;
						
			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
			TangoSys_OMemStream desc;
			desc << "Timeout (" << timeout << " mS) exceeded on device " << device_name;
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < names->length();i++)
			{
				desc << (*names)[i];
				if (i != (names->length() - 1))
					desc << ", ";
			}
			desc << ends;
			
			remove_asyn_request(id);

			if (type == SIMPLE)
				ApiCommExcept::re_throw_exception(cb_excep_mess,
							  (const char *)"API_DeviceTimedOut",
						  	  desc.str(),
							  (const char *)"DeviceProxy::read_attribute_reply()");
			else					
				ApiCommExcept::re_throw_exception(cb_excep_mess,
							  (const char *)"API_DeviceTimedOut",
						  	  desc.str(),
							  (const char *)"DeviceProxy::read_attributes_reply()");
		}
	}


	CORBA::UnknownUserException *unk_ex;
	if ((unk_ex = CORBA::UnknownUserException::_downcast(ex_ptr)) != NULL)
	{
			
//
// It is a UserUnknownException exception. This means that the
// server has sent a DevFailed exception
//
	
		const Tango::DevFailed *serv_ex;
		unk_ex->exception() >>= serv_ex;
		Tango::DevFailed ex(*serv_ex);

		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::DevVarStringArray *names;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		*(nv->value()) >>= names;

		TangoSys_OMemStream desc;
		desc << "Failed to execute read_attribute_asynch on device " << device_name;
		desc << "\nAttribute(s): ";
		for (unsigned int i = 0;i < names->length();i++)
		{
			desc << (*names)[i];
			if (i != (names->length() - 1))
				desc << ", ";
		}
		desc << ends;
			
		remove_asyn_request(id);

		if (type == SIMPLE)			
	        	Except::re_throw_exception(ex,
						   (const char*)"API_AttributeFailed",
                        		   	   desc.str(),
						   (const char*)"DeviceProxy::read_attribute_reply()");
		else
	        	Except::re_throw_exception(ex,
						   (const char*)"API_AttributeFailed",
                        		   	   desc.str(),
						   (const char*)"DeviceProxy::read_attributes_reply()");
					
	
	}

	CORBA::SystemException *sys_ex;
	if ((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL)
	{

		set_connection_state(CONNECTION_NOTOK);
			
//
// Re-throw nearly (but not all) all CORBA system exceptions
//

		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::DevVarStringArray *names;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		*(nv->value()) >>= names;

		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

//
// Check if the exception was a connection exception
// In this case, try to ping the device.
// If successfull, just returns otherwise, throw the first exception
//

		string ex(cb_excep_mess);
		string::size_type pos = ex.find("TRANSIENT_ConnectFailed");
		if (pos != string::npos)
		{
			try
			{
				ping();
				return;
			}
			catch (Tango::DevFailed &) {}
		}

		TangoSys_OMemStream desc;
		desc << "Failed to execute read_attributes_asynch on device " << device_name;
		desc << "\nAttribute(s): ";
		for (unsigned int i = 0;i < names->length();i++)
		{
			desc << (*names)[i];
			if (i != (names->length() - 1))
				desc << ", ";
		}
		desc << ends;
					
		remove_asyn_request(id);

		if (type == SIMPLE)
			ApiCommExcept::re_throw_exception(cb_excep_mess,
							  (const char*)"API_CommunicationFailed",
                        			  	  desc.str(),
							  (const char*)"DeviceProxy::read_attribute_reply()");
		else			
			ApiCommExcept::re_throw_exception(cb_excep_mess,
							  (const char*)"API_CommunicationFailed",
                        			  	  desc.str(),
							  (const char*)"DeviceProxy::read_attributes_reply()");	
	}
	
}


//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::write_attributes_asynch()
// 
// description : 	Write Tango device attributes asynchronously.
//			The client is not blocked until the attributes are written
//
// argin(s) :		attr_list : The attribute name(s) and value(s)
//
// return :		The asynchronous call identifier
// 
//-----------------------------------------------------------------------------


long DeviceProxy::write_attributes_asynch(vector<DeviceAttribute> &attr_list)
{
//
// Throw exception if caller not allowed to write_attribute
//
 
	if (access == ACCESS_READ)
	{
		TangoSys_OMemStream desc;
		desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
	
		NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
									  	  (const char *)"DeviceProxy::write_attributes_asynch()");
	}
		
//
// Reconnect to device in case it is needed
//

	try
	{
		check_and_reconnect();
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes_asynch on device " << dev_name() << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"DeviceProxy::write_attributes_asynch()");
	}
			
//
// Create the request object
//

	Tango::AttributeValueList att;
	Tango::AttributeValueList_4 att_4;

	long nb_attr = attr_list.size();
	if (version >= 4)
		att_4.length(nb_attr);
	else
		att.length(nb_attr);

	for (int i = 0;i < nb_attr;i++)
	{
		if (version >= 4)
			ApiUtil::device_to_attr(attr_list[i],att_4[i]);
		else
			ApiUtil::device_to_attr(attr_list[i],att[i],device_name);
	}
		
	CORBA::Request_ptr request;
	if (version >= 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		request = ext->device_4->_request("write_attributes_4");
		request->add_in_arg() <<= att_4;
		request->add_in_arg() <<= ci;
		request->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else if (version == 3)
	{
		request = device->_request("write_attributes_3");
		request->add_in_arg() <<= att;
		request->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else
	{
		request = device->_request("write_attributes");
		request->add_in_arg() <<= att;
	}
	request->exceptions()->add(Tango::_tc_DevFailed);

//
// Send the request 
//

	long id = 0;		
	id = add_asyn_request(request,TgRequest::WRITE_ATTR);

	request->send_deferred();
		
	return id;
}

long DeviceProxy::write_attribute_asynch(DeviceAttribute &attr)
{
//
// Throw exception if caller not allowed to write_attribute
//
 
	if (access == ACCESS_READ)
	{
		TangoSys_OMemStream desc;
		desc << "Writing attribute(s) on device " << dev_name() << " is not authorized" << ends;
	
		NotAllowedExcept::throw_exception((const char *)"API_ReadOnlyMode",desc.str(),
									  	  (const char *)"DeviceProxy::write_attribute_asynch()");
	}
		
//
// Reconnect to device in case it is needed
//

	try
	{
		check_and_reconnect();
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes_asynch on device " << dev_name() << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"DeviceProxy::write_attribute_asynch()");
	}
			
//
// Create the request object
//

	Tango::AttributeValueList att;
	Tango::AttributeValueList_4 att_4;

	if (version < 4)
	{
		att.length(1);
		ApiUtil::device_to_attr(attr,att[0],device_name);
	}
	else
	{
		att_4.length(1);
		ApiUtil::device_to_attr(attr,att_4[0]);
	}
	
	CORBA::Request_ptr request;
	
	if (version >= 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		request = ext->device_4->_request("write_attributes_4");
		request->add_in_arg() <<= att_4;
		request->add_in_arg() <<= ci;
		request->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else if (version == 3)
	{
		request = device->_request("write_attributes_3");
		request->add_in_arg() <<= att;
		request->exceptions()->add(Tango::_tc_MultiDevFailed);
	}	
	else
	{		
		request = device->_request("write_attributes");
		request->add_in_arg() <<= att;
	}
	request->exceptions()->add(Tango::_tc_DevFailed);

//
// Send the request 
//

	long id = 0;		
	id = add_asyn_request(request,TgRequest::WRITE_ATTR_SINGLE);

	request->send_deferred();
		
	return id;
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::write_attributes_reply()
// 
// description : 	Try to obtain data returned by a write attribute asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//
// argin(s) :		call_timeout : The max time to block the calling process
//			if timeout is 0, this means wait until the reply is
//			there
//			id : The asynchrnous call identifier
//
//-----------------------------------------------------------------------------


void DeviceProxy::write_attributes_reply(long id,long call_timeout)
{

//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if ((req.req_type == TgRequest::CMD_INOUT) || (req.req_type == TgRequest::READ_ATTR))
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::write_attributes_reply");
	}

//
// Use CORBA get_response call if the call timeout is specified as 0
// (If the response is not already there).
// Otherwise, uses a loop. Switch process to sleeping state for 20 ms
// between each test to check if the replies has arrived.
//

	if (call_timeout == 0)
	{
		if (req.request->poll_response() == false)
		{
			req.request->get_response();
		}
	}
	else
	{
		long nb = call_timeout / 20;
#ifndef _TG_WINDOWS_
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef _TG_WINDOWS_
			Sleep(20);
#else
			nanosleep(&to_wait,&inter);
#endif
			if (req.request->poll_response() == true)
			{
				break;
			}
		}
		
		if (i == nb)
		{
			TangoSys_OMemStream desc;
			desc << "Device " << device_name;
			desc << ": Reply for asynchronous call (id = " << id;
			desc << ") is not yet arrived" << ends;
			ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"DeviceProxy::write_attributes_reply");		
		}
	}

//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req.request->env();
	if (!CORBA::is_nil(env) && (env->exception() != NULL))
	{
		write_attr_except(req.request,id,req.req_type);
		
			
//
// If we arrive here, this means the exception was there due to a server
// shutdown but it is now back into operation
// Try to redo the call synchronously
//

		redo_synch_write_call(req);
	}
				
//
// Remove request from request global table.
//

	remove_asyn_request(id);	
}


//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::write_attributes_reply()
// 
// description : 	Try to obtain data returned by a read attribute asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. An exception is thrown in this case.
//
// argin(s) :		id : The asynchrnous call identifier
//
//-----------------------------------------------------------------------------


void DeviceProxy::write_attributes_reply(long id)
{
//
// Retrieve request object
//

	TgRequest &req = ApiUtil::instance()->get_pasyn_table()->get_request(id);

//
// Check request type
//

	if ((req.req_type == TgRequest::CMD_INOUT) || (req.req_type == TgRequest::READ_ATTR))
	{
		ApiAsynExcept::throw_exception((const char *)"API_BadAsynReqType",
					       (const char *)"Incompatible request type",
					       (const char *)"Connection::write_attributes_reply");
	}
	
//
// Reply arrived ? Throw exception is not yet arrived
//

	if (req.request->poll_response() == false)
	{
		TangoSys_OMemStream desc;
		desc << "Device " << dev_name();
		desc << ": Reply for asynchronous call (id = " << id;
		desc << ") is not yet arrived" << ends;
		ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       desc.str(),
						       (const char *)"DeviceProxy::write_attributes_reply");
	}
	else
	{
//
// Check if the reply is an exception
//

		CORBA::Environment_ptr env = req.request->env();
		if (!CORBA::is_nil(env) && (env->exception() != NULL))
		{
			write_attr_except(req.request,id,req.req_type);
			
//
// If we arrive here, this means the exception was there due to a server
// shutdown but it is now back into operation
// Try to redo the call synchronously
//

			redo_synch_write_call(req);
		}
	}
				
//
// Remove request from request global table.
//

	remove_asyn_request(id);

}


//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::write_attr_except()
// 
// description : 	Analyse the exception returned by the DII call and 
//			send a Tango::DevFailed exception to the caller
//
// argin(s) :		req : The CORBA request object
//			id : The asynchrnous call identifier
// 
//-----------------------------------------------------------------------------


void DeviceProxy::write_attr_except(CORBA::Request_ptr req,long id,TgRequest::ReqType req_type)
{
	CORBA::Environment_ptr env = req->env();
	CORBA::Exception *ex_ptr = env->exception();

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
			
	CORBA::TRANSIENT *tra;
	if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
	{
		if (tra->minor() == omni::TRANSIENT_CallTimedout)
		{		
			CORBA::NVList_ptr req_arg = req->arguments();
			const Tango::AttributeValueList *att;
			const Tango::AttributeValueList_4 *att_4;
			unsigned int nb_att = 0;
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			
			if (version < 4)
			{
				if ((*(nv->value()) >>= att) == true)
					nb_att = att->length();
			}
			else
			{
				if ((*(nv->value()) >>= att_4) == true)
					nb_att = att_4->length();
			}
						
			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
			TangoSys_OMemStream desc;
			desc << "Timeout (" << timeout << " mS) exceeded on device " << device_name;
			if (nb_att != 0)
			{
				desc << "\nAttribute(s): ";
				for (unsigned int i = 0;i < nb_att;i++)
				{
					(version < 4) ? desc << (*att)[i].name : desc << (*att_4)[i].name;
				
					if (i != (nb_att - 1))
						desc << ", ";
				}
				desc << ends;
			}
			
			remove_asyn_request(id);	

			ApiCommExcept::re_throw_exception(cb_excep_mess,
							  (const char *)"API_DeviceTimedOut",
						  	  desc.str(),
							  (const char *)"DeviceProxy::write_attributes_reply()");
		}
	}

	CORBA::UnknownUserException *unk_ex;
	if ((unk_ex = CORBA::UnknownUserException::_downcast(ex_ptr)) != NULL)
	{
			
//
// It is a UserUnknownException exception. This means that the
// server has sent a DevFailed exception
//
	
		const Tango::DevFailed *serv_ex = NULL;
		Tango::DevFailed ex;
		const Tango::MultiDevFailed *multi_serv_ex = NULL;
		Tango::MultiDevFailed m_ex;
		
		if (version < 3)
		{
			unk_ex->exception() >>= serv_ex;
			ex = *serv_ex;
		}
		else
		{
			if ((unk_ex->exception() >>= multi_serv_ex) == true)
				m_ex = *multi_serv_ex;
			else
			{
				unk_ex->exception() >>= serv_ex;
				ex = *serv_ex;
			}
		}
		
		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::AttributeValueList *att;
		const Tango::AttributeValueList_4 *att_4;
		unsigned int nb_att = 0;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		
		if (version < 4)
		{
			if ((*(nv->value()) >>= att) == true)
				nb_att = att->length();
		}
		else
		{
			if ((*(nv->value()) >>= att_4) == true)
				nb_att = att_4->length();
		}
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes_asynch on device " << device_name;
		if (nb_att != 0)
		{
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < nb_att;i++)
			{
				if (version < 4)
					desc << (*att)[i].name;
				else
					desc << (*att_4)[i].name;
			
				if (i != (nb_att - 1))
					desc << ", ";
			}
		}
		desc << ends;
		
		remove_asyn_request(id);
						
		if (version < 3)
		{
			Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
						   desc.str(),
						   (const char*)"DeviceProxy::write_attributes_reply()");
		}
		else
		{
			if (serv_ex != NULL)
			{
				Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
							   desc.str(),
							   (const char*)"DeviceProxy::write_attributes_reply()");				
			}
			
			if (req_type == TgRequest::WRITE_ATTR)
				throw Tango::NamedDevFailedList(m_ex,
					       			device_name,
					       			(const char *)"DeviceProxy::write_attributes_reply()",
					       			(const char *)"API_AttributeFailed");
			else
			{
			
//
// Transfer this exception into a DevFailed exception
//

				Tango::DevFailed ex(m_ex.errors[0].err_list);
				Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
                        				   desc.str(), (const char*)"DeviceProxy::write_attributes_reply()");
			
			}
		}				
	}

	CORBA::SystemException *sys_ex;
	if ((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL)
	{

		set_connection_state(CONNECTION_NOTOK);
			
//
// Re-throw nearly all CORBA system exceptions
//

		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::AttributeValueList *att;
		const Tango::AttributeValueList *att_4;
		unsigned int nb_att = 0;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		if (version < 4)
		{
			if ((*(nv->value()) >>= att) == true)
				nb_att = att->length();
		}
		else
		{
			if ((*(nv->value()) >>= att_4) == true)
				nb_att = att_4->length();
		}

		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);
		
//
// Check if the exception was a connection exception
// In this case, try to ping the device.
// If successfull, just returns otherwise, throw the first exception
//

		string ex(cb_excep_mess);
		string::size_type pos = ex.find("TRANSIENT_ConnectFailed");
		if (pos != string::npos)
		{
			try
			{
				ping();
				return;
			}
			catch (Tango::DevFailed &) {}
		}

		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes_asynch on device " << device_name;
		if (nb_att != 0)
		{
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < nb_att;i++)
			{
				if (version < 4)
					desc << (*att)[i].name;
				else
					desc << (*att_4)[i].name;
			
				if (i != (nb_att - 1))
					desc << ", ";
			}
		}
		desc << ends;
					
		remove_asyn_request(id);

		ApiCommExcept::re_throw_exception(cb_excep_mess,
						  (const char*)"API_CommunicationFailed",
                        			  desc.str(),
						  (const char*)"DeviceProxy::write_attributes_reply()");	
	}	
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::retreve_read_args()
// 
// description : 	Retrieve the read_attribute asynchrouns call from the
//					Tango request object which itself contains the CORBA
//					request object
//
// argin(s) :		req : The Tango request object
//					att_list : reference to a string vector whihc will be filled
//							   in with attribute(s) name
//
//-----------------------------------------------------------------------------

void DeviceProxy::retrieve_read_args(TgRequest &req,vector<string> &att_list)
{			

	att_list.clear();
	
//
// Retrieve which attribute was read
//

	const Tango::DevVarStringArray *att_names = NULL;
	try
	{
		CORBA::NVList_ptr args_ptr = req.request->arguments();
		CORBA::NamedValue_ptr arg_ptr = args_ptr->item(0);
		CORBA::Any *arg_val = arg_ptr->value();
		(*arg_val) >>= att_names;
		
		for (unsigned loop = 0;loop < att_names->length();loop++)
			att_list.push_back(string((*att_names)[loop]));
	}
	catch (CORBA::SystemException &e)
	{
		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(&e);

		TangoSys_OMemStream desc;
		desc << "Failed to redo the call synchronously on device " << device_name;
		if (att_names != NULL)
		{
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < att_names->length();i++)
			{
				desc << (*att_names)[i];
				if (i != (att_names->length() - 1))
					desc << ", ";
			}
		}
		desc << ends;
					
		ApiCommExcept::re_throw_exception(cb_excep_mess,
							  			  (const char*)"API_CommunicationFailed",
                        			  	  desc.str(),
							  			  (const char*)"DeviceProxy::redo_simpl_call()");
	}
}


//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::redo_synch_read_call()
// 
// description : 	Redo a synchronous read_attribute() call
//					This is needed for re-connection with asynchronous call
//
// argin(s) :		req : The Tango request object
//
//-----------------------------------------------------------------------------

DeviceAttribute *DeviceProxy::redo_synch_read_call(TgRequest &req)
{			

//
// Retrieve which attribute was read
//

	vector<string> att_list;
	retrieve_read_args(req,att_list);

//
// Redo the read_attribute but synchronously
//
		
	DeviceAttribute attrib = read_attribute(att_list[0]);
	DeviceAttribute *attr_ptr = new DeviceAttribute(attrib);
	return attr_ptr;
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::redo_synch_reads_call()
// 
// description : 	Redo a read-attributes() call but synchronously
//					This is needed for re-connection with asynchronous call
//
// argin(s) :		req : The Tango request object
//
//-----------------------------------------------------------------------------

vector<DeviceAttribute> *DeviceProxy::redo_synch_reads_call(TgRequest &req)
{			

//
// Retrieve which attributes was read
//

	vector<string> att_list;
	retrieve_read_args(req,att_list);		

//
// Redo the read_attributes but synchronously
//
		
	return read_attributes(att_list);
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::redo_synch_write_call()
// 
// description : 	Redo a read-attributes() call but synchronously
//					This is needed for re-connection with asynchronous call
//
// argin(s) :		req : The Tango request object
//
//-----------------------------------------------------------------------------

void DeviceProxy::redo_synch_write_call(TgRequest &req)
{			

//
// Retrieve which attributes was written
//

	const Tango::AttributeValueList *att;
	try
	{
		CORBA::NVList_ptr args_ptr = req.request->arguments();
		CORBA::NamedValue_ptr arg_ptr = args_ptr->item(0);
		CORBA::Any *arg_val = arg_ptr->value();
		(*arg_val) >>= att;
	}
	catch (CORBA::SystemException &e)
	{
		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(&e);

		TangoSys_OMemStream desc;
		desc << "Failed to redo the call synchronously on device " << device_name << ends;
					
		ApiCommExcept::re_throw_exception(cb_excep_mess,
							  			  (const char*)"API_CommunicationFailed",
                        			  	  desc.str(),
							  			  (const char*)"DeviceProxy::redo_synch_write_call()");
	}

//
// Redo the write_attributes but synchronously
//
		
	return write_attribute(*att);
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::redo_synch_cmd()
// 
// description : 	Redo a command_inout() call but synchronously
//					This is needed for re-connection with asynchronous call
//
// argin(s) :		req : The Tango request object
//
//-----------------------------------------------------------------------------

DeviceData Connection::redo_synch_cmd(TgRequest &req)
{
	const char *cmd_name;
	const CORBA::Any *a_ptr;

	try
	{
		CORBA::NVList_ptr args_ptr = req.request->arguments();
		CORBA::NamedValue_ptr arg_ptr = args_ptr->item(0);
		
		*(arg_ptr->value()) >>= cmd_name;
					
		arg_ptr = args_ptr->item(1);
		*(arg_ptr->value()) >>= a_ptr;
	}
	catch (CORBA::SystemException &e)
	{
		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(&e);

		TangoSys_OMemStream desc;
		desc << "Failed to redo the call synchronously on device " << dev_name() << ends;
					
		ApiCommExcept::re_throw_exception(cb_excep_mess,
							  			  (const char*)"API_CommunicationFailed",
                        			  	  desc.str(),
							  			  (const char*)"DeviceProxy::redo_synch_write_call()");
	}
	
//
// Redo the call synchronously
//

	DeviceData dd;
	CORBA::Any *tmp_any_ptr = new CORBA::Any(*a_ptr);
	dd.any = tmp_any_ptr;
	
	return command_inout(cmd_name,dd);
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::cancel_asynch_request()
// 
// description : 	Cancel a running asynchronous request
//					This is a client side call. Obviously, the call cannot be aborted
//					while it is running in the device.
//
// argin(s) :		id : The asynchronous call identifier
//
//-----------------------------------------------------------------------------

void Connection::cancel_asynch_request(long id)
{
	omni_mutex_lock guard(ext->asyn_mutex);
	ApiUtil::instance()->get_pasyn_table()->mark_as_cancelled(id);
	pasyn_ctr--;
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::cancel_all_polling_asynch_request()
// 
// description : 	Cancel all running asynchronous request
//					This is a client side call. Obviously, the calls cannot be aborted
//					while it is running in the device.
//
// argin(s) :		id : The asynchronous call identifier
//
//-----------------------------------------------------------------------------

void Connection::cancel_all_polling_asynch_request()
{
	omni_mutex_lock guard(ext->asyn_mutex);
	ApiUtil::instance()->get_pasyn_table()->mark_all_polling_as_cancelled();
	pasyn_ctr = 0;
}

} // End of tango namespace

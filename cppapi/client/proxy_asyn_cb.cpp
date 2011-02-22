static const char *RcsId = "$Id$\n$Name$";

//
// cpp 	- C++ source code file for TANGO api class and asynchronous callback
//	  related methods
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- January 2003
//
// Copyright (C) :      2003,2004,2005,2006,2007,2008,2009,2010,2011
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
// Revision 3.22  2010/09/12 12:18:23  taurel
// - Now, the test suite seems OK
//
// Revision 3.21  2010/09/09 13:44:06  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.20  2010/06/25 07:16:35  taurel
// - Also protect the asynchronous DeviceProxy::read_attributes() methods
// against multiple times the same attribute in att name list
//
// Revision 3.19  2009/12/18 14:51:01  taurel
// - Safety commit before christmas holydays
// - Many changes to make the DeviceProxy, Database and AttributeProxy
// classes thread safe (good help from the helgrind tool from valgrind)
//
// Revision 3.18  2009/03/27 13:05:10  taurel
// - Fix bug due to new Attribute format data member in AttributeValue4
// structure
//
// Revision 3.17  2009/03/27 12:19:37  taurel
// - Fix bug in read_attribute for IDL 4
//
// Revision 3.16  2009/03/13 09:32:27  taurel
// - Small changes to fix Windows VC8 warnings in Warning level 3
//
// Revision 3.15  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.14  2008/12/17 09:54:44  taurel
// - First implementation of attributes sent on the wire using IDL Union
// instead of IDL Any
//
// Revision 3.13  2008/10/06 15:02:17  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.12  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.11  2008/09/23 14:38:29  taurel
// - Commit after the end of DevEncoded data type implementation
// - The new test suite is also now running fine
//
// Revision 3.10  2008/06/14 11:28:07  taurel
// - DevEncoded attribute data type implementation work going on
//
// Revision 3.9  2008/05/20 12:42:30  taurel
// - Commit after merge with release 7 branch
//
// Revision 3.8.2.2  2008/05/20 06:14:21  taurel
// - Last commit before merge with trunk
//
// Revision 3.8.2.1  2008/02/07 15:56:58  taurel
// - First implementation of the Controlled Access done
//
// Revision 3.8  2007/04/20 14:38:33  taurel
// - Ported to Windows 64 bits x64 architecture
//
// Revision 3.7  2007/04/16 14:55:16  taurel
// - Added 3 new attributes data types (DevULong, DevULong64 and DevState)
// - Ported to omniORB4.1
// - Increased the MAX_TRANSFER_SIZE to 256 MBytes
// - Added a new filterable field in the archive event
//
// Revision 3.6  2005/01/13 08:36:37  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.5.2.1  2004/09/15 06:44:43  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.5  2004/07/07 08:39:56  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.4  2003/08/21 07:22:02  taurel
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
// Revision 3.3  2003/07/03 07:37:56  taurel
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
// Revision 3.2.2.2  2004/03/09 16:34:07  taurel
// - Added port to HP aCC (Thanks to Claudio from Elettra)
// - Some last small bug fixes
//
// Revision 3.2.2.1  2003/09/18 14:07:41  taurel
// Fixes some bugs:
//  - Bug fix in DeviceProxy copy constructor and assignement operator
//  - Change the way how DeviceProxy::write_attribute() is coded
//  - Added DeviceAttribute ctors from "const char *"
//  - Split "str().c_str()" in two lines of code. It was the reason of some
//    problems using Windows VC6
//
// Revision 3.2  2003/05/28 14:42:56  taurel
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
//
// Revision 3.0  2003/03/25 16:30:53  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 1.3  2003/03/20 16:09:18  taurel
// Use device instead of device_2 fields of the DeviceProxy class for
// asynchronous calls (still for Java device)
//
// Revision 1.2  2003/03/20 15:02:33  taurel
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
//			cb : The callback object
//
//-----------------------------------------------------------------------------


void Connection::command_inout_asynch(const char *command, DeviceData &data_in, CallBack &cb)
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
		desc << "Failed to execute command_inout on device " << dev_name();
		desc << ", command " << command << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"Connection::command_inout_asynch()");
	}
			
//
// Create the request object
//

	CORBA::ORB::RequestSeq req_seq;
	req_seq.length(1);

	if (version >= 4)
		req_seq[0] = ext->device_4->_request("command_inout_4");
	else if (version >= 2)
		req_seq[0] = device_2->_request("command_inout_2");
	else	
		req_seq[0] = device->_request("command_inout");
	
	req_seq[0]->add_in_arg() <<= command;
	req_seq[0]->add_in_arg() <<= data_in.any.in();

	if (version >= 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		req_seq[0]->add_in_arg() <<= source;
		req_seq[0]->add_in_arg() <<= ci;
	}
	else if (version >= 2)
		req_seq[0]->add_in_arg() <<= source;
	
	req_seq[0]->set_return_type(CORBA::_tc_any);
	req_seq[0]->exceptions()->add(Tango::_tc_DevFailed);

//
// Send the request
// and store it in the global asynchronous cb requests table
//

	ApiUtil *au = ApiUtil::instance();	
	add_asyn_cb_request(req_seq[0],&cb,this,TgRequest::CMD_INOUT);
	CORBA::ORB_ptr orb = au->get_orb();
	orb->send_multiple_requests_deferred(req_seq);
	if (au->get_asynch_cb_sub_model() == PUSH_CALLBACK)
		au->get_pasyn_table()->signal();
	
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
//			cb : The callback object
//
//-----------------------------------------------------------------------------


void Connection::command_inout_asynch(string &command, DeviceData &data_in, CallBack &cb)
{
	command_inout_asynch(command.c_str(),data_in,cb);
}

void Connection::command_inout_asynch(const char *command,CallBack &cb)
{
	DeviceData data_in;
	command_inout_asynch(command,data_in,cb);
}

void Connection::command_inout_asynch(string &command,CallBack &cb)
{
	DeviceData data_in;
	command_inout_asynch(command.c_str(),data_in,cb);
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::get_asynch_replies()
// 
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method does not block if the reply is
//			not yet arrived. This method fires callback for already
//			arrived replies.
//
//-----------------------------------------------------------------------------

void Connection::get_asynch_replies()
{

	CORBA::ORB_ptr orb = ApiUtil::instance()->get_orb();

//
// First get all replies from ORB buffers
//

	try
	{	
		while (orb->poll_next_response() == true)
		{
			CORBA::Request_ptr req;
			orb->get_next_response(req);
				
//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

			TgRequest &tg_req = ApiUtil::instance()->get_pasyn_table()->get_request(req);

			tg_req.arrived = true;
			ApiUtil::instance()->get_pasyn_table()->mark_as_arrived(req);
			
//		
// Is it a request for our device, process it ?
//

			if (tg_req.dev == this)
			{
				switch (tg_req.req_type)
				{
				case TgRequest::CMD_INOUT :
					Cb_Cmd_Request(req,tg_req.cb_ptr);
					break;
				
				case TgRequest::READ_ATTR :
					Cb_ReadAttr_Request(req,tg_req.cb_ptr);
					break;
				
				case TgRequest::WRITE_ATTR :
				case TgRequest::WRITE_ATTR_SINGLE :
					Cb_WriteAttr_Request(req,tg_req.cb_ptr);
					break;
				}
			
				remove_asyn_cb_request(this,req);
			}
		}
		
		
	}
	catch (CORBA::BAD_INV_ORDER &e)
	{
		if (e.minor() != omni::BAD_INV_ORDER_RequestNotSentYet)
			throw;
	}
	
//
// Now check all replies already there
//

	TgRequest *tg_ptr;
	while ((tg_ptr = ApiUtil::instance()->get_pasyn_table()->get_request(this)) != NULL)
	{
		switch (tg_ptr->req_type)
		{
		case TgRequest::CMD_INOUT :
			Cb_Cmd_Request(tg_ptr->request,tg_ptr->cb_ptr);
			break;
				
		case TgRequest::READ_ATTR :
			Cb_ReadAttr_Request(tg_ptr->request,tg_ptr->cb_ptr);
			break;
				
		case TgRequest::WRITE_ATTR :
		case TgRequest::WRITE_ATTR_SINGLE :
			Cb_WriteAttr_Request(tg_ptr->request,tg_ptr->cb_ptr);
			break;
		}
		
		remove_asyn_cb_request(this,tg_ptr->request);
	}	
	
}	
	
	
//-----------------------------------------------------------------------------
//
// method : 		Connection::Cb_Cmd_Request()
// 
// description : 	Fire callback for a command request
//
// argin(s) :		req : The CORBA request object
//			cb_ptr : The callback object
//
//-----------------------------------------------------------------------------	
			
void Connection::Cb_Cmd_Request(CORBA::Request_ptr req,Tango::CallBack *cb_ptr)
{
	
	DeviceData data_out;
	Tango::DevErrorList errors(2);
	errors.length(0);
	
	CORBA::NVList_ptr req_arg = req->arguments();
	const char *cmd;
	CORBA::NamedValue_ptr nv = req_arg->item(0);
	*(nv->value()) >>= cmd;
			
//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req->env();
	if (!CORBA::is_nil(env) && (env->exception() == NULL))
	{
		
//
// Get received value
//

		const CORBA::Any *received;
		CORBA::Any &dii_any = req->return_value();
		dii_any >>= received;
		CORBA::Any *server_any = new CORBA::Any(*received);
			
		data_out.any = server_any;
	}
	else
	{

//
// Retrieve exception 
//
		
		CORBA::Exception *ex_ptr = env->exception();

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
			
		CORBA::TRANSIENT *tra;
		CORBA::UnknownUserException *unk_ex;
		CORBA::SystemException *sys_ex;
		bool to_except = false;
		
		if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
		{
			if (tra->minor() == omni::TRANSIENT_CallTimedout)
			{
				to_except = true;
				char *tmp = CORBA::string_dup(cmd);
				char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
				TangoSys_OMemStream desc;
				desc << "Timeout (" << timeout << " mS) exceeded on device " << dev_name();
				desc << ", command " << tmp << ends;
				CORBA::string_free(tmp);
			
				errors.length(2);
				errors[0].desc = CORBA::string_dup(cb_excep_mess);
				errors[0].severity = Tango::ERR;
				errors[0].reason = CORBA::string_dup("API_CorbaException");
				errors[0].origin = CORBA::string_dup("Connection::Cb_Cmd_Request()");

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				errors[1].desc = CORBA::string_dup(st.c_str());
#else
				char *ptr = desc.str();
				errors[1].desc = CORBA::string_dup(ptr);
				delete[] ptr;
#endif
				errors[1].severity = Tango::ERR;
				errors[1].reason = CORBA::string_dup("API_DeviceTimedOut");
				errors[1].origin = CORBA::string_dup("Connection::Cb_Cmd_request()");
			}
		}
			
			
		if ((unk_ex = CORBA::UnknownUserException::_downcast(ex_ptr)) != NULL)
		{
			
//
// It is a UserUnknownException exception. This means that the
// server has sent a DevFailed exception
//
	
			const Tango::DevFailed *serv_ex;
			unk_ex->exception() >>= serv_ex;
			errors = serv_ex->errors;

			char *tmp = CORBA::string_dup(cmd);

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout_asynch on device " << dev_name();
			desc << ", command " << tmp << ends;
			CORBA::string_free(tmp);

			long nb_err = errors.length();
			errors.length(nb_err + 1);
			errors[nb_err].severity = Tango::ERR;

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			errors[nb_err].desc = CORBA::string_dup(st.c_str());
#else
			char *ptr = desc.str();
			errors[nb_err].desc = CORBA::string_dup(ptr);
			delete[] ptr;
#endif
			errors[nb_err].origin = CORBA::string_dup("Connection::Cb_Cmd_Request()");
			errors[nb_err].reason = CORBA::string_dup("API_CommandFailed");	
		}
		else if (((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL) &&
			 (to_except == false))
		{

			set_connection_state(CONNECTION_NOTOK);
			
//
// Re-throw all CORBA system exceptions
//

			char *tmp = CORBA::string_dup(cmd);

			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

			TangoSys_OMemStream desc;
			desc << "Failed to execute command_inout_asynch on device " << dev_name();
			desc << ", command " << cmd << ends;
			CORBA::string_free(tmp);
			
			errors.length(2);
			errors[0].desc = CORBA::string_dup(cb_excep_mess);
			errors[0].severity = Tango::ERR;
			errors[0].reason = CORBA::string_dup("API_CorbaException");
			errors[0].origin = CORBA::string_dup("Connection::Cb_Cmd_Request()");

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			errors[1].desc = CORBA::string_dup(st.c_str());
#else
			char *ptr = desc.str();
			errors[1].desc = CORBA::string_dup(ptr);
			delete[] ptr;
#endif
			errors[1].severity = Tango::ERR;
			errors[1].reason = CORBA::string_dup("API_CommunicationFailed");
			errors[1].origin = CORBA::string_dup("Connection::Cb_Cmd_request()");	
		}
	}
					
//
// Fire Callback
//
		
	string cmd_str(cmd);

	DeviceProxy *local_dev = static_cast<DeviceProxy *>(this);					
	CmdDoneEvent *cb_data = new CmdDoneEvent(local_dev,cmd_str,data_out,errors);
	cb_ptr->cmd_ended(cb_data);
		
	delete cb_data;

}

//-----------------------------------------------------------------------------
//
// method : 		Connection::Cb_ReadAttr_Request()
// 
// description : 	Fire callback for a read attributes request
//
// argin(s) :		req : The CORBA request object
//			cb_ptr : The callback object
//
//-----------------------------------------------------------------------------

void Connection::Cb_ReadAttr_Request(CORBA::Request_ptr req,Tango::CallBack *cb_ptr)
{
	vector<DeviceAttribute> *dev_attr = NULL;		
	Tango::DevErrorList errors(2);
	errors.length(0);

	CORBA::NVList_ptr req_arg = req->arguments();
	const Tango::DevVarStringArray *names;
	CORBA::NamedValue_ptr nv = req_arg->item(0);
	*(nv->value()) >>= names;
	
	vector<string> attr_names;
	attr_names.resize(names->length());
	for (unsigned int i = 0;i < names->length();i++)
		attr_names[i] = (*names)[i];
				
//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req->env();
	if (!CORBA::is_nil(env) && (env->exception() == NULL))
	{
		
//
// Get received value
//

		dev_attr = new(vector<DeviceAttribute>);	

		const Tango::AttributeValueList *received;
		const Tango::AttributeValueList_3 *received_3;
		const Tango::AttributeValueList_4 *received_4;
		unsigned long nb_received;
		
		CORBA::Any &dii_any = req->return_value();
		
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
					desc << "Failed to read_attributes on device " << dev_name();
					desc << ", attribute " << (*dev_attr)[i].name << ends;
		
					(*dev_attr)[i].ext->err_list.inout().length(nb_except + 1);
					(*dev_attr)[i].ext->err_list[nb_except].reason = CORBA::string_dup("API_AttributeFailed");
					(*dev_attr)[i].ext->err_list[nb_except].origin = CORBA::string_dup("Connection::Cb_ReadAttr_Request");
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
	}
	else
	{
	
//
// The reply is an exception
//

		CORBA::Exception *ex_ptr = env->exception();
		CORBA::UnknownUserException *unk_ex;
		CORBA::SystemException *sys_ex;
		bool to_except = false;

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
			
		CORBA::TRANSIENT *tra;
		if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
		{
			if (tra->minor() == omni::TRANSIENT_CallTimedout)
			{
				to_except = true;			
				char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
				TangoSys_OMemStream desc;
				desc << "Timeout (" << timeout << " mS) exceeded on device " << dev_name();
				desc << "\nAttribute(s): ";
				for (unsigned int i = 0;i < names->length();i++)
				{
					desc << (*names)[i];
					if (i != (names->length() - 1))
						desc << ", ";
				}
				desc << ends;

				errors.length(2);
				errors[0].desc = CORBA::string_dup(cb_excep_mess);
				errors[0].severity = Tango::ERR;
				errors[0].reason = CORBA::string_dup("API_CorbaException");
				errors[0].origin = CORBA::string_dup("Connection::Cb_ReadAttr_Request()");

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				errors[1].desc = CORBA::string_dup(st.c_str());
#else
				char *ptr = desc.str();
				errors[1].desc = CORBA::string_dup(ptr);
				delete[] ptr;
#endif
				errors[1].severity = Tango::ERR;
				errors[1].reason = CORBA::string_dup("API_DeviceTimedOut");
				errors[1].origin = CORBA::string_dup("Connection::Cb_ReadAttr_request()");
			}
		}
		
		if ((unk_ex = CORBA::UnknownUserException::_downcast(ex_ptr)) != NULL)
		{
			
//
// It is a UserUnknownException exception. This means that the
// server has sent a DevFailed exception
//
	
			const Tango::DevFailed *serv_ex;
			unk_ex->exception() >>= serv_ex;
			errors = serv_ex->errors;


			TangoSys_OMemStream desc;
			desc << "Failed to execute read_attribute_asynch on device " << dev_name();
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < names->length();i++)
			{
				desc << (*names)[i];
				if (i != (names->length() - 1))
					desc << ", ";
			}
			desc << ends;

			long nb_err = errors.length();
			errors.length(nb_err + 1);
			errors[nb_err].severity = Tango::ERR;

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			errors[nb_err].desc = CORBA::string_dup(st.c_str());
#else
			char *ptr = desc.str();
			errors[nb_err].desc = CORBA::string_dup(ptr);
			delete[] ptr;
#endif
			errors[nb_err].origin = CORBA::string_dup("Connection::Cb_ReadAttr_Request()");
			errors[nb_err].reason = CORBA::string_dup("API_AttributeFailed");	
		}
		else if (((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL) &&
			 (to_except == false))
		{

			set_connection_state(CONNECTION_NOTOK);
			
//
// Re-throw all CORBA system exceptions
//

			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

			TangoSys_OMemStream desc;
			desc << "Failed to execute read_attributes_asynch on device " << dev_name();
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < names->length();i++)
			{
				desc << (*names)[i];
				if (i != (names->length() - 1))
					desc << ", ";
			}
			desc << ends;
			
			errors.length(2);
			errors[0].desc = CORBA::string_dup(cb_excep_mess);
			errors[0].severity = Tango::ERR;
			errors[0].reason = CORBA::string_dup("API_CorbaException");
			errors[0].origin = CORBA::string_dup("Connection::Cb_ReadAttr_Request()");

#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			errors[1].desc = CORBA::string_dup(st.c_str());
#else
			char *ptr = desc.str();
			errors[1].desc = CORBA::string_dup(ptr);
			delete[] ptr;
#endif
			errors[1].severity = Tango::ERR;
			errors[1].reason = CORBA::string_dup("API_CommunicationFailed");
			errors[1].origin = CORBA::string_dup("Connection::Cb_ReadAttr_Request()");	
		}
	}
	
//
// Fire Callback
//
		
	DeviceProxy *local_dev = static_cast<DeviceProxy *>(this);										
	AttrReadEvent *cb_data = new AttrReadEvent(local_dev,attr_names,dev_attr,errors);
	cb_ptr->attr_read(cb_data);
		
	delete cb_data;	
}

//-----------------------------------------------------------------------------
//
// method : 		Connection::Cb_WriteAttr_Request()
// 
// description : 	Fire callback for a write attribute request
//
// argin(s) :		req : The CORBA request object
//			cb_ptr : The callback object
//
//-----------------------------------------------------------------------------

void Connection::Cb_WriteAttr_Request(CORBA::Request_ptr req,Tango::CallBack *cb_ptr)
{
	Tango::NamedDevFailedList err_3;

	CORBA::NVList_ptr req_arg = req->arguments();
	const Tango::AttributeValueList *att;
	const Tango::AttributeValueList_4 *att_4;
	CORBA::NamedValue_ptr nv = req_arg->item(0);
	long nb_attr = 0;
	if (version >= 4)
	{
		if ((*(nv->value()) >>= att_4) == true)
			nb_attr = att_4->length();
	}
	else
	{
		if ((*(nv->value()) >>= att) == true)
			nb_attr = att->length();
	}
					
//
// Check if the reply is an exception
//

	CORBA::Environment_ptr env = req->env();
	if (!CORBA::is_nil(env) && (env->exception() != NULL))
	{
	
//
// The reply is an exception
//

		CORBA::Exception *ex_ptr = env->exception();
		CORBA::UnknownUserException *unk_ex;
		CORBA::SystemException *sys_ex;
		bool to_except = false;

//
// Special treatement for timeout exception (TRANSIENT with specific minor code)
//
			
		CORBA::TRANSIENT *tra;
		if ((tra = CORBA::TRANSIENT::_downcast(ex_ptr)) != NULL)
		{
			if (tra->minor() == omni::TRANSIENT_CallTimedout)
			{
				to_except = true;			
				char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
				TangoSys_OMemStream desc;
				desc << "Timeout (" << timeout << " mS) exceeded on device " << dev_name();
				if (nb_attr != 0)
				{
					desc << "\nAttribute(s): ";
					for (int i = 0;i < nb_attr;i++)
					{
						(version < 4) ? desc << (*att)[i].name : desc << (*att_4)[i].name;
					
						if (i != (nb_attr - 1))
							desc << ", ";
					}
				}
				desc << ends;

				err_3.errors.length(2);
				err_3.errors[0].desc = CORBA::string_dup(cb_excep_mess);
				err_3.errors[0].severity = Tango::ERR;
				err_3.errors[0].reason = CORBA::string_dup("API_CorbaException");
				err_3.errors[0].origin = CORBA::string_dup("Connection::Cb_WriteAttr_Request()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				err_3.errors[1].desc = CORBA::string_dup(st.c_str());
#else
				char *ptr = desc.str();
				err_3.errors[1].desc = CORBA::string_dup(ptr);
				delete[] ptr;
#endif
				err_3.errors[1].severity = Tango::ERR;
				err_3.errors[1].reason = CORBA::string_dup("API_DeviceTimedOut");
				err_3.errors[1].origin = CORBA::string_dup("Connection::Cb_WriteAttr_request()");
			}
		}
		
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
			TangoSys_OMemStream desc;

			if (version < 3)
			{		
				unk_ex->exception() >>= serv_ex;
				ex = *serv_ex;
				
				desc << "Failed to execute write_attributes_asynch on device " << dev_name();
				if (nb_attr != 0)
					desc << "\nAttribute(s): ";
				for (int i = 0;i < nb_attr;i++)
				{
					desc << (*att)[i].name.in();
					if (i != (nb_attr - 1))
						desc << ", ";
				}
				desc << ends;

				err_3.errors.length(1);
				err_3.errors[0].severity = Tango::ERR;
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
				string st = desc.str();
				err_3.errors[0].desc = CORBA::string_dup(st.c_str());
#else
				char *ptr = desc.str();
				err_3.errors[0].desc = CORBA::string_dup(ptr);
				delete[] ptr;
#endif
				err_3.errors[0].origin = CORBA::string_dup("Connection::Cb_WriteAttr_Request()");
				err_3.errors[0].reason = CORBA::string_dup("API_AttributeFailed");

				err_3.err_list.resize(1);
				err_3.err_list[0].err_stack = serv_ex->errors;
				err_3.err_list[0].name = (*att)[0].name.in();				
				err_3.err_list[0].idx_in_call = 0;				
			}
			else
			{
				if ((unk_ex->exception() >>= multi_serv_ex) == true)
				{
					m_ex = *multi_serv_ex;
					err_3 = Tango::NamedDevFailedList(m_ex,
					       dev_name(),
					       (const char *)"Connection::Cb_WriteAttr_Request()",
					       (const char *)"API_AttributeFailed");
				}
				else
				{
					unk_ex->exception() >>= serv_ex;
					ex = *serv_ex;
					
					desc << "Failed to execute write_attributes_asynch on device " << dev_name();
					if (nb_attr != 0)
						desc << "\nAttribute(s): ";
					for (int i = 0;i < nb_attr;i++)
					{
						(version < 4) ? desc << (*att)[i].name : desc << (*att_4)[i].name;
						
						if (i != (nb_attr - 1))
							desc << ", ";
					}
					desc << ends;

					err_3.errors.length(1);
					err_3.errors[0].severity = Tango::ERR;
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
					string st = desc.str();
					err_3.errors[0].desc = CORBA::string_dup(st.c_str());
#else
					char *ptr = desc.str();
					err_3.errors[0].desc = CORBA::string_dup(ptr);
					delete[] ptr;
#endif
					err_3.errors[0].origin = CORBA::string_dup("Connection::Cb_WriteAttr_Request()");
					err_3.errors[0].reason = CORBA::string_dup("API_AttributeFailed");

					err_3.err_list.resize(1);
					err_3.err_list[0].err_stack = serv_ex->errors;
					err_3.err_list[0].name = (*att)[0].name.in();				
					err_3.err_list[0].idx_in_call = 0;				
				}
			}			
		}
		else if (((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL) &&
			 (to_except == false))
		{

			set_connection_state(CONNECTION_NOTOK);
			
//
// Re-throw all CORBA system exceptions
//

			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

			TangoSys_OMemStream desc;
			desc << "Failed to execute write_attributes_asynch on device " << dev_name();
			if (nb_attr != 0)
				desc << "\nAttribute(s): ";
			for (int i = 0;i < nb_attr;i++)
			{
				(version < 4) ? desc << (*att)[i].name : desc << (*att_4)[i].name;

				if (i != (nb_attr - 1))
					desc << ", ";
			}
			desc << ends;
			
			err_3.errors.length(2);
			err_3.errors[0].desc = CORBA::string_dup(cb_excep_mess);
			err_3.errors[0].severity = Tango::ERR;
			err_3.errors[0].reason = CORBA::string_dup("API_CorbaException");
			err_3.errors[0].origin = CORBA::string_dup("Connection::Cb_WriteAttr_Request()");
#if ((defined _TG_WINDOWS_) || (defined __SUNPRO_CC) || (defined GCC_STD))
			string st = desc.str();
			err_3.errors[1].desc = CORBA::string_dup(st.c_str());
#else
			char *ptr = desc.str();
			err_3.errors[1].desc = CORBA::string_dup(ptr);
			delete[] ptr;
#endif
			err_3.errors[1].severity = Tango::ERR;
			err_3.errors[1].reason = CORBA::string_dup("API_CommunicationFailed");
			err_3.errors[1].origin = CORBA::string_dup("Connection::Cb_WriteAttr_Request()");	
		}
	}
	
//
// Fire Callback
//
		
	vector<string> att_name;
	if (version >= 4)
	{
		for (int i = 0;i < nb_attr;i++)
			att_name.push_back((*att_4)[i].name.in());
	}
	else
	{
		for (int i = 0;i < nb_attr;i++)
			att_name.push_back((*att)[i].name.in());
	}		

	DeviceProxy *local_dev = static_cast<DeviceProxy *>(this);							
	AttrWrittenEvent *cb_data = new AttrWrittenEvent(local_dev,att_name,err_3);
	cb_ptr->attr_written(cb_data);
		
	delete cb_data;	
}


//-----------------------------------------------------------------------------
//
// method : 		Connection::get_asynch_replies()
// 
// description : 	Try to obtain data returned by a command asynchronously
//			requested. This method block for the specified timeout
//			if the reply is not yet arrived. This method fires
//			callback when the reply arived. If the timeout is
//			set to 0, the call wait undefinitely wait the reply.
//
// argin(s) : 		call_timeout : The timeout in mS
//
//-----------------------------------------------------------------------------

void Connection::get_asynch_replies(long call_timeout)
{
//
// First check all replies already there
//

	TgRequest *tg_ptr;
	while ((tg_ptr = ApiUtil::instance()->get_pasyn_table()->get_request(this)) != NULL)
	{
		switch (tg_ptr->req_type)
		{
		case TgRequest::CMD_INOUT :
			Cb_Cmd_Request(tg_ptr->request,tg_ptr->cb_ptr);
			break;
				
		case TgRequest::READ_ATTR :
			Cb_ReadAttr_Request(tg_ptr->request,tg_ptr->cb_ptr);
			break;
				
		case TgRequest::WRITE_ATTR :
		case TgRequest::WRITE_ATTR_SINGLE :
			Cb_WriteAttr_Request(tg_ptr->request,tg_ptr->cb_ptr);
			break;
		}
		
		remove_asyn_cb_request(this,tg_ptr->request);
	}	

//
// If they are requests already sent but without being replied yet
//

	if (get_pasyn_cb_ctr() != 0)
	{	
		CORBA::ORB_ptr orb = ApiUtil::instance()->get_orb();
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
				
			while ((nb > 0) && (get_pasyn_cb_ctr() != 0))
			{
#ifdef _TG_WINDOWS_
				Sleep(20);
#else		
				nanosleep(&to_wait,&inter);
#endif
				nb--;
			
				if (orb->poll_next_response() == true)
				{
					orb->get_next_response(req);
								
//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

					TgRequest &tg_req = ApiUtil::instance()->get_pasyn_table()->get_request(req);

					tg_req.arrived = true;
					ApiUtil::instance()->get_pasyn_table()->mark_as_arrived(req);
			
//		
// Is it a request for our device, process it ?
//

					if (tg_req.dev == this)
					{
						switch (tg_req.req_type)
						{
						case TgRequest::CMD_INOUT :
							Cb_Cmd_Request(req,tg_req.cb_ptr);
							break;
				
						case TgRequest::READ_ATTR :
							Cb_ReadAttr_Request(req,tg_req.cb_ptr);
							break;
				
						case TgRequest::WRITE_ATTR :
						case TgRequest::WRITE_ATTR_SINGLE :
							Cb_WriteAttr_Request(req,tg_req.cb_ptr);
							break;
						}
			
						remove_asyn_cb_request(this,req);
					}
				}
			}

//
// Throw exception if the timeout has expired but there are still request
// without replies
//
			
			if ((nb == 0) && (get_pasyn_cb_ctr() != 0))
			{	
				TangoSys_OMemStream desc;
				desc << "Still some reply(ies) for asynchronous callback call(s) to be received" << ends;
				ApiAsynNotThereExcept::throw_exception((const char *)"API_AsynReplyNotArrived",
						       	       desc.str(),
						               (const char *)"Connection::get_asynch_replies");		
			}		
		}
		else
		{
		
//
// If timeout is set to 0, this means wait until all the requests sent to this
// device has sent their replies
//

			while (get_pasyn_cb_ctr() != 0)
			{					
				orb->get_next_response(req);
								
//
// Retrieve this request in the cb request map and mark it as "arrived" in both maps
//

				TgRequest &tg_req = ApiUtil::instance()->get_pasyn_table()->get_request(req);

				tg_req.arrived = true;
				ApiUtil::instance()->get_pasyn_table()->mark_as_arrived(req);
			
//		
// Is it a request for our device, process it ?
//

				if (tg_req.dev == this)
				{
					switch (tg_req.req_type)
					{
					case TgRequest::CMD_INOUT :
						Cb_Cmd_Request(req,tg_req.cb_ptr);
						break;
				
					case TgRequest::READ_ATTR :
						Cb_ReadAttr_Request(req,tg_req.cb_ptr);
						break;
				
					case TgRequest::WRITE_ATTR :
					case TgRequest::WRITE_ATTR_SINGLE :
						Cb_WriteAttr_Request(req,tg_req.cb_ptr);
						break;
					}
			
					remove_asyn_cb_request(this,req);
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::read_attributes_asynch()
// 
// description : 	Read Tango device attributes asynchrnously.
//			The client is not blocked until the attributes are read
//
// argin(s) :		attr_names : The attribute name(s)
//			cb : The callback object
//
//-----------------------------------------------------------------------------


void DeviceProxy::read_attributes_asynch(vector<string> &attr_names,CallBack &cb)
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

	CORBA::ORB::RequestSeq req_seq;
	req_seq.length(1);
	
	Tango::DevVarStringArray names;
	long nb_names = attr_names.size();
	names.length(nb_names);
	for (int i = 0;i < nb_names;i++)
		names[i] = attr_names[i].c_str();

	if (version == 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		req_seq[0] = Connection::ext->device_4->_request("read_attributes_4");
		req_seq[0]->add_in_arg() <<= names;
		req_seq[0]->add_in_arg() <<= source;
		req_seq[0]->add_in_arg() <<= ci;
		req_seq[0]->set_return_type(Tango::_tc_AttributeValueList_4);
	}
	else if (version == 3)
	{
		req_seq[0] = Connection::ext->device_3->_request("read_attributes_3");
		req_seq[0]->add_in_arg() <<= names;
		req_seq[0]->add_in_arg() <<= source;
		req_seq[0]->set_return_type(Tango::_tc_AttributeValueList_3);
	}
	else if (version == 2)
	{
		req_seq[0] = device_2->_request("read_attributes_2");
		req_seq[0]->add_in_arg() <<= names;
		req_seq[0]->add_in_arg() <<= source;
		req_seq[0]->set_return_type(Tango::_tc_AttributeValueList);
	}
	else
	{
		req_seq[0] = device->_request("read_attributes");
		req_seq[0]->add_in_arg() <<= names;
		req_seq[0]->set_return_type(Tango::_tc_AttributeValueList);
	}
	req_seq[0]->exceptions()->add(Tango::_tc_DevFailed);
	
//
// Send the request
// and store it in the global asynchronous cb requests table
//

	ApiUtil *au = ApiUtil::instance();	
	add_asyn_cb_request(req_seq[0],&cb,this,TgRequest::READ_ATTR);
	CORBA::ORB_ptr orb = au->get_orb();
	orb->send_multiple_requests_deferred(req_seq);
	if (au->get_asynch_cb_sub_model() == PUSH_CALLBACK)
		au->get_pasyn_table()->signal();
}

void DeviceProxy::read_attribute_asynch(string &attr_name,CallBack &cb)
{
	vector<string> tmp_att_names(1,attr_name);
	read_attributes_asynch(tmp_att_names,cb);
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::write_attributes_asynch()
// 
// description : 	Write Tango device attributes asynchronously.
//			The client is not blocked until the attributes are written
//
// argin(s) :		attr_list : The attribute name(s) and value(s)
//			cb : The callback object
//
//-----------------------------------------------------------------------------


void DeviceProxy::write_attributes_asynch(vector<DeviceAttribute> &attr_list,
					  CallBack &cb)
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
                        desc.str(), (const char*)"DeviceProxy::write_attributes_asynch()");
	}
	
//
// Create the request object
//

	CORBA::ORB::RequestSeq req_seq;
	req_seq.length(1);
	

	Tango::AttributeValueList att;
	Tango::AttributeValueList_4 att_4;

	long nb_attr = attr_list.size();
	if (version < 4)
		att.length(nb_attr);
	else
		att_4.length(nb_attr);

	for (int i = 0;i < nb_attr;i++)
	{
		if (version < 4)
			ApiUtil::device_to_attr(attr_list[i],att[i],device_name);
		else
			ApiUtil::device_to_attr(attr_list[i],att_4[i]);
	}

	if (version >= 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		req_seq[0] = ext->device_4->_request("write_attributes_4");
		req_seq[0]->add_in_arg() <<= att_4;
		req_seq[0]->add_in_arg() <<= ci;
		req_seq[0]->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else if (version == 3)
	{		
		req_seq[0] = device->_request("write_attributes_3");
		req_seq[0]->add_in_arg() <<= att;
		req_seq[0]->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else
	{
		req_seq[0] = device->_request("write_attributes");
		req_seq[0]->add_in_arg() <<= att;
	}
	req_seq[0]->exceptions()->add(Tango::_tc_DevFailed);
	
//
// Send the request
// and store it in the global asynchronous cb requests table
//

	ApiUtil *au = ApiUtil::instance();	
	add_asyn_cb_request(req_seq[0],&cb,this,TgRequest::WRITE_ATTR);
	CORBA::ORB_ptr orb = au->get_orb();
	orb->send_multiple_requests_deferred(req_seq);
	if (au->get_asynch_cb_sub_model() == PUSH_CALLBACK)
		au->get_pasyn_table()->signal();
}

//-----------------------------------------------------------------------------
//
// method : 		DeviceProxy::write_attribute_asynch()
// 
// description : 	Write Tango device attribute asynchronously.
//			The client is not blocked until the attributes are written
//
//			WARNING : Obviously, this method could simply create
//			a vector of DeviceAttribute and call the previous
//			method (write_attributes_asynch (note the attributes))
//			using this vector. But the copy constructor of the
//			DeviceAttribute class, pass the memory to the newly
//			created DeviceAttribute object !!!!
//			Therefore, it is not possible to use this solution
//			in this case
//
// argin(s) :		attr_list : The attribute name(s) and value(s)
//			cb : The callback object
//
//-----------------------------------------------------------------------------

void DeviceProxy::write_attribute_asynch(DeviceAttribute &attr,CallBack &cb)
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
                        desc.str(), (const char*)"DeviceProxy::write_attributes_asynch()");
	}
	
//
// Create the request object
//

	CORBA::ORB::RequestSeq req_seq;
	req_seq.length(1);
	

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

	if (version >= 4)
	{
		ClntIdent ci;
		ApiUtil *au = ApiUtil::instance();
		ci.cpp_clnt(au->get_client_pid());
		
		req_seq[0] = ext->device_4->_request("write_attributes_4");
		req_seq[0]->add_in_arg() <<= att_4;
		req_seq[0]->add_in_arg() <<= ci;
		req_seq[0]->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else if (version == 3)
	{
		req_seq[0] = device->_request("write_attributes_3");
		req_seq[0]->add_in_arg() <<= att;
		req_seq[0]->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	else
	{
		req_seq[0] = device->_request("write_attributes");
		req_seq[0]->add_in_arg() <<= att;
	}
	req_seq[0]->exceptions()->add(Tango::_tc_DevFailed);
	
//
// Send the request
// and store it in the global asynchronous cb requests table
//

	ApiUtil *au = ApiUtil::instance();	
	add_asyn_cb_request(req_seq[0],&cb,this,TgRequest::WRITE_ATTR_SINGLE);
	CORBA::ORB_ptr orb = au->get_orb();
	orb->send_multiple_requests_deferred(req_seq);
	if (au->get_asynch_cb_sub_model() == PUSH_CALLBACK)
		au->get_pasyn_table()->signal();
}


} // End of Tango namespace

static const char *RcsId = "$Id$\n$Name$";

//
// cpp 	- C++ source code file for TANGO api Connection and DeviceProxy
//	  classes (asynchronous polling related methods)
//
// programmer 	- Emmanuel Taurel (taurel@esrf.fr)
//
// original 	- August 2002
//
// $Log$
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
#ifdef WIN32
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
// Reconnect to device in case it is needed
//

	try
	{
		if (connection_state != CONNECTION_OK)
			reconnect(dbase_used);
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
	if (version == 2)
		request = device_2->_request("command_inout_2");
	else	
		request = device->_request("command_inout");
	request->add_in_arg() <<= command;
	request->add_in_arg() <<= data_in.any.in();
	if (version == 2)
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
		id = ApiUtil::instance()->get_pasyn_table()->store_request(request,TgRequest::CMD_INOUT);
		request->send_deferred();
		pasyn_ctr++;
	}
	else
	{
		request->send_oneway();
		CORBA::release(request);
//		connection_state = CONNECTION_NOTOK;
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
	else
	{
			
		DeviceData data_out;
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
			
					ApiUtil::instance()->get_pasyn_table()->remove_request(id);
					pasyn_ctr--;	
					
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
				desc << ", command " << cmd << ends;
				CORBA::string_free(tmp);
			
				ApiUtil::instance()->get_pasyn_table()->remove_request(id);
				pasyn_ctr--;
			
	               		Except::re_throw_exception(ex,
						   (const char*)"API_CommandFailed",
                        			   desc.str(),
						   (const char*)"Connection::command_inout_reply()");
					
	
			}

			CORBA::SystemException *sys_ex;
			if ((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL)
			{

				connection_state = CONNECTION_NOTOK;
			
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
				desc << ", command " << cmd << ends;
				CORBA::string_free(tmp);
			
				ApiUtil::instance()->get_pasyn_table()->remove_request(id);
				pasyn_ctr--;
			
	               		ApiCommExcept::re_throw_exception(cb_excep_mess,
						          (const char*)"API_CommunicationFailed",
                        			          desc.str(),
						          (const char*)"Connection::command_inout_reply()");
					
	
			}		
		}
				
//
// Remove request from request global table.
//

		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;
					
		return data_out;
	}
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
#ifndef WIN32
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef WIN32
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
				
				ApiUtil::instance()->get_pasyn_table()->remove_request(id);			
				pasyn_ctr--;	
					
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
			desc << ", command " << cmd << ends;
			CORBA::string_free(tmp);
			
			ApiUtil::instance()->get_pasyn_table()->remove_request(id);
			pasyn_ctr--;
			
	               	Except::re_throw_exception(ex,
						   (const char*)"API_CommandFailed",
                        			   desc.str(),
						   (const char*)"Connection::command_inout_reply()");
					
	
		}

		CORBA::SystemException *sys_ex;
		if ((sys_ex = CORBA::SystemException::_downcast(ex_ptr)) != NULL)
		{

			connection_state = CONNECTION_NOTOK;
			
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
			desc << ", command " << cmd << ends;
			CORBA::string_free(tmp);
			
			ApiUtil::instance()->get_pasyn_table()->remove_request(id);
			pasyn_ctr--;
			
	               	ApiCommExcept::re_throw_exception(cb_excep_mess,
						          (const char*)"API_CommunicationFailed",
                        			          desc.str(),
						          (const char*)"Connection::command_inout_reply()");
					
	
		}		
	}
				
//
// Remove request from request global table.
//

	ApiUtil::instance()->get_pasyn_table()->remove_request(id);
	pasyn_ctr--;
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
		if (connection_state != CONNECTION_OK)
			reconnect(dbase_used);
	}
	catch (Tango::ConnectionFailed &e)
	{
		TangoSys_OMemStream desc;
		desc << "Failed to execute read_attributes_asynch on device " << dev_name() << ends;
                ApiConnExcept::re_throw_exception(e,(const char*)"API_CommandFailed",
                        desc.str(), (const char*)"DeviceProxy::read_attributes_asynch()");
	}
			
//
// Create the request object
//

	Tango::DevVarStringArray names;
	long nb_names = attr_names.size();
	names.length(nb_names);
	for (int i = 0;i < nb_names;i++)
		names[i] = attr_names[i].c_str();
		
	CORBA::Request_ptr request;
	if (version == 3)
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
	
	id = ApiUtil::instance()->get_pasyn_table()->store_request(request,TgRequest::READ_ATTR);
	request->send_deferred();
	pasyn_ctr++;
	
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
		}
		
		vector<DeviceAttribute> *dev_attr = new(vector<DeviceAttribute>);	
		
//
// Get received value
//

		const Tango::AttributeValueList *received;
		const Tango::AttributeValueList_3 *received_3;
		CORBA::Any &dii_any = req.request->return_value();
		unsigned long nb_received;

		if (version < 3)
		{
			dii_any >>= received;
			nb_received = received->length();
		}
		else
		{
			dii_any >>= received_3;
			nb_received = received_3->length();
		}
		dev_attr->resize(nb_received);
				
		for (unsigned long i=0; i < nb_received; i++)
		{
			if (version >= 3)
			{		
				ApiUtil::attr_to_device(NULL,
							&((*received_3)[i]),
							version,&((*dev_attr)[i]));


				
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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
							NULL,
							version,&((*dev_attr)[i]));
			}	
		}
				
//
// Remove request from request global table.
//

		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;	
		
		return dev_attr;
	}
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
		}
		
		DeviceAttribute *dev_attr = new DeviceAttribute;	
		
//
// Get received value
//

		const Tango::AttributeValueList *received;
		const Tango::AttributeValueList_3 *received_3;
		
		CORBA::Any &dii_any = req.request->return_value();
		
		if (version < 3)
			dii_any >>= received;
		else
			dii_any >>= received_3;

		if (version >= 3)
		{
			ApiUtil::attr_to_device(NULL,
						&((*received_3)[0]),
						version,dev_attr);				
	

			
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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
						NULL,
						version,dev_attr);
		}
				
//
// Remove request from request global table.
//

		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;	
		
		return dev_attr;
	}

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
#ifndef WIN32
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef WIN32
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
	}


	vector<DeviceAttribute> *dev_attr = new(vector<DeviceAttribute>);	
		
//
// Get received value
//

	const Tango::AttributeValueList *received;
	const Tango::AttributeValueList_3 *received_3;
	unsigned long nb_received;
	
	CORBA::Any &dii_any = req.request->return_value();
	if (version < 3)
	{
		dii_any >>= received;
		nb_received = received->length();
	}
	else
	{
		dii_any >>= received_3;
		nb_received = received_3->length();
	}

	dev_attr->resize(nb_received);
							
	for (unsigned long i=0; i < nb_received; i++)
	{
		if (version >= 3)
		{	
			ApiUtil::attr_to_device(NULL,
						&((*received_3)[i]),
						version,&((*dev_attr)[i]));


			
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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
						NULL,
						version,&((*dev_attr)[i]));
		}
	}
				
//
// Remove request from request global table.
//

	ApiUtil::instance()->get_pasyn_table()->remove_request(id);
	pasyn_ctr--;	
		
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
#ifndef WIN32
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef WIN32
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
	}


	DeviceAttribute *dev_attr = new DeviceAttribute;	
		
//
// Get received value
//

	const Tango::AttributeValueList *received;
	const Tango::AttributeValueList_3 *received_3;
	CORBA::Any &dii_any = req.request->return_value();
	if (version < 3)
		dii_any >>= received;
	else
		dii_any >>= received_3;
		
	if (version >= 3)
	{			
		ApiUtil::attr_to_device(NULL,
					&((*received_3)[0]),
					version,dev_attr);
				
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
#if ((defined WIN32) || (defined __SUNPRO_CC) || (defined GCC_STD) || (defined __HP_aCC))
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
					NULL,
					version,dev_attr);
	}
				
//
// Remove request from request global table.
//

	ApiUtil::instance()->get_pasyn_table()->remove_request(id);
	pasyn_ctr--;	
		
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
			
			ApiUtil::instance()->get_pasyn_table()->remove_request(id);
			pasyn_ctr--;	

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
			
		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;

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

		connection_state = CONNECTION_NOTOK;
			
//
// Re-throw all CORBA system exceptions
//

		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::DevVarStringArray *names;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		*(nv->value()) >>= names;

		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

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
					
		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;

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
// Reconnect to device in case it is needed
//

	try
	{
		if (connection_state != CONNECTION_OK)
			reconnect(dbase_used);
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
	long nb_attr = attr_list.size();
	att.length(nb_attr);
	for (int i = 0;i < nb_attr;i++)
	{
		att[i].name = attr_list[i].name.c_str();
		att[i].quality = attr_list[i].quality;
		att[i].time = attr_list[i].time;
		att[i].dim_x = attr_list[i].dim_x;
		att[i].dim_y = attr_list[i].dim_y;
		
		if (attr_list[i].LongSeq.operator->() != NULL)
		{
			att[i].value <<= attr_list[i].LongSeq.in();	
			continue;
		}
		if (attr_list[i].ShortSeq.operator->() != NULL)
		{	
			att[i].value <<= attr_list[i].ShortSeq.in();
			continue;
		}	
		if (attr_list[i].DoubleSeq.operator->() != NULL)
		{	
			att[i].value <<= attr_list[i].DoubleSeq.in();
			continue;
		}
		if (attr_list[i].StringSeq.operator->() != NULL)
		{	
			att[i].value  <<= attr_list[i].StringSeq.in();
			continue;
		}
		if (attr_list[i].FloatSeq.operator->() != NULL)
		{
			att[i].value <<= attr_list[i].FloatSeq.in();	
			continue;
		}
		if (attr_list[i].BooleanSeq.operator->() != NULL)
		{	
			att[i].value <<= attr_list[i].BooleanSeq.in();
			continue;
		}	
		if (attr_list[i].UShortSeq.operator->() != NULL)
		{	
			att[i].value <<= attr_list[i].UShortSeq.in();
			continue;
		}
		if (attr_list[i].UCharSeq.operator->() != NULL)
		{	
			att[i].value  <<= attr_list[i].UCharSeq.in();
			continue;
		}
	}
		
	CORBA::Request_ptr request;
	if (version < 3)
	{
		request = device->_request("write_attributes");
		request->exceptions()->add(Tango::_tc_DevFailed);
	}
	else
	{
		request = device->_request("write_attributes_3");
		request->exceptions()->add(Tango::_tc_MultiDevFailed);
	}
	request->add_in_arg() <<= att;

//
// Send the request 
//

	long id = 0;		
	id = ApiUtil::instance()->get_pasyn_table()->store_request(request,TgRequest::WRITE_ATTR);

	request->send_deferred();
	pasyn_ctr++;
		
	return id;
}

long DeviceProxy::write_attribute_asynch(DeviceAttribute &attr)
{

//
// Reconnect to device in case it is needed
//

	try
	{
		if (connection_state != CONNECTION_OK)
			reconnect(dbase_used);
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
	att.length(1);
	
	att[0].name = attr.name.c_str();
	att[0].quality = attr.quality;
	att[0].time = attr.time;
	att[0].dim_x = attr.dim_x;
	att[0].dim_y = attr.dim_y;
	
	if (attr.LongSeq.operator->() != NULL)
	{
		att[0].value <<= attr.LongSeq.in();	
	}
	else if (attr.ShortSeq.operator->() != NULL)
	{	
		att[0].value <<= attr.ShortSeq.in();
	}	
	else if (attr.DoubleSeq.operator->() != NULL)
	{	
		att[0].value <<= attr.DoubleSeq.in();
	}
	else if (attr.StringSeq.operator->() != NULL)
	{	
		att[0].value  <<= attr.StringSeq.in();
	}
	else if (attr.FloatSeq.operator->() != NULL)
	{	
		att[0].value  <<= attr.FloatSeq.in();
	}
	else if (attr.BooleanSeq.operator->() != NULL)
	{	
		att[0].value  <<= attr.BooleanSeq.in();
	}
	else if (attr.UShortSeq.operator->() != NULL)
	{	
		att[0].value  <<= attr.UShortSeq.in();
	}
	else if (attr.UCharSeq.operator->() != NULL)
	{	
		att[0].value  <<= attr.UCharSeq.in();
	}

	CORBA::Request_ptr request;
	if (version < 3)
	{		
		request = device->_request("write_attributes");
		request->exceptions()->add(Tango::_tc_DevFailed);
	}
	else
	{
		request = device->_request("write_attributes_3");
		request->exceptions()->add(Tango::_tc_MultiDevFailed);
	}	
	request->add_in_arg() <<= att;

//
// Send the request 
//

	long id = 0;		
	id = ApiUtil::instance()->get_pasyn_table()->store_request(request,TgRequest::WRITE_ATTR_SINGLE);

	request->send_deferred();
	pasyn_ctr++;
		
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
#ifndef WIN32
		struct timespec to_wait,inter;
		to_wait.tv_sec = 0;
		to_wait.tv_nsec = 20000000;
#endif				
		int i;
		
		for (i = 0;i < nb;i++)
		{
#ifdef WIN32
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
	}
				
//
// Remove request from request global table.
//

	ApiUtil::instance()->get_pasyn_table()->remove_request(id);
	pasyn_ctr--;	
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
		}
	}
				
//
// Remove request from request global table.
//

	ApiUtil::instance()->get_pasyn_table()->remove_request(id);
	pasyn_ctr--;

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
			CORBA::NamedValue_ptr nv = req_arg->item(0);
			*(nv->value()) >>= att;
						
			char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(tra);
	
			TangoSys_OMemStream desc;
			desc << "Timeout (" << timeout << " mS) exceeded on device " << device_name;
			desc << "\nAttribute(s): ";
			for (unsigned int i = 0;i < att->length();i++)
			{
				desc << (*att)[i].name;
				if (i != (att->length() - 1))
					desc << ", ";
			}
			desc << ends;
			
			ApiUtil::instance()->get_pasyn_table()->remove_request(id);
			pasyn_ctr--;	

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
	
		const Tango::DevFailed *serv_ex;
		Tango::DevFailed ex;
		const Tango::MultiDevFailed *multi_serv_ex;
		Tango::MultiDevFailed m_ex;
		
		if (version < 3)
		{
			unk_ex->exception() >>= serv_ex;
			ex = *serv_ex;
		}
		else
		{
			unk_ex->exception() >>= multi_serv_ex;
			m_ex = *multi_serv_ex;
		}
		
		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::AttributeValueList *att;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		*(nv->value()) >>= att;	
		
		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes_asynch on device " << device_name;
		desc << "\nAttribute(s): ";
		for (unsigned int i = 0;i < att->length();i++)
		{
			desc << (*att)[i].name;
			if (i != (att->length() - 1))
				desc << ", ";
		}
		desc << ends;
		
		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;
						
		if (version < 3)
		{
			Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
						   desc.str(),
						   (const char*)"DeviceProxy::write_attributes_reply()");
		}
		else
		{
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

		connection_state = CONNECTION_NOTOK;
			
//
// Re-throw all CORBA system exceptions
//

		CORBA::NVList_ptr req_arg = req->arguments();
		const Tango::AttributeValueList *att;
		CORBA::NamedValue_ptr nv = req_arg->item(0);
		*(nv->value()) >>= att;

		char *cb_excep_mess = Tango::Except::print_CORBA_SystemException(sys_ex);

		TangoSys_OMemStream desc;
		desc << "Failed to execute write_attributes_asynch on device " << device_name;
		desc << "\nAttribute(s): ";
		for (unsigned int i = 0;i < att->length();i++)
		{
			desc << (*att)[i].name;
			if (i != (att->length() - 1))
				desc << ", ";
		}
		desc << ends;
					
		ApiUtil::instance()->get_pasyn_table()->remove_request(id);
		pasyn_ctr--;

		ApiCommExcept::re_throw_exception(cb_excep_mess,
						  (const char*)"API_CommunicationFailed",
                        			  desc.str(),
						  (const char*)"DeviceProxy::write_attributes_reply()");	
	}	
}

} // End of tango namespace

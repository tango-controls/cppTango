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
	if (version == 2)
	{
		request = device_2->_request("read_attributes_2");
		request->add_in_arg() <<= names;
		request->add_in_arg() <<= source;
	}
	else
	{
		request = device->_request("read_attributes");
		request->add_in_arg() <<= names;
	}
	request->set_return_type(Tango::_tc_AttributeValueList);
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
		CORBA::Any &dii_any = req.request->return_value();
		dii_any >>= received;

		dev_attr->resize(received->length());
			
		const DevVarLongArray *tmp_seq_lo;
		CORBA::Long *tmp_lo;
		const DevVarShortArray *tmp_seq_sh;
		CORBA::Short *tmp_sh;
		const DevVarDoubleArray *tmp_seq_db;
		CORBA::Double *tmp_db;
		const DevVarStringArray *tmp_seq_str;
		char **tmp_str;
		CORBA::ULong max,len;
				
		for (unsigned int i=0; i < received->length(); i++)
		{
			(*dev_attr)[i].name = (*received)[i].name;
			(*dev_attr)[i].quality = (*received)[i].quality;
			(*dev_attr)[i].time = (*received)[i].time;
			(*dev_attr)[i].dim_x = (*received)[i].dim_x;
			(*dev_attr)[i].dim_y = (*received)[i].dim_y;
				
			if ((*dev_attr)[i].quality != Tango::ATTR_INVALID)
			{
				CORBA::TypeCode_var ty = (*received)[i].value.type();
				CORBA::TypeCode_var ty_alias = ty->content_type();
				CORBA::TypeCode_var ty_seq = ty_alias->content_type();		
				switch (ty_seq->kind())		
				{
				case CORBA::tk_long:		
					(*received)[i].value >>= tmp_seq_lo;
					max = tmp_seq_lo->maximum();
					len = tmp_seq_lo->length();
					if (tmp_seq_lo->release() == true)
					{
						tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
						(*dev_attr)[i].LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
					}
					else
					{
						tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
						(*dev_attr)[i].LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
					}
					break;
		
				case CORBA::tk_short:
					(*received)[i].value >>= tmp_seq_sh;
					max = tmp_seq_sh->maximum();
					len = tmp_seq_sh->length();
					if (tmp_seq_sh->release() == true)
					{
						tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
						(*dev_attr)[i].ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
					}
					else
					{
						tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
						(*dev_attr)[i].ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
					}
					break;
		
				case CORBA::tk_double:
					(*received)[i].value >>= tmp_seq_db;
					max = tmp_seq_db->maximum();
					len = tmp_seq_db->length();
					if (tmp_seq_db->release() == true)
					{
						tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
						(*dev_attr)[i].DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
					}
					else
					{
						tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
						(*dev_attr)[i].DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
					}
					break;
		
				case CORBA::tk_string:
					(*received)[i].value >>= tmp_seq_str;
					max = tmp_seq_str->maximum();
					len = tmp_seq_str->length();
					if (tmp_seq_str->release() == true)
					{
						tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
						(*dev_attr)[i].StringSeq = new DevVarStringArray(max,len,tmp_str,true);
					}
					else
					{
						tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
						(*dev_attr)[i].StringSeq = new DevVarStringArray(max,len,tmp_str,false);
					}
					break;
				}
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
		CORBA::Any &dii_any = req.request->return_value();
		dii_any >>= received;
			
		const DevVarLongArray *tmp_seq_lo;
		CORBA::Long *tmp_lo;
		const DevVarShortArray *tmp_seq_sh;
		CORBA::Short *tmp_sh;
		const DevVarDoubleArray *tmp_seq_db;
		CORBA::Double *tmp_db;
		const DevVarStringArray *tmp_seq_str;
		char **tmp_str;
		CORBA::ULong max,len;
				
		dev_attr->name = (*received)[0].name;
		dev_attr->quality = (*received)[0].quality;
		dev_attr->time = (*received)[0].time;
		dev_attr->dim_x = (*received)[0].dim_x;
		dev_attr->dim_y = (*received)[0].dim_y;
				
		if (dev_attr->quality != Tango::ATTR_INVALID)
		{
			CORBA::TypeCode_var ty = (*received)[0].value.type();
			CORBA::TypeCode_var ty_alias = ty->content_type();
			CORBA::TypeCode_var ty_seq = ty_alias->content_type();		
			switch (ty_seq->kind())			
			{
			case CORBA::tk_long:		
				(*received)[0].value >>= tmp_seq_lo;
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
		
			case CORBA::tk_short:
				(*received)[0].value >>= tmp_seq_sh;
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
				(*received)[0].value >>= tmp_seq_db;
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
				(*received)[0].value >>= tmp_seq_str;
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
	CORBA::Any &dii_any = req.request->return_value();
	dii_any >>= received;

	dev_attr->resize(received->length());
			
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	CORBA::ULong max,len;
				
	for (unsigned int i=0; i < received->length(); i++)
	{
		(*dev_attr)[i].name = (*received)[i].name;
		(*dev_attr)[i].quality = (*received)[i].quality;
		(*dev_attr)[i].time = (*received)[i].time;
		(*dev_attr)[i].dim_x = (*received)[i].dim_x;
		(*dev_attr)[i].dim_y = (*received)[i].dim_y;
				
		if ((*dev_attr)[i].quality != Tango::ATTR_INVALID)
		{
			CORBA::TypeCode_var ty = (*received)[i].value.type();
			CORBA::TypeCode_var ty_alias = ty->content_type();
			CORBA::TypeCode_var ty_seq = ty_alias->content_type();		
			switch (ty_seq->kind())		
			{
			case CORBA::tk_long:		
				(*received)[i].value >>= tmp_seq_lo;
				max = tmp_seq_lo->maximum();
				len = tmp_seq_lo->length();
				if (tmp_seq_lo->release() == true)
				{
					tmp_lo = (const_cast<DevVarLongArray *>(tmp_seq_lo))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].LongSeq = new DevVarLongArray(max,len,tmp_lo,true);
				}
				else
				{
					tmp_lo = const_cast<CORBA::Long *>(tmp_seq_lo->get_buffer());
					(*dev_attr)[i].LongSeq = new DevVarLongArray(max,len,tmp_lo,false);
				}
				break;
		
			case CORBA::tk_short:
				(*received)[i].value >>= tmp_seq_sh;
				max = tmp_seq_sh->maximum();
				len = tmp_seq_sh->length();
				if (tmp_seq_sh->release() == true)
				{
					tmp_sh = (const_cast<DevVarShortArray *>(tmp_seq_sh))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].ShortSeq = new DevVarShortArray(max,len,tmp_sh,true);
				}
				else
				{
					tmp_sh = const_cast<CORBA::Short *>(tmp_seq_sh->get_buffer());
					(*dev_attr)[i].ShortSeq = new DevVarShortArray(max,len,tmp_sh,false);
				}
				break;
		
			case CORBA::tk_double:
				(*received)[i].value >>= tmp_seq_db;
				max = tmp_seq_db->maximum();
				len = tmp_seq_db->length();
				if (tmp_seq_db->release() == true)
				{
					tmp_db = (const_cast<DevVarDoubleArray *>(tmp_seq_db))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,true);
				}
				else
				{
					tmp_db = const_cast<CORBA::Double *>(tmp_seq_db->get_buffer());
					(*dev_attr)[i].DoubleSeq = new DevVarDoubleArray(max,len,tmp_db,false);
				}
				break;
		
			case CORBA::tk_string:
				(*received)[i].value >>= tmp_seq_str;
				max = tmp_seq_str->maximum();
				len = tmp_seq_str->length();
				if (tmp_seq_str->release() == true)
				{
					tmp_str = (const_cast<DevVarStringArray *>(tmp_seq_str))->get_buffer((CORBA::Boolean)true);
					(*dev_attr)[i].StringSeq = new DevVarStringArray(max,len,tmp_str,true);
				}
				else
				{
					tmp_str = const_cast<char **>(tmp_seq_str->get_buffer());
					(*dev_attr)[i].StringSeq = new DevVarStringArray(max,len,tmp_str,false);
				}
				break;
			}
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
	CORBA::Any &dii_any = req.request->return_value();
	dii_any >>= received;
			
	const DevVarLongArray *tmp_seq_lo;
	CORBA::Long *tmp_lo;
	const DevVarShortArray *tmp_seq_sh;
	CORBA::Short *tmp_sh;
	const DevVarDoubleArray *tmp_seq_db;
	CORBA::Double *tmp_db;
	const DevVarStringArray *tmp_seq_str;
	char **tmp_str;
	CORBA::ULong max,len;
				
	dev_attr->name = (*received)[0].name;
	dev_attr->quality = (*received)[0].quality;
	dev_attr->time = (*received)[0].time;
	dev_attr->dim_x = (*received)[0].dim_x;
	dev_attr->dim_y = (*received)[0].dim_y;
				
	if (dev_attr->quality != Tango::ATTR_INVALID)
	{
		CORBA::TypeCode_var ty = (*received)[0].value.type();
		CORBA::TypeCode_var ty_alias = ty->content_type();
		CORBA::TypeCode_var ty_seq = ty_alias->content_type();		
		switch (ty_seq->kind())		
		{
		case CORBA::tk_long:		
			(*received)[0].value >>= tmp_seq_lo;
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
		
		case CORBA::tk_short:
			(*received)[0].value >>= tmp_seq_sh;
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
			(*received)[0].value >>= tmp_seq_db;
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
			(*received)[0].value >>= tmp_seq_str;
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
	}
		
	CORBA::Request_ptr request = device->_request("write_attributes");
	request->add_in_arg() <<= att;
	request->exceptions()->add(Tango::_tc_DevFailed);

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
		
	CORBA::Request_ptr request = device->_request("write_attributes");
	request->add_in_arg() <<= att;
	request->exceptions()->add(Tango::_tc_DevFailed);

//
// Send the request 
//

	long id = 0;		
	id = ApiUtil::instance()->get_pasyn_table()->store_request(request,TgRequest::WRITE_ATTR);

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

	if (req.req_type != TgRequest::WRITE_ATTR)
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
		write_attr_except(req.request,id);
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

	if (req.req_type != TgRequest::WRITE_ATTR)
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
			write_attr_except(req.request,id);
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


void DeviceProxy::write_attr_except(CORBA::Request_ptr req,long id)
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
		unk_ex->exception() >>= serv_ex;
		Tango::DevFailed ex(*serv_ex);

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

		Except::re_throw_exception(ex,(const char*)"API_AttributeFailed",
                        		   desc.str(),(const char*)"DeviceProxy::write_attributes_reply()");
					
	
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

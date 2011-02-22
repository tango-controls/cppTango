static const char *RcsId = "$Header$";

//+============================================================================
//
// file :               Device.cpp
//
// description :        C++ source code for the DeviceImpl and DeviceClass
//			classes. These classes
//			are the root class for all derived Device classes.
//			They are abstract classes. The DeviceImpl class is the
//			CORBA servant which is "exported" onto the network and
//			accessed by the client.
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:14  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <new>

#include <basiccommand.h>
#include <blackbox.h>
#include <dserversignal.h>
#include <classattribute.h>


namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::DeviceImpl 
// 
// description : 	constructors for the device_impl class from the class object
//			pointer, the device name, the description field, 
//			the state and the status.
//
// argument : in : 	- cl_ptr : The class object pointer
//			- d_name : The device name 
//			- de : The device description (default to "A TANGO device")
//			- st : The device state (default to UNKNOWN)
//			- sta : The device status (default to "Not initialised")
//
//--------------------------------------------------------------------------

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,const char *d_name,
	               const char *de,Tango::DevState st,const char *sta)
:device_class(cl_ptr),device_name(d_name),desc(de),
 device_state(st),device_status(sta)
{
	version = DevVersion;
	blackbox_depth = 0;

//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name)
:device_class(cl_ptr),device_name(d_name)
{
	desc = "A Tango device";
	device_state = Tango::UNKNOWN;
	device_status = "Not Initialised";

	version = DevVersion;
	blackbox_depth = 0;

//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &description)
:device_class(cl_ptr),device_name(d_name)
{
	desc = description;
	device_state = Tango::UNKNOWN;
	device_status = "Not Initialised";

	version = DevVersion;
	blackbox_depth = 0;

//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
}

DeviceImpl::DeviceImpl(DeviceClass *cl_ptr,string &d_name,string &de,
	               Tango::DevState st,string &sta)
:device_class(cl_ptr),device_name(d_name),desc(de),
 device_state(st),device_status(sta)
{
	version = DevVersion;
	blackbox_depth = 0;

//
// Create the DbDevice object
//

	try
	{
		db_dev = new DbDevice(device_name,Tango::Util::instance()->get_database());
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
	
	get_dev_system_resource();
		
	black_box_create();
	
//
// Create the multi attribute object
//

	try
	{
		dev_attr = new MultiAttribute(device_name,device_class);
	}
	catch (Tango::DevFailed)
	{
		_OB_setRef(0);
		throw;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::~DeviceImpl 
// 
// description : 	Destructor for the device class. It simply frees
//			the memory allocated for the black box object
//
//--------------------------------------------------------------------------

DeviceImpl::~DeviceImpl()
{
	cout4 << "Entering DeviceImpl destructor for device " << device_name << endl;
	
//
// Delete the black box
//

	delete blackbox_ptr;

//
// Delete the DbDevic object
//

	delete db_dev;
		
//
// Unregister the signal from signal handler
//

	DServerSignal::instance()->unregister_dev_signal(this);
	
//
// Delete the multi attribute object
//

	delete dev_attr;

	cout4 << "Leaving DeviceImpl destructor for device " << device_name << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::black_box_create 
// 
// description : 	Private method to create the device black box.
//			The black box depth is a resource with a default
//			value if the resource is not defined
//
//--------------------------------------------------------------------------

void DeviceImpl::black_box_create()
{

//
// If the black box depth object attribute is null, create one with the 
// default depth
//

	try
	{
		if (blackbox_depth == 0)
			blackbox_ptr = new BlackBox();
		else
			blackbox_ptr = new BlackBox(blackbox_depth);
	}
	catch (bad_alloc)
	{
		throw;
	}
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceImpl::get_dev_system_resource()
// 
// description : 	Method to retrieve some basic device resources
//			The resources to be retrived are :
//				- The black box depth
//				- The device description
//
//-----------------------------------------------------------------------------

void DeviceImpl::get_dev_system_resource()
{

//
// Try to retrieve resources for device black box depth and device
// description
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->_UseDb == true)
	{
		DbData db_data;
		
		db_data.push_back(DbDatum("blackbox_depth"));
		db_data.push_back(DbDatum("description"));

		try
		{		
			db_dev->get_property(db_data);
		}
		catch (Tango::DevFailed &)
		{
			TangoSys_OMemStream o;
			o << "Database error while trying to retrieve device prperties for device " << device_name.c_str() << ends;
					
			Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"DeviceImpl::get_dev_system_resource");
		}
		
		if (db_data[0].is_empty() == false)
			db_data[0] >> blackbox_depth;
		if (db_data[1].is_empty() == false)
			db_data[1] >> desc;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::_default_POA
// 
// description : 	Return a pointer to the POA on which the device should
//			be activated. This method is required by CORBA to
//			create a POA with the IMPLICIT_ACTIVATION policy
//
//--------------------------------------------------------------------------

PortableServer::POA_ptr DeviceImpl::_default_POA() 
{
	return Util::instance()->get_root_poa();
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::register_signal
// 
// description : 	Method to register a device on a signal. When the
//			signal is sent to the process, the signal_handler
//			method of this class will be executed
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceImpl::register_signal(long signo)
{
	cout4 << "DeviceImpl::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_dev_signal(signo,this);
		
	cout4 << "Leaving DeviceImpl::register_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::unregister_signal
// 
// description : 	Method to unregister a device on a signal.
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceImpl::unregister_signal(long signo)
{
	cout4 << "DeviceImpl::unregister_signal() arrived for signal " << signo << endl;
	
	DServerSignal::instance()->unregister_dev_signal(signo,this);
	
	cout4 << "Leaving DeviceImpl::unregister_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::signal_handler
// 
// description : 	This is the signal handler for the device. This method
//			is defined as virtual and therefore, can be redefined
//			by DS programmers in their own classes derived from
//			DeviceImpl
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceImpl::signal_handler(long signo)
{
	cout4 << "DeviceImpl::signal_handler() arrived for signal " << signo << endl;
	
	cout4 << "Leaving DeviceImpl::signal_handler method()" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::dev_state
// 
// description : 	The default method called by the DevState command.
//			If the device is ON, this method checks attribute 
//			with a defined alarm and set the state to ALARM if one
//			of these attribute is in alarm. Otherwise, simply
//			returns device state
//
//--------------------------------------------------------------------------

Tango::DevState DeviceImpl::dev_state()
{
	if ((device_state == Tango::ON) ||
	    (device_state == Tango::ALARM))
	{
	
//
// Read the hardware
//

		vector<long> &attr_list = dev_attr->get_alarm_list();
		long nb_wanted_attr = attr_list.size();
		if (nb_wanted_attr != 0)
		{
			read_attr_hardware(attr_list);
	
//
// Set attr value
//

			long i;
			for (i = 0;i < nb_wanted_attr;i++)
			{
				Attribute &att = dev_attr->get_attr_by_ind(attr_list[i]);
				att.wanted_date(false);
				try
				{
					read_attr(att);
				}
				catch (Tango::DevFailed)
				{
					att.wanted_date(true);
					throw;
				}
				att.wanted_date(true);
			}
		
//
// Check alarm level
//

			if (dev_attr->check_alarm() == true)
				device_state = Tango::ALARM;
			
//
// Free the sequence created to store the attribute value
//

			for (i = 0;i < nb_wanted_attr;i++)
			{
				dev_attr->get_attr_by_ind(attr_list[i]).delete_seq();
			}
		}
	}
	return device_state;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::dev_status
// 
// description : 	The default method called by the DevStatus command.
//			If the device is ON, this method add Attribute status
//			for all device attribute in alarm state.
//
//--------------------------------------------------------------------------

Tango::ConstDevString DeviceImpl::dev_status()
{
	if ((device_state == Tango::ALARM) ||
	    (device_state == Tango::ON))
	{
		alarm_status = device_status;
		dev_attr->read_alarm(alarm_status);
		return alarm_status.c_str();
	}
	return device_status.c_str();
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::command_inout 
// 
// description : 	Method called for each command_inout operation executed
//			from any client
//			The call to this method is in a try bloc for the
//			Tango::DevFailed exception (generated by the idl
//			compiler in the tango_skel.cpp file). Therefore, it
//			is not necessary to propagate by re-throw the exceptions
//			thrown by the underlying functions/methods.
//
//--------------------------------------------------------------------------

CORBA::Any *DeviceImpl::command_inout(const char *in_cmd,
				     const CORBA::Any &in_any)
throw (Tango::DevFailed, CORBA::SystemException)
{
	string command(in_cmd);

	cout4 << "DeviceImpl::command_inout(): command received : " << command << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_cmd(in_cmd);

//	
// Execute command
//

	CORBA::Any *out_any = device_class->command_handler(this,command,in_any);
	
//
// Return value to the caller 
//

	cout4 << "DeviceImpl::command_inout(): leaving method for command " << command << endl;	
	return(out_any);
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::name 
// 
// description : 	Method called when a client request the name attribute
//                      This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::name()
throw (CORBA::SystemException)
{
	cout4 << "DeviceImpl::name arrived" << endl;

//
// Record attribute request in black box
//

	blackbox_ptr->insert_attr(Attr_Name);

//
// Return data to caller
//

	cout4 << "Leaving DeviceImpl::name" << endl;				
	return CORBA::string_dup(device_name.c_str());
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::description 
// 
// description : 	Method called when a client request the description
//			attribute
//                      This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::description()
throw (CORBA::SystemException)
{
	cout4 << "DeviceImpl::description arrived" << endl;

//
// Record attribute request in black box
//
	blackbox_ptr->insert_attr(Attr_Description);

//
// Return data to caller
//
	
	cout4 << "Leaving DeviceImpl::description" << endl;			
	return CORBA::string_dup(desc.c_str());
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::state
// 
// description : 	Method called when a client request the state
//			attribute
//
//--------------------------------------------------------------------------

Tango::DevState DeviceImpl::state()
throw (CORBA::SystemException)
{
	cout4 << "DeviceImpl::state (attibute) arrived" << endl;

//
// Record attribute request in black box
//

	blackbox_ptr->insert_attr(Attr_State);

//
// Return data to caller. If the state_cmd throw an exception, catch it and do 
// not re-throw it because we are in a CORBA attribute implementation
//


	Tango::DevState tmp;
	try
	{
		tmp = dev_state();
	}
	catch (Tango::DevFailed &)
	{
	}

	cout4 << "Leaving DeviceImpl::state (attribute)" << endl;			
	return tmp;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::status
// 
// description : 	Method called when a client request the description
//			status
//                      This method is called for a IDL attribute which can
//			not throw exception to client ==> There is no point
//			to check if the allocation done by the string_dup
//			function failed.
//
//--------------------------------------------------------------------------

char *DeviceImpl::status()
throw (CORBA::SystemException)
{
	cout4 << "DeviceImpl::status (attibute) arrived" << endl;

//
// Record attribute request in black box
//

	blackbox_ptr->insert_attr(Attr_Status);

//
// Return data to caller. If the status_cmd method throw exception, catch it
// and forget it because we are in a CORBA attribute implementation
//

	char *tmp;
	try
	{
		tmp = CORBA::string_dup(dev_status());
	}
	catch (Tango::DevFailed e)
	{
	}

	cout4 << "Leaving DeviceImpl::status (attribute)" << endl;		
	return tmp;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::black_box 
// 
// description : 	CORBA operation to read n element(s) of the black-box.
//			This method returns black box element as strings
//
// argument: in :	- n : Number of elt to read
//
//--------------------------------------------------------------------------


Tango::DevVarStringArray* DeviceImpl::black_box(CORBA::Long n)
throw (Tango::DevFailed, CORBA::SystemException)
{

	cout4 << "DeviceImpl::black_box arrived" << endl;
	
	Tango::DevVarStringArray *ret = blackbox_ptr->read((long)n);
	
//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_BlackBox);	

	cout4 << "Leaving DeviceImpl::black_box" << endl;	
	return ret;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::command_list_query
// 
// description : 	CORBA operation to read the device command list.
//			This method returns command info in a sequence of
//			DevCmdInfo
//
//--------------------------------------------------------------------------


Tango::DevCmdInfoList* DeviceImpl::command_list_query()
throw (Tango::DevFailed, CORBA::SystemException)
{

	cout4 << "DeviceImpl::command_list_query arrived" << endl;

//
// Retrive number of command and allocate memory to send back info
//

	long nb_cmd = device_class->get_command_list().size();
	cout4 << nb_cmd << " command(s) for device" << endl;
	Tango::DevCmdInfoList *back;

	try
	{
		back = new Tango::DevCmdInfoList(nb_cmd);
                back->length(nb_cmd);
		
//
// Populate the vector
//

		for (long i = 0;i < nb_cmd;i++)
		{
			Tango::DevCmdInfo tmp;
			tmp.cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
			tmp.cmd_tag = 0;
			tmp.in_type = (long)((device_class->get_command_list())[i]->get_in_type());
			tmp.out_type = (long)((device_class->get_command_list())[i]->get_out_type());
			string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
			if (str_in.size() != 0)
				tmp.in_type_desc = CORBA::string_dup(str_in.c_str());
			else
				tmp.in_type_desc = CORBA::string_dup(DescNotSet);
			string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
			if (str_out.size() != 0)
				tmp.out_type_desc = CORBA::string_dup(str_out.c_str());
			else
				tmp.out_type_desc = CORBA::string_dup(DescNotSet);
			
			(*back)[i] = tmp;
		}
	}	
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::command_list_query");
	}
		
//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command_list);
	
//
// Return to caller
//
				
	cout4 << "Leaving DeviceImpl::command_list_query" << endl;
	return back;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::command_query
// 
// description : 	CORBA operation to read a device command info.
//			This method returns command info for a specific
//			command.
//
//
//--------------------------------------------------------------------------


Tango::DevCmdInfo *DeviceImpl::command_query(const char *command)
throw (Tango::DevFailed, CORBA::SystemException)
{

	cout4 << "DeviceImpl::command_query arrived" << endl;

	Tango::DevCmdInfo *back;
	string cmd(command);

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

	try
	{
		back = new Tango::DevCmdInfo();
	}	
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::command_query");
	}
	
//
// Try to retrieve the command in the command list
//

	long i;	
	long nb_cmd = device_class->get_command_list().size();
	for (i = 0;i < nb_cmd;i++)
	{
		if (device_class->get_command_list()[i]->get_name() == command)
		{
			back->cmd_name = CORBA::string_dup(((device_class->get_command_list())[i]->get_name()).c_str());
			back->cmd_tag = 0;
			back->in_type = (long)((device_class->get_command_list())[i]->get_in_type());
			back->out_type = (long)((device_class->get_command_list())[i]->get_out_type());
			string &str_in = (device_class->get_command_list())[i]->get_in_type_desc();
			if (str_in.size() != 0)
				back->in_type_desc = CORBA::string_dup(str_in.c_str());
			else
				back->in_type_desc = CORBA::string_dup(DescNotSet);
			string &str_out = (device_class->get_command_list())[i]->get_out_type_desc();
			if (str_out.size() != 0)
				back->out_type_desc = CORBA::string_dup(str_out.c_str());
			else
				back->out_type_desc = CORBA::string_dup(DescNotSet);
			break;			
		}
	}
		
	if (i == nb_cmd)
	{
		delete back;
		cout3 << "DeviceImpl::command_query(): operation " << command << " not found" << endl;
		
//		
// throw an exception to client
//

		TangoSys_OMemStream o;
		
		o << "Command " << command << " not found" << ends;
		Except::throw_exception((const char *)"API_CommandNotFound",
				      o.str(),
				      (const char *)"DeviceImpl::command_query");
	}
		
//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Command);
	
//
// Return to caller
//
				
	cout4 << "Leaving DeviceImpl::command_query" << endl;
	return back;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::info
// 
// description : 	CORBA operation to get device info
//
//--------------------------------------------------------------------------


Tango::DevInfo *DeviceImpl::info()
throw (Tango::DevFailed, CORBA::SystemException)
{

	cout4 << "DeviceImpl::info arrived" << endl;

	Tango::DevInfo *back;

//
// Allocate memory for the stucture sent back to caller. The ORB will free it
//

	try
	{
		back = new Tango::DevInfo();
	}	
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::info");
	}

//
// Retrieve server host
//

	Tango::Util *tango_ptr = Tango::Util::instance();
	back->server_host = CORBA::string_dup(tango_ptr->get_host_name().c_str());
		
//
// Fill-in remaining structure fields
//

	back->dev_class = CORBA::string_dup(device_class->get_name().c_str());
	back->server_id = CORBA::string_dup(tango_ptr->get_ds_name().c_str());
	back->server_version = DevVersion;
	back->doc_url = CORBA::string_dup(device_class->get_doc_url().c_str());
		
//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Info);
	
//
// Return to caller
//
				
	cout4 << "Leaving DeviceImpl::info" << endl;
	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::ping
// 
// description : 	CORBA operation to ping if a device to see it is alive
//
//--------------------------------------------------------------------------


void DeviceImpl::ping()
throw (Tango::DevFailed, CORBA::SystemException)
{

	cout4 << "DeviceImpl::ping arrived" << endl;
		
//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Ping);
	
//
// Return to caller
//
				
	cout4 << "Leaving DeviceImpl::ping" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::get_attribute_config
// 
// description : 	CORBA operation to get attribute configuration.
//
// argument: in :	- names: name of attribute(s)
//
// This method returns a pointer to a AttributeConfigList with one AttributeConfig
// structure for each atribute
//
//--------------------------------------------------------------------------

Tango::AttributeConfigList *DeviceImpl::get_attribute_config(const Tango::DevVarStringArray& names)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::get_attribute_config arrived" << endl;

	long nb_attr = names.length();
	Tango::AttributeConfigList *back;
	bool all_attr = false;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Get_Attr_Config);

//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				      (const char *)"The device does not have any attribute",
				      (const char *)"DeviceImpl::get_attribute_config");
	}
	
//
// Check if the caller want to get config for all attribute
//

	string in_name(names[0]);
	if ((nb_attr == 1) && (in_name == AllAttr))
	{
		nb_attr = nb_dev_attr;
		all_attr = true;
	}
		
//
// Allocate memory for the AttributeConfig structures
//

	try
	{
		back = new Tango::AttributeConfigList(nb_attr);
		back->length(nb_attr);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::get_attribute_config");
	}

//
// Fill in these structures
//
		
	for (long i = 0;i < nb_attr;i++)
	{
		try
		{
			if (all_attr == true)
			{
				Attribute &attr = dev_attr->get_attr_by_ind(i);
				attr.get_properties((*back)[i]);
			}
			else
			{
				Attribute &attr = dev_attr->get_attr_by_name(names[i]);
				attr.get_properties((*back)[i]);
			}
		}
		catch (Tango::DevFailed e)
		{
			delete back;
			throw;
		}
	}
		
//
// Return to caller
//

	cout4 << "Leaving DeviceImpl::get_attribute_config" << endl;
	
	return back;	
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::set_attribute_config
// 
// description : 	CORBA operation to set attribute configuration locally
//			and in the Tango database
//
// argument: in :	- new_conf: The new attribute(s) configuration. One
//				    AttributeConfig structure is needed for each
//				    attribute to update
//
//--------------------------------------------------------------------------

void DeviceImpl::set_attribute_config(const Tango::AttributeConfigList& new_conf)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::set_attribute_config arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Set_Attr_Config);

//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				      (const char *)"The device does not have any attribute",
				      (const char *)"DeviceImpl::set_attribute_config");
	}
	

//
// Update attribute config first in database, then locally
//
	
	long nb_attr = new_conf.length();
	long i;

	try
	{	
		for (i = 0;i < nb_attr;i++)
		{
			Attribute &attr = dev_attr->get_attr_by_name(new_conf[i].name);
			attr.upd_database(new_conf[i],device_name);
			attr.set_properties(new_conf[i],device_name);
		}

	}
	catch (Tango::DevFailed &e)
	{
	
//
// Re build the list of "alarmable" attribute
//

		dev_attr->get_alarm_list().clear();
		for (long j = 0;j < nb_dev_attr;j++)
		{
			if (dev_attr->get_attr_by_ind(j).is_alarmed() == true)
			{
				dev_attr->get_alarm_list().push_back(j);
			}
		}		
	
//
// Change the exception reason flag
//

		TangoSys_OMemStream o;
		
		o << e.errors[0].reason;
		if (i != 0)
			o << "\nAll previous attribute(s) have been successfully updated";
		if (i != (nb_attr - 1))
			o << "\nAll remaining attribute(s) have not been updated";
		o << ends;

#ifdef WIN32
		e.errors[0].reason = CORBA::string_dup(o.str().c_str());
#else		
		char *mess = o.str();			
		e.errors[0].reason = CORBA::string_dup(mess);
		delete [] mess;
#endif
		throw;
	}

//
// Re build the list of "alarmable" attribute
//

	dev_attr->get_alarm_list().clear();
	for (i = 0;i < nb_dev_attr;i++)
	{
		if (dev_attr->get_attr_by_ind(i).is_alarmed() == true)
		{
			dev_attr->get_alarm_list().push_back(i);
		}
	}

//
// Return to caller
//

	cout4 << "Leaving DeviceImpl::set_attribute_config" << endl;	
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::read_attributes
// 
// description : 	CORBA operation to read attribute(s) value.
//
// argument: in :	- names: name of attribute(s) to be read
//
// This method returns a pointer to a AttributeConfigList with one AttributeValue
// structure for each atribute. This structure contains the attribute value, the
// value quality and the date.
//
//--------------------------------------------------------------------------

Tango::AttributeValueList *DeviceImpl::read_attributes(const Tango::DevVarStringArray& names)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::read_attributes arrived" << endl;

//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Read_Attr);
//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				      (const char *)"The device does not have any attribute",
				      (const char *)"DeviceImpl::read_attributes");
	}
	
//
// Retrieve index of wanted attributes in the device attribute list and clear 
// their value set flag
//

	long nb_names = names.length();
	vector<long> wanted_attr;
	vector<long> wanted_w_attr;

	long i;
	for (i = 0;i < nb_names;i++)
	{
		long j = dev_attr->get_attr_ind_by_name(names[i]);
		if (dev_attr->get_attr_by_ind(j).get_writable() == Tango::READ_WRITE)
		{		
			wanted_w_attr.push_back(j);
			wanted_attr.push_back(j);
			dev_attr->get_attr_by_ind(wanted_attr[i]).set_value_flag(false);
		}
		else
		{
			if (dev_attr->get_attr_by_ind(j).get_writable() == Tango::WRITE)
			{					
				wanted_w_attr.push_back(j);
			}
			else
			{
				wanted_attr.push_back(j);
				dev_attr->get_attr_by_ind(wanted_attr[i]).set_value_flag(false);
			}
		}		
	}
	long nb_wanted_attr = wanted_attr.size();
	long nb_wanted_w_attr = wanted_w_attr.size();
	
//
// Read the hardware for readable attribute
//

	if (nb_wanted_attr != 0)
		read_attr_hardware(wanted_attr);
	
//
// Set attr value (for readable attribute)
//

	for (i = 0;i < nb_wanted_attr;i++)
	{
		read_attr(dev_attr->get_attr_by_ind(wanted_attr[i]));
	}

//
// Set attr value for writable attribute
//

	for (i = 0;i < nb_wanted_w_attr;i++)
	{
		Tango::AttrWriteType w_type = dev_attr->get_attr_by_ind(wanted_w_attr[i]).get_writable();
		if ((w_type == Tango::READ_WITH_WRITE) ||
		    (w_type == Tango::WRITE)) 
			dev_attr->get_attr_by_ind(wanted_w_attr[i]).set_value();
	}
	
//
// Allocate memory for the AttributeValue structures
//

	Tango::AttributeValueList *back;
	try
	{
		back = new Tango::AttributeValueList(nb_names);
		back->length(nb_names);
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DeviceImpl::read_attributes");
	}
	
//
// Build the sequence returned to caller for readable attributes and check 
// that all the wanted attributes set value has been updated
//

	for (i = 0;i < nb_names;i++)
	{
		Attribute &att = dev_attr->get_attr_by_name(names[i]);
		if (att.get_value_flag() == false)
		{
			delete back;
			for (long j = 0;j < i;j++)
			{
				Attribute &att_del = dev_attr->get_attr_by_ind(wanted_attr[j]);
				att.delete_seq();
			}
			
			TangoSys_OMemStream o;
		
			o << "Read value for attribute ";
			o << att.get_name();
			o << " has not been updated" << ends;
			Except::throw_exception((const char *)"API_AttrValueNotSet",
					      o.str(),
					      (const char *)"DeviceImpl::read_attributes");
		}
		else
		{
			Tango::AttrWriteType w_type = att.get_writable();
			if ((w_type == Tango::READ) ||
			    (w_type == Tango::READ_WRITE) ||
			    (w_type == Tango::READ_WITH_WRITE))
			{
				if ((w_type == Tango::READ_WRITE) ||
				    (w_type == Tango::READ_WITH_WRITE))
					dev_attr->add_write_value(att);
				
				if ((att.is_alarmed() == true) && (att.get_quality() != Tango::ATTR_INVALID))
					att.check_alarm();
			}
			
			(*back)[i].time = att.get_when();
			(*back)[i].quality = att.get_quality();
			(*back)[i].name = CORBA::string_dup(att.get_name().c_str());
			(*back)[i].dim_x = att.get_x();
			(*back)[i].dim_y = att.get_y();

			CORBA::Any &a = (*back)[i].value;
			switch (att.get_data_type())
			{
			case Tango::DEV_SHORT :			
				a <<= att.get_short_value();
				break;
				
			case Tango::DEV_LONG :
				a <<= att.get_long_value();
				break;
				
			case Tango::DEV_DOUBLE :
				a <<= att.get_double_value();
				break;
				
			case Tango::DEV_STRING :
				a <<= att.get_string_value();
				break;
			}
		}
	}
					
//
// Return to caller
//

	cout4 << "Leaving DeviceImpl::read_attributes" << endl;
	return back;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceImpl::write_attributes
// 
// description : 	CORBA operation to write attribute(s) value
//
// argument: in :	- values: The new attribute(s) value to be set.
//
//--------------------------------------------------------------------------

void DeviceImpl::write_attributes(const Tango::AttributeValueList& values)
throw (Tango::DevFailed, CORBA::SystemException)
{
	cout4 << "DeviceImpl::write_attributes arrived" << endl;
	
//
// Record operation request in black box
//

	blackbox_ptr->insert_op(Op_Write_Attr);

//
// Return exception if the device does not have any attribute
//

	long nb_dev_attr = dev_attr->get_attr_nb();
	if (nb_dev_attr == 0)
	{
		Except::throw_exception((const char *)"API_AttrNotFound",
				      (const char *)"The device does not have any attribute",
				      (const char *)"DeviceImpl::write_attributes");
	}
	
//
// Retrieve index of wanted attributes in the device attribute list
//

	long nb_updated_attr = values.length();
	vector<long> updated_attr;

	long i;
	for (i = 0;i < nb_updated_attr;i++)
	{
		updated_attr.push_back(dev_attr->get_attr_ind_by_name(values[i].name));
	}

//
// Check that these attributes are writable
//

	for (i = 0;i < nb_updated_attr;i++)
	{
		if ((dev_attr->get_attr_by_ind(updated_attr[i]).get_writable() == Tango::READ))
		{
			TangoSys_OMemStream o;
		
			o << "Attribute ";
			o << dev_attr->get_attr_by_ind(updated_attr[i]).get_name();
			o << " is not writable" << ends;

			Except::throw_exception((const char *)"API_AttrNotWritable",
					      o.str(),
					      (const char *)"DeviceImpl::write_attributes");
		}
	}

//
// Set attribute internal value
//

	for (i = 0;i < nb_updated_attr;i++)
	{
		try
		{
			dev_attr->get_w_attr_by_ind(updated_attr[i]).set_write_value(values[i].value);
		}
		catch (Tango::DevFailed)
		{
			for (long j = 0;j < i;j++)
				dev_attr->get_w_attr_by_ind(updated_attr[j]).rollback();
				
			throw;	
		}
	}
	
//
// Write the hardware
//

	write_attr_hardware(updated_attr);
	
//
// Return to caller.
//
	
	cout4 << "Leaving DeviceImpl::write_attributes" << endl;						
}
	


//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::DeviceClass(string &s)
// 
// description : 	DeviceClass constructor. Protected method which will 
//			be called automatically by the compiler.
//
//-----------------------------------------------------------------------------

DeviceClass::DeviceClass(string &s):name(s)
{

//
// Create the associated DbClass object
//

	db_class = new DbClass(name,Tango::Util::instance()->get_database());
		
//
// initialise command_list with DevState and DevStatus 
//

	try
	{
		command_list.push_back(new DevStatusCmd("DevStatus",Tango::DEV_VOID,Tango::DEV_STRING));
		command_list[0]->set_out_type_desc("Device status");
		command_list.push_back(new DevStateCmd("DevState",Tango::DEV_VOID,Tango::DEV_STATE));
		command_list[1]->set_out_type_desc("Device state");
		command_list.push_back(new DevRestartCmd("DevRestart",Tango::DEV_VOID,Tango::DEV_VOID));
	}
	catch (bad_alloc)
	{
		if (command_list.empty() == false)
		{
			for(long i = 0;i < command_list.size();i++)
				delete command_list[i];
			command_list.clear();
		}
		throw;
	}

//
// Retrieve basic class resource
//
	
	get_class_system_resource();
	
//
// Create the multi class attribute object
//

	class_attr = new MultiClassAttribute();
		
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::get_class_system_resource(string &s)
// 
// description : 	Method to retrieve some basic class resource(s)
//			The resource to be retrived are :
//				- The class doc URL
//
//-----------------------------------------------------------------------------

void DeviceClass::get_class_system_resource()
{

//
// Try to retrieve the class resource doc_url
//

	Tango::Util *tg = Tango::Util::instance();
	if (tg->_UseDb == true)
	{
		DbData db_data;
		
		db_data.push_back(DbDatum("doc_url"));
		
		try
		{
			db_class->get_property(db_data);
		}
		catch (Tango::DevFailed &)
		{
			TangoSys_OMemStream o;
			o << "Database error while trying to retrive call properties for class " << name.c_str() << ends;
					
			Except::throw_exception((const char *)"API_DatabaseAccess",
				                o.str(),
				                (const char *)"DeviceClass::get_class_system_resource");
		}
		
		if (db_data[0].is_empty() == true)
		{
			cout4 << "doc_url property for class " << name << " is not defined in database" << endl;
			doc_url = DefaultDocUrl;
		}
		else
			db_data[0] >> doc_url;		
	}
	else
	{
		doc_url = DefaultDocUrl;
	}	
}

//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::~DeviceClass(string &s)
// 
// description : 	DeviceClass destructor.
//
//-----------------------------------------------------------------------------

DeviceClass::~DeviceClass()
{

	cout4 << "Entering DeviceClass destructor for class " << name << endl;

//
// Destroy the DbClass object
//

	delete db_class;
		
//
// Destroy the device list
//

	Tango::Util *tg = Tango::Util::instance();
	PortableServer::POA_ptr r_poa = tg->get_root_poa();
	long i;
	
	for (i = 0;i < device_list.size();i++)
	{
	
//
// Deactivate the CORBA object
//

		r_poa->deactivate_object(device_list[i]->get_obj_id());
		
//
// Remove the C++ servant
//

		device_list[i]->_remove_ref();
		
	}
	device_list.clear();
	CORBA::release(r_poa);

//
// Destroy the command list
//

	for (i = 0;i < command_list.size();i++)
		delete command_list[i];
	command_list.clear();

//
// Destroy the MultiClassAttribute object
//

	delete class_attr;
		
//
// Unregister the class from signal handler
//

	DServerSignal::instance()->unregister_class_signal(this);
	
	cout4 << "Leaving DeviceClass destructor for class " << name << endl;
	
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::register_signal
// 
// description : 	Method to register a class on a signal. When the
//			signal is sent to the process, the signal_handler
//			method of this class will be executed
//
// in :			signo : The signal number
//			delayed : The boolean delayed flag
//
//--------------------------------------------------------------------------

void DeviceClass::register_signal(long signo)
{
	cout4 << "DeviceClass::register_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->register_class_signal(signo,this);
		
	cout4 << "Leaving DeviceClass::register_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::unregister_signal
// 
// description : 	Method to unregister a class on a signal.
//
// in :			signo : The signal number
//			delayed : The boolean delayed flag
//
//--------------------------------------------------------------------------

void DeviceClass::unregister_signal(long signo)
{
	cout4 << "DeviceClass::unregister_signal() arrived for signal " << signo << endl;

	DServerSignal::instance()->unregister_class_signal(signo,this);
		
	cout4 << "Leaving DeviceClass::unregister_signal method()" << endl;
}

//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::signal_handler
// 
// description : 	This is the signal handler for the class. This method
//			is defined as virtual and therefore, can be redefined
//			by DS programmers in their own classes derived from
//			DeviceClass class
//
// in :			signo : The signal number
//
//--------------------------------------------------------------------------

void DeviceClass::signal_handler(long signo)
{
	cout4 << "DeviceClass::signal_handler() arrived for signal " << signo << endl;
	
	cout4 << "Leaving DeviceClass::signal_handler method()" << endl;
}


//+-------------------------------------------------------------------------
//
// method : 		DeviceClass::export_device()
// 
// description : 	This method exports a device to the outside world. 
//			This is done by sending its CORBA network parameter
//			(mainly the IOR) to the Tango database
//
// in :			dev : The device to be exported
//			corba_obj_name : The CORBA object name associated
//					 with the device. A default value
//					 is provided. This field is mainly
//					 use for specific device server
//					 like the database device server.
//
//--------------------------------------------------------------------------

void DeviceClass::export_device(DeviceImpl *dev,const char *corba_obj_name)
{
	cout4 << "DeviceClass::export_device() arrived" << endl;

	Device_var d;
	if (Tango::Util::_UseDb == true)
	{
	
// 
// Activate the CORBA object incarnated by the dev C++ object
//

		d = dev->_this();
		dev->set_d_var(Tango::Device::_duplicate(d));

//
// Store the ObjectId (The ObjectId_var type is a typedef of a string_var
// type)
//

		PortableServer::ObjectId_var oid;
		try
		{
			PortableServer::POA_ptr r_poa = Util::instance()->get_root_poa();
			oid = r_poa->reference_to_id(d);
			CORBA::release(r_poa);
		}
		catch (...)
		{
			TangoSys_OMemStream o;
			o << "Cant get CORBA reference Id for device " << dev->get_name() << ends;
			Except::throw_exception((const char *)"API_CantGetDevObjectId",
					      o.str(),
					      (const char *)"DeviceClass::export_device");
		}
		dev->set_obj_id(oid);
	}
	else
	{
	
//
// For server started without db usage (Mostly the database server). In this case,
// it is necessary to create our own CORBA object id and to bind it into the
// OOC Boot Manager for access through a stringified object reference 
// constructed using the corbaloc style
//

		PortableServer::ServantBase_var servant = dev;
		d = dev->_this();
		PortableServer::ObjectId_var id = PortableServer::string_to_ObjectId(corba_obj_name);				

		dev->set_obj_id(id);		
		dev->set_d_var(Tango::Device::_duplicate(d));
		
		OB::BootManager_ptr bo_man = Tango::Util::instance()->get_boot_manager();		
		bo_man->add_binding(id,d);
		CORBA::release(bo_man);
	}
			
//
// Prepare sent parameters and allocate mem for them
//

	if (Tango::Util::_UseDb == true)
	{
		Tango::Util *tg = Tango::Util::instance();
		CORBA::ORB_ptr orb_ptr = tg->get_orb();	
		CORBA::Any send;
		long i;
	
		Tango::DevVarStringArray *dev_param = new Tango::DevVarStringArray(5);
        	dev_param->length(5);
		(*dev_param)[0] = CORBA::string_dup(dev->get_name().c_str());
		(*dev_param)[1] = CORBA::string_dup(orb_ptr->object_to_string(d));
		(*dev_param)[2] = CORBA::string_dup(tg->get_host_name().c_str());
		(*dev_param)[3] = CORBA::string_dup(tg->get_pid_str().c_str());
		(*dev_param)[4] = CORBA::string_dup(tg->get_version_str().c_str());

		for (i = 0;i < 5;i++)
		{
			if ((*dev_param)[i] == NULL)
			{
				delete dev_param;
				CORBA::release(orb_ptr);
				throw bad_alloc();
			}
		}

//
// Call db server
//
	
		send <<= dev_param;								
		CORBA::Any_var received = tg->get_database()->get_dbase()->command_inout("DbExportDevice",send);

		CORBA::release(orb_ptr);
	}		
	cout4 << "Leaving DeviceClass::export_device method()" << endl;
}


//+----------------------------------------------------------------------------
//
// method : 		DeviceClass::command_handler(string &s)
// 
// description : 	Command handler which is called by Device
//			when a command is received. It will check
//			to see if the command is implemented. If
//			so it will test to see if it is allowed
//			in this state. Finally it will execute the
//			command by calling its execute method.
//			If an error occurs it will throw a DevFailed
//			exception.
//
//-----------------------------------------------------------------------------

CORBA::Any *DeviceClass::command_handler(DeviceImpl *device,string &command,const CORBA::Any &in_any)
{	
	CORBA::Any *ret;
	vector<Command *>::iterator i_cmd;

	cout4 << "Entering DeviceClass::command_handler() method" << endl;
	
	for (i_cmd = command_list.begin();i_cmd < command_list.end();i_cmd++)
	{
		if ((*i_cmd)->get_name() == command)
		{
		
//
// Call the always executed method
//

			device->always_executed_hook();

//
// Chaeck if command is allowed
//
			
			if ((*i_cmd)->is_allowed(device,in_any) == false)
			{
				TangoSys_OMemStream o;
				o << "Command " << command << " not allowed when the device is in " << Tango::DevStateName[device->get_state()] << " state"  << ends;
				Except::throw_exception((const char *)"API_CommandNotAllowed",
						      o.str(),
						      (const char *)"DeviceClass::command_handler");
			}

//
// Execute command
//

			ret = (*i_cmd)->execute(device,in_any);
			break;
		}
	}
		
	if (i_cmd == command_list.end())
	{
		cout3 << "DeviceClass::command_handler(): command " << command << " not found" << endl;
		
//		
// throw an exception to client
//

		TangoSys_OMemStream o;
		
		o << "Command " << command << " not found" << ends;
		Except::throw_exception((const char *)"API_CommandNotFound",
				      o.str(),
				      (const char *)"DeviceClass::command_handler");
	}
	
	cout4 << "Leaving DeviceClass::command_handler() method" << endl;	
	return ret;
}

} // End of Tango namespace

static const char *RcsId = "$Header$";

//+============================================================================
//
// file :               BasicCommand.cpp
//
// description :        C++ source code for commands which are automatically
//			installed for every devices
//			Three commands are : 
//				DevState, DevStatus, DevRestart
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:12  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//-============================================================================

#include <tango.h>
#include <basiccommand.h>

namespace Tango
{

//+-------------------------------------------------------------------------
//
// method : 		DevStatusCmd::DevStatusCmd 
// 
// description : 	constructor for Command class Status
//
//--------------------------------------------------------------------------

DevStatusCmd::DevStatusCmd(const char *name,Tango::CmdArgType in,Tango::CmdArgType out)
:Command(name,in,out)
{
}


//+-------------------------------------------------------------------------
//
// method : 		DevStatusCmd::execute 
// 
// description : 	return status as string
//
//--------------------------------------------------------------------------

CORBA::Any *DevStatusCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "DevStatus::execute(): arrived " << endl;

//
// return status string as Any
//

	CORBA::Any *out_any;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevStatus::execute");
	}
	
	(*out_any) <<= device->dev_status();
	
	cout4 << "Leaving DevStatus::execute()" << endl;
	return out_any;

}

//+-------------------------------------------------------------------------
//
// method : 		DevStateCmd::DevStateCmd
// 
// description : 	constructor for Command class State
//
//--------------------------------------------------------------------------

DevStateCmd::DevStateCmd(const char *name,Tango::CmdArgType in, Tango::CmdArgType out)
:Command(name,in,out)
{
}

//+-------------------------------------------------------------------------
//
// method : 		StateCmd::execute 
// 
// description : 	return state as enumerated type
//
//--------------------------------------------------------------------------

CORBA::Any *DevStateCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "DevState::execute(): arrived" << endl;

//
// return state as Any
//

	CORBA::Any *out_any;
	try
	{
		out_any = new CORBA::Any();
	}
	catch (bad_alloc)
	{
		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"DevStatus::execute");
	}
	(*out_any) <<= device->dev_state();
	
	cout4 << "Leaving DevState::execute()" << endl;
	return out_any;
}

//+-------------------------------------------------------------------------
//
// method : 		DevRestartCmd::DevRestartCmd 
// 
// description : 	constructors for Command class Restart
//
//--------------------------------------------------------------------------

DevRestartCmd::DevRestartCmd(const char *name,Tango::CmdArgType in,Tango::CmdArgType out)
:Command(name,in,out)
{
}


//+-------------------------------------------------------------------------
//
// method : 		DevRestartCmd::execute 
// 
// description : 	restart a device without changing its netwok interface
//
//--------------------------------------------------------------------------
static DeviceImpl *dev_to_del;

CORBA::Any *DevRestartCmd::execute(DeviceImpl *device, const CORBA::Any &in_any)
{

	cout4 << "DevRestart::execute(): arrived " << endl;

//
// Get class device list
//

	DeviceClass *dev_cl = device->get_device_class();	
	vector<DeviceImpl *> &dev_list = dev_cl->get_device_list();

//
// Find ourself in the device list
//

	vector<DeviceImpl *>::iterator ite;
	for (ite = dev_list.begin();ite != dev_list.end();ite++)
	{
		if (*ite == device)
			break;
	}
	
//
// Remove ourself from device list
//

	dev_list.erase(ite);

//
// Create the thread and start it
//

	dev_to_del = device;
	JTCThread *t = new KillDevThread;
	
	t->start();	
			
//	
// Leave command
//

	CORBA::Any *ret = return_empty_any("DevRestart");
	return ret;
}


void KillDevThread::run()
{
	cout4 << "In the dev-killer thread !!!" << endl;
	
	sleep(1);
	
//
// Get device name and class pointer
//

	DeviceClass *dev_cl = dev_to_del->get_device_class();	
	Tango::DevVarStringArray name(1);
	name.length(1);
	
	name[0] = dev_to_del->get_name().c_str();
	
//
// Delete the device (deactivate it and remove it)
//

	Tango::Util *tg = Tango::Util::instance();
	PortableServer::POA_ptr r_poa = tg->get_root_poa();
	r_poa->deactivate_object(dev_to_del->get_obj_id());
	dev_to_del->_remove_ref();
	CORBA::release(r_poa);
	
//
// Re-create device
//

	dev_cl->device_factory(&name);

}


} // End of Tango namespace

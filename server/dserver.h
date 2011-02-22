//=============================================================================
//
// file :               DServer.h
//
// description :        Include for the DServer class. This class implements
//                      all the commands which are available for device
//			of the DServer class. There is one device of the
//			DServer class for each device server process
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:41  taurel
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
//=============================================================================

#ifndef _DSERVER_H
#define _DSERVER_H

#include <tango.h>

namespace Tango
{

//=============================================================================
//
//			The DServer class
//
// description :	Class to implement all data members and commands for 
//			device of the DServer class
//
//=============================================================================


class DServer: public DeviceImpl
{
public :
	DServer(DeviceClass *,const char *,const char *,
		Tango::DevState,const char *);
	~DServer();
	
	Tango::DevVarStringArray *query_class();
	Tango::DevVarStringArray *query_device();
	void kill();
		
	string &get_process_name() {return process_name;}
	string &get_personal_name() {return instance_name;}
	string &get_instance_name() {return instance_name;}
	string &get_full_name() {return full_name;}
		
	vector<DeviceClass *> &get_class_list() {return class_list;}
	virtual void init_device();

protected :
	string			process_name;
	string			instance_name;
	string			full_name;
	
	vector<DeviceClass *>	class_list;
	
private:
	void class_factory();
	void add_class(DeviceClass *);
	bool			from_constructor;	
};

class KillThread: public JTCThread
{
public:

	virtual void run();
};

} // End of namespace Tango

#endif

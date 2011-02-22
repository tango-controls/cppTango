//=============================================================================
//
// file :               auto_tango_monitor.h
//
// description :        Include file for two utility classes related
//			to monitor  
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _AUTO_TANGO_MONITOR_H
#define _AUTO_TANGO_MONITOR_H


namespace Tango
{
	
//=============================================================================
//
//			The AutoTangoMonitor class
//
//
// description :	This class is only a helper class used to get a
//			TangoMonitor object during its construction and to 
//			it during its destruction. It makes developper life
//			easier.
//
//=============================================================================

class AutoTangoMonitor
{
public:
	AutoTangoMonitor(Tango::DeviceImpl *dev, bool force = false)
	{
		SerialModel ser = Util::instance()->get_serial_model();

		switch(ser)
		{
		case NO_SYNC:
			if (force == true)
			{
				mon = &(dev->ext->only_one);
			}
			else
				mon = NULL;
			break;			

		case BY_DEVICE:
			mon = &(dev->ext->only_one);
			break;

		case BY_CLASS:
			mon = &(dev->device_class->ext->only_one);
			break;

		case BY_PROCESS:
			mon = &(Util::instance()->ext->only_one);
			break;
		}
		if (mon)
			mon->get_monitor();

	}

	AutoTangoMonitor(Tango::DeviceClass *dev_cl)
	{
		SerialModel ser = Util::instance()->get_serial_model();

		switch(ser)
		{
		case NO_SYNC:
		case BY_DEVICE:
			mon = NULL;
			break;
					
		case BY_CLASS:
			mon = &(dev_cl->ext->only_one);
			mon->get_monitor();
			break;
		
		case BY_PROCESS:
			mon = &(Util::instance()->ext->only_one);
			mon->get_monitor();
			break;
		}
	}

	AutoTangoMonitor(Tango::TangoMonitor *m):mon(m)
	{
		if (mon)
			mon->get_monitor();
	}
		
	~AutoTangoMonitor() {if (mon)mon->rel_monitor();}

private:
	TangoMonitor *mon;
};

//=============================================================================
//
//			The NoSyncModelTangoMonitor class
//
//
// description :	This class is only a helper class used to get a
//			TangoMonitor object during its construction and to 
//			it during its destruction only if the device server
//			process is in NO_SYNC synchronisation model
//
//=============================================================================

class NoSyncModelTangoMonitor
{
public:
	NoSyncModelTangoMonitor(Tango::DeviceImpl *dev)
	{
		SerialModel ser = Util::instance()->get_serial_model();
		if (ser == NO_SYNC)
		{
			mon = &(dev->ext->only_one);
			mon->get_monitor();
		}
		else
			mon = NULL;
	}
	
	~NoSyncModelTangoMonitor() {if (mon)mon->rel_monitor();}

private:
	TangoMonitor *mon;
};


} // End of Tango namespace
		     
#endif /* AUTO_TANGO_MONITOR */

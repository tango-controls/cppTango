/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout

bool verbose = false;

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if ((argc == 1) || (argc > 3))
	{
		cout << "usage: %s device [-v] " << endl;
		exit(-1);
	}

	string device_name = argv[1];
	
	if (argc == 3)
	{
		if (strcmp(argv[2],"-v") == 0)
			verbose = true;
	}	

	try 
	{
		device = new DeviceProxy(device_name);
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;


//**************************************************************************
//
//			Check that state and status are defined as attr.
//
//**************************************************************************



	try
	{
	
		string att_name("Short_spec_attr_rw");
			
// Set attribute rds property (delta_t and delta_val)

		DbAttribute dba(att_name,device_name);
		DbDatum att_na("Short_spec_attr_rw");
		DbDatum dt("delta_t");
		DbDatum dv("delta_val");
		att_na << (short)2;
		dt << (long)1000;
		dv << (short)2;
		
		DbData db;
		db.push_back(att_na);
		db.push_back(dt);
		db.push_back(dv);
		
		dba.put_property(db);		

// Restart the device

		string adm_name = device->adm_name();
		DeviceProxy adm_dev(adm_name);

		DeviceData dd;
		dd << device_name;
#ifdef VALGRIND
		adm_dev.set_timeout_millis(15000);
#endif		
		adm_dev.command_inout("DevRestart",dd);

#ifdef WIN32
		Tango_sleep(2);
#else		
		struct timespec sle;
		sle.tv_sec = 1;
		sle.tv_nsec = 500000000;
		nanosleep(&sle,NULL);
#endif
		
		delete device;
		device = new DeviceProxy(device_name);
				
// Write the attribute without setting the rds

		vector<short> vs_w;
		vector<short> vs;
		vs_w.push_back(7);
		vs_w.push_back(8);
		DeviceAttribute da_w(att_name,vs_w);
		device->write_attribute(da_w);

		DeviceAttribute da;		
		da = device->read_attribute(att_name);
		da >> vs;
		
		int i;
		for (i = 0;i < vs.size();i++)
			coutv << "Attribute vector " << i << " = " << vs[i] << endl;

// Get device state and status

		DevState sta;
		sta = device->state();
		coutv << "State = " << DevStateName[sta] << endl;
				
		string status;
		status = device->status();
		coutv << "Status = " << status << endl;
		
		string::size_type pos;
		pos = status.find("ON state");
		
		assert (sta == Tango::ON);
		assert (pos != string::npos);

#ifdef WIN32
		Tango_sleep(2);
#else		
		sle.tv_sec = 1;
		sle.tv_nsec = 200000000;
		nanosleep(&sle,NULL);
#endif		

		sta = device->state();
		coutv << "State = " << DevStateName[sta] << endl;
				
		status = device->status();
		coutv << "Status = " << status << endl;
		
		pos = status.find("ON state");
		
		assert (sta == Tango::ON);
		assert (pos != string::npos);

		cout << "   Write without setting RDS --> OK" << endl;
		
// Write the attribute with setting the rds

		vs_w.clear();
		vs_w.push_back(7);
		vs_w.push_back(25);
		DeviceAttribute da_w2(att_name,vs_w);
		device->write_attribute(da_w2);

		da = device->read_attribute(att_name);
		da >> vs;
		
		for (i = 0;i < vs.size();i++)
			coutv << "Attribute vector " << i << " = " << vs[i] << endl;

// Get device state and status

		sta = device->state();
		coutv << "State = " << DevStateName[sta] << endl;
				
		status = device->status();
		coutv << "Status = " << status << endl;
		
		pos = status.find("ON state");
		
		assert (sta == Tango::ON);
		assert (pos != string::npos);

#ifdef WIN32
		Tango_sleep(2);
#else		
		sle.tv_sec = 1;
		sle.tv_nsec = 200000000;
		nanosleep(&sle,NULL);
#endif		

		sta = device->state();
		coutv << "State = " << DevStateName[sta] << endl;
		assert (sta == Tango::ALARM);
				
		status = device->status();
		coutv << "Status = " << status << endl;
		
		pos = status.find("ALARM state");
		assert (pos != string::npos);
		pos = status.find("RDS");
		assert (pos != string::npos);
		pos = status.find("Short_spec_attr_rw");
		assert (pos != string::npos);		

		cout << "   Write with setting RDS --> OK" << endl;

// Write the attribute without setting the rds

		vs_w.clear();
		vs_w.push_back(7);
		vs_w.push_back(8);
		DeviceAttribute da_w3(att_name,vs_w);
		device->write_attribute(da_w3);

		da = device->read_attribute(att_name);
		da >> vs;
		
		for (i = 0;i < vs.size();i++)
			coutv << "Attribute vector " << i << " = " << vs[i] << endl;

// Get device state and status

		sta = device->state();
		coutv << "State = " << DevStateName[sta] << endl;
				
		status = device->status();
		coutv << "Status = " << status << endl;
		
		pos = status.find("ON state");
		
		assert (sta == Tango::ON);
		assert (pos != string::npos);

#ifdef WIN32
		Tango_sleep(2);
#else		
		sle.tv_sec = 1;
		sle.tv_nsec = 200000000;
		nanosleep(&sle,NULL);
#endif		

		sta = device->state();
		coutv << "State = " << DevStateName[sta] << endl;
				
		status = device->status();
		coutv << "Status = " << status << endl;
		
		pos = status.find("ON state");
		
		assert (sta == Tango::ON);
		assert (pos != string::npos);

		cout << "   Write without setting RDS --> OK" << endl;
									
// Remove rds property (delta_t and delta_val)
		
		dba.delete_property(db);
							
	}
	catch (Tango::DevFailed &e)
	{
              	Except::print_exception(e);
		exit(1);
        }
	
	delete device;		
	return 0;
	
}

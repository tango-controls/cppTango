;/* 
 * example of a client using the TANGO database api.
 */

#include <errno.h>
#include <stdlib.h>
#include <tango.h>
#include <dbapi.h>
#ifndef _HPUX_SOURCE
#include <iomanip>
#else
#include <iomanip.h>
#endif /* !_HPUX_SOURCE */

using namespace Tango;
using namespace CORBA;

int main(int argc, char **argv)
{
	Database *dbase;
	DbDatum sl_parity("parity"), sl_baudrate("baud_rate"), sl_stopbits("stopbits");
	DbData sl_props_in, sl_props_out, ct_props_out;
	DbDevInfo dev_info;
	DbDevExportInfo export_info;
	DbDevImportInfo import_info;
	vector<short> stopbits;
	CORBA::ORB *my_orb;
	
	double offseta;
	double baudrate=19200.192;
	string parity("even");
	string device_name("my/serial/device");
	const char *parity_c_str;
	DbDatum export_device_datum;
	vector<string> export_device_list;

	my_orb = ORB_init(argc, argv);
//	dbase = new Database("tango", 20000, my_orb);
	dbase = new Database();

	try {
		export_device_datum = dbase->get_device_exported("*");
		export_device_datum >> export_device_list;
		for (int i=0; i<export_device_list.size(); i++)
		{
			cout << "exported device [" << i << "] " << export_device_list[i] << endl;
		}

		cout << "dbase->get_info() : " << dbase->get_info() << endl;

		cout << "sl_parity.is_empty() " << sl_parity.is_empty() << endl;
		sl_parity << parity;
		sl_baudrate << baudrate;
		stopbits.resize(1);
		stopbits[0] = 01;
//		stopbits[1] = 1;
//		stopbits[2] = 2;
		sl_stopbits << stopbits;
		sl_props_in.push_back(sl_parity);
		sl_props_in.push_back(sl_baudrate);	
		sl_props_in.push_back(sl_stopbits);	
		cout << "dbase->put_device_property() called" << endl;
		dbase->put_device_property(device_name, sl_props_in);

		sl_props_out.push_back(DbDatum("blabla"));
//		sl_props_out[0].exceptions(DbDatum::isempty_flag);
		sl_props_out.push_back(DbDatum("baud_rate"));
		sl_props_out.push_back(DbDatum("stopbits"));
		cout << "dbase->get_device_property(" << device_name << ") called" << endl;
		dbase->get_device_property(device_name, sl_props_out);
		cout << device_name;
		sl_props_out[0] >> parity;
		sl_props_out[0] >> parity_c_str;
		cout << " parity = " << parity << " (string) " << parity_c_str << " (char*) ,";
		sl_props_out[1] >> baudrate;
#ifndef _HPUX_SOURCE
		cout << std::setprecision(15) << " baudrate = " << baudrate << ", ";
#else
		cout << setprecision(15) << " baudrate = " << baudrate << ", ";
#endif 
		sl_props_out[2] >> stopbits;
		cout << " stopbits = " << stopbits.size() << endl;
		for (int i=0; i< stopbits.size(); i++)
		{
			cout << " stopbits[" << i << "] " << stopbits[i] << endl;
		}

		cout << "sl_props_out[0].is_empty() " << sl_props_out[0].is_empty() << endl;

		ct_props_out.push_back(DbDatum("offseta"));
		dbase->get_device_property("fe/ct/1", ct_props_out);
		if (!ct_props_out[0].is_empty()) 
		{
			ct_props_out[0] >> offseta;
			cout << " offseta = " << offseta << endl;
		}
		else
		{
			cout << "ct offseta property is empty !" << endl;
		}

/*
		dev_info.name = device_name;
		dev_info._class = "OPCplc";
		dev_info.server = "OPCServer/Eurotherm";
		cout << "dbase->add_device(" << device_name << ") called" << endl;
		dbase->add_device(dev_info);
		export_info.name = device_name;
		export_info.ior = "ior";
		export_info.host = "dumela";
		export_info.version = "1.0";
		export_info.pid = 1;//getpid();
		cout << "dbase->export_device(" << device_name << ") called" << endl;
		dbase->export_device(export_info);
		{
		cout << "dbase->import_device(" << device_name << ") called" << endl;
		import_info = dbase->import_device(device_name);
		//sleep(1);
		}
		cout << "info : name " << import_info.name << ", ";
		cout << "exported " << import_info.exported << ", ";
		cout << "ior " << import_info.ior << ", ";
		cout << "version " << import_info.version << ", ";
		cout << endl;
	cout << "shutdown orb ..." << endl;
	my_orb = dbase->get_orb();
 	my_orb->shutdown(false);
	CORBA::release(my_orb);
	CORBA::release(my_orb);
	CORBA::release(my_orb);
 */
	} 
	catch (DevFailed &e)
        {
		delete dbase;
                Except::print_exception(e);
                exit(-1);
        } 
	delete dbase;
}

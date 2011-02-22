/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device1,*device2,*device3;
	
	if (argc != 5)
	{
		cout << "usage: %s device1 device2 device3 loop" << endl;
		exit(-1);
	}

	string device_name1 = argv[1];
	string device_name2 = argv[2];
	string device_name3 = argv[3];
	long loop = atoi(argv[4]);

//
// First device
//

	for (int k = 0;k < loop;k++)
	{
		try 
		{
			device1 = new DeviceProxy(device_name1);
			cout << endl << "new DeviceProxy(" << device1->name() << ") returned" << endl << endl;
			
			device1->ping();
			cout << "Ping on device " << device1->name() << " successfull" << endl;
		}
       	 	catch (CORBA::Exception &e)
       	 	{
              		Except::print_exception(e);
			exit(1);
        	}

//
// Second device
//
	
		try 
		{
			device2 = new DeviceProxy(device_name2);
			cout << endl << "new DeviceProxy(" << device2->name() << ") returned" << endl << endl;
			
			device2->ping();
			cout << "Ping on device " << device2->name() << " successfull" << endl;
		}
        	catch (CORBA::Exception &e)
        	{
              		Except::print_exception(e);
			exit(1);
        	}

//
// Third device
//
	
		try 
		{
			device3 = new DeviceProxy(device_name3);
			cout << endl << "new DeviceProxy(" << device3->name() << ") returned" << endl << endl;
			
			device3->ping();
			cout << "Ping on device " << device3->name() << " successfull" << endl;
		}
        	catch (CORBA::Exception &e)
        	{
              		Except::print_exception(e);
			exit(1);
        	}

//
// print db object 
//

		vector<Database *> dbs = ApiUtil::instance()->get_db_vect();
		cout << "Db objects number = " << dbs.size() << endl;
		for (int i = 0;i < dbs.size();i++)
		{
			cout << "database host = " << dbs[i]->get_db_host() << endl;
			cout << "database port = " << dbs[i]->get_db_port() << endl;
		}
	
		delete device1;
		delete device2;
		delete device3;
	}

	return 0;
}

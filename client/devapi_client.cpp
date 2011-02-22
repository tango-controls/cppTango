/* 
 * example of a client using the TANGO device api.
 */

#include <errno.h>
#include <stdlib.h>
#include <tango.h>
#include <devapi.h>
#ifndef _HPUX_SOURCE
#include <iomanip>
#else
#include <iomanip.h>
#endif /* !_HPUX_SOURCE */
#include <time.h>
#include <signal.h>

using namespace Tango;
using namespace CORBA;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	char *device_name;
	string cmd;
	DeviceData data_out;
	string my_string;
	vector<string> string_array;
	vector<double> double_array;
	DevCommandInfoList command_info_list;
	
	if (argc < 3)
	{
		cout << "usage: %s device command" << endl;
		exit(-1);
	}

	device_name = argv[1];
	cmd = argv[2];


        try
        {
		
		device = new DeviceProxy(device_name);
		cout << "new DeviceProxy() returned" << endl;

		for (int i=0;i<1000;i++)
		{

			command_info_list = device->command_list_query();

			for (int i=0; i<command_info_list.size(); i++)
			{
				cout << command_info_list[i].cmd_name << " ";
				cout << command_info_list[i].cmd_tag << " ";
				cout << command_info_list[i].in_type << " ";
				cout << command_info_list[i].out_type << " ";
				cout << command_info_list[i].in_type_desc << " ";
				cout << command_info_list[i].out_type_desc << " ";
				cout << endl;
			}
/*
                	data_out = device->command_inout(cmd);

			cout << "command_inout(" << cmd << ") loop " << i << " returned : " << endl ;

			data_out >> my_string;
			cout << my_string;
			data_out >> double_array;

			for (int i=0; i<double_array.size(); i++)
			{
				cout << " " << double_array[i];
			}
 */
			cout << endl;
			sleep(1);
		}
        }
        catch (CORBA::Exception &e)
        {
              Except::print_exception(e);
        }

        CORBA::ORB *orb = device->get_orb();
	/*
	orb->shutdown(false);
	CORBA::release(orb);
	 */
	orb->destroy();
}

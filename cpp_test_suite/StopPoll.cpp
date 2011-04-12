/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>


using namespace Tango;
using namespace std;

void stop_poll_att_no_except(DeviceProxy *,const char *);
void stop_poll_cmd_no_except(DeviceProxy *,const char *);


int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc != 2)
	{
		cout << "usage: %s device" << endl;
		exit(-1);
	}

	string device_name = argv[1];

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
	
	stop_poll_cmd_no_except(device,"IOPollStr1");
	stop_poll_cmd_no_except(device,"IOPollArray2");
	stop_poll_cmd_no_except(device,"IOExcept");
	stop_poll_cmd_no_except(device,"State");
	stop_poll_cmd_no_except(device,"Status");
	
	stop_poll_att_no_except(device,"PollLong_attr");
	stop_poll_att_no_except(device,"PollString_spec_attr");
	stop_poll_att_no_except(device,"attr_wrong_type");
	
	stop_poll_att_no_except(device,"Long64_attr_rw");
	stop_poll_att_no_except(device,"ULong_spec_attr_rw");
	stop_poll_att_no_except(device,"ULong64_attr_rw");
	stop_poll_att_no_except(device,"State_spec_attr_rw");
	stop_poll_att_no_except(device,"Encoded_attr");

	stop_poll_att_no_except(device,"event_change_tst");
	stop_poll_att_no_except(device,"event64_change_tst");
	stop_poll_att_no_except(device,"short_attr");
	stop_poll_att_no_except(device,"slow_actuator");
	stop_poll_att_no_except(device,"fast_actuator");
		
	delete device;	
}

void stop_poll_att_no_except(DeviceProxy *dev,const char *att_name)
{
	try
	{
		dev->stop_poll_attribute(att_name);
	}
	catch(Tango::DevFailed &) {}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
}

void stop_poll_cmd_no_except(DeviceProxy *dev,const char *cmd_name)
{
	try
	{
		dev->stop_poll_command(cmd_name);
	}
	catch(Tango::DevFailed &) {}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
}

#include <tango.h>

using namespace std;

int main(int argc,char *argv[])
{
	if (argc != 2)
	{
		cerr << "Usage: unlock <device name>" << endl;
		exit(-1);
	}

	try
	{
		string dev_name(argv[1]);

		Tango::DeviceProxy dev(argv[1]);
		string admin_dev_name = dev.adm_name();
		Tango::DeviceProxy admin_dev(admin_dev_name);

		Tango::DeviceData d_in;
		vector<string> vs;
		vs.push_back(dev_name);
		vector<Tango::DevLong> vl;
		vl.push_back(1L);
		d_in.insert(vl,vs);

		admin_dev.command_inout("UnLockDevice",d_in);
	}
	catch (Tango::DevFailed &e)
	{
		Tango::Except::print_exception(e);
	}
}

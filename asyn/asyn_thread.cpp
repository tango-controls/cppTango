/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout
		    
using namespace Tango;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	bool verbose = false;
	
	if (argc == 1)
	{
		cout << "usage: %s device [-v]" << endl;
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

	
	try
	{
		ApiUtil *au = ApiUtil::instance();

		Tango::cb_sub_model mode = au->get_asynch_cb_sub_model();
		assert (mode == PULL_CALLBACK);
			
//
// Start callback thread
//

		char key;		
		cout << "Hit any key ";
		cin >> key;
		
		au->set_asynch_cb_sub_model(PUSH_CALLBACK);

		cout << "Hit any key ";
		cin >> key;
		
		au->set_asynch_cb_sub_model(PULL_CALLBACK);
		
		cout << "Hit any key ";
		cin >> key;
		
		au->set_asynch_cb_sub_model(PUSH_CALLBACK);

		cout << "Hit any key ";
		cin >> key;

		au->set_asynch_cb_sub_model(PULL_CALLBACK);
		
		cout << "Hit any key ";
		cin >> key;		
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	catch (CORBA::Exception &ex)
	{
		Except::print_exception(ex);
		exit(-1);
	}

		
	delete device;
	
	return 0;
}

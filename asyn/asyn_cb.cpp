/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#define	coutv	if (verbose == true) cout

using namespace Tango;

bool verbose = false;

class MyCallBack: public CallBack
{
public:
	MyCallBack():cb_executed(0) {};
	
	virtual void cmd_ended(CmdDoneEvent *);
	
	long cb_executed;
};

void MyCallBack::cmd_ended(CmdDoneEvent *cmd)
{
	coutv << "In cmd_ended method for device " << cmd->device->dev_name() << endl;
	coutv << "Command = " << cmd->cmd_name << endl;
	short l;
	cmd->argout >> l;
	coutv << "Command result = " << l << endl;
	
	cb_executed++;
}		    

int main(int argc, char **argv)
{
	DeviceProxy *device,*device2;
	
	if (argc == 2)
	{
		if (strcmp(argv[1],"-v") == 0)
			verbose = true;
	}

	try 
	{
		device = new DeviceProxy("dev/test/10");
		device2 = new DeviceProxy("dev/test/11");
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }


	MyCallBack cb_dev1,cb_dev2;	
	try
	{
				
// Send commands to check asynchronous callbacks

		DeviceData din,dout;
		vector<short> send;
		send.push_back(4);
		send.push_back(1);
		din << send;

		device2->command_inout_asynch("IOShortSleep",din,cb_dev2);
		device->command_inout_asynch("IOShortSleep",din,cb_dev1);
		device->command_inout_asynch("IOShortSleep",din,cb_dev1);

//
// Wait for replies
//

		coutv << "Waiting for replies" << endl;
		Tango_sleep(5);
		

		long nb_replies;
		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request arrived" << endl;
		assert ( nb_replies == 3 );

//
// Fire callbacks only for one device
//
		
		device->get_asynch_replies();
		
		assert (cb_dev1.cb_executed == 2);
		assert (cb_dev2.cb_executed == 0);

		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request arrived" << endl;

//
// Fire callback for the second device
//
		
		device2->get_asynch_replies();
		
		assert (cb_dev1.cb_executed == 2);
		assert (cb_dev2.cb_executed == 1);
		
		cout << "   Callback fired by device --> OK" << endl;
					
		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request there" << endl;

//
// Check get_asynch_replies without any replies
//
		
		device2->get_asynch_replies();
		device->get_asynch_replies();
		assert (cb_dev1.cb_executed == 2);
		assert (cb_dev2.cb_executed == 1);
		
		cout << "   Fire callabck without replies --> OK" << endl;

//
// Send commands
//
		
		device2->command_inout_asynch("IOShortSleep",din,cb_dev2);
		device->command_inout_asynch("IOShortSleep",din,cb_dev1);
		device->command_inout_asynch("IOShortSleep",din,cb_dev1);

		coutv << "Waiting for replies" << endl;			
		Tango_sleep(5);
		
		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request there" << endl;
		assert (nb_replies == 3);

//
// Fire callbacks for all devices
//
		
		ApiUtil::instance()->get_asynch_replies();
		
		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request there" << endl;
		assert (nb_replies == 0);
		assert (cb_dev1.cb_executed == 4);
		assert (cb_dev2.cb_executed == 2);
		
		cout << "   Fire all callbacks at once --> OK" << endl;

//
// Send a request in polling mode
//
								
		device->command_inout_asynch("IOShortSleep",din);
		
		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request there" << endl;
		assert (nb_replies == 0);
		nb_replies = ApiUtil::instance()->pending_asynch_call(POLLING);
		coutv << nb_replies << " request there (polling)" << endl;
		assert (nb_replies == 1);

		coutv << "Waiting for replies" << endl;
		Tango_sleep(3);
		
		ApiUtil::instance()->get_asynch_replies();

		nb_replies = ApiUtil::instance()->pending_asynch_call(CALL_BACK);
		coutv << nb_replies << " request there" << endl;
		assert (nb_replies == 0);
		nb_replies = ApiUtil::instance()->pending_asynch_call(POLLING);
		coutv << nb_replies << " request there (polling)" << endl;
		assert (nb_replies == 1);
		cout << "   No pb when mixing polling and callback mode --> OK" << endl;
				
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
	delete device2;
	
	return 0;
}

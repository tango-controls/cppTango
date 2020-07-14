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
	MyCallBack():cb_executed(0),to(false),cmd_failed(false) {};

	virtual void cmd_ended(CmdDoneEvent *);

	long cb_executed;
	short l;
	bool to;
	bool cmd_failed;
};

void MyCallBack::cmd_ended(CmdDoneEvent *cmd)
{
	coutv << "In cmd_ended method for device " << cmd->device->dev_name() << std::endl;
	coutv << "Command = " << cmd->cmd_name << std::endl;

	if (cmd->errors.length() == 0)
	{
		cmd->argout >> l;
		coutv << "Command result = " << l << std::endl;
	}
	else
	{
		long nb_err = cmd->errors.length();
		coutv << "Command returns error" << std::endl;
		coutv << "error length = " << nb_err << std::endl;
		for (int i = 0;i < nb_err;i++)
			coutv << "error[" << i << "].reason = " << cmd->errors[i].reason << std::endl;
		if (strcmp(cmd->errors[nb_err - 1].reason,API_DeviceTimedOut) == 0)
		{
			to = true;
			coutv << "Timeout error" << std::endl;
		}
		else if (strcmp(cmd->errors[nb_err - 1].reason,API_CommandFailed) == 0)
		{
			cmd_failed = true;
			coutv << "Command failed error" << std::endl;
		}
		else
			coutv << "Unknown error" << std::endl;
	}

	cb_executed++;
}


int main(int argc, char **argv)
{
	DeviceProxy *device;

	if (argc == 1)
	{
		cout << "usage: %s device [-v]" << std::endl;
		exit(-1);
	}

	std::string device_name = argv[1];

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


// Send a command to check asynchronous callback

		long nb_not_arrived = 0;

		DeviceData din,dout;
		std::vector<short> send;
		send.push_back(4);
		send.push_back(2);
		din << send;

		MyCallBack cb;

		device->command_inout_asynch("IOShortSleep",din,cb);

// Check if command returned

		while (cb.cb_executed == 0)
		{
			coutv << "Command not yet arrived" << std::endl;
			nb_not_arrived++;
			Tango_sleep(1);

			device->get_asynch_replies();
		}
		assert ( nb_not_arrived >= 2);
		assert ( cb.cb_executed == 1);
		assert ( cb.l == 8);

		cout << "   Aynchronous command_inout in callback mode --> OK" << std::endl;

// Send a command to check callback with blocking with timeout

		cb.cb_executed = 0;
		cb.l = 0;
		device->command_inout_asynch("IOShortSleep",din,cb);

// Check if command returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(200);
			}
			catch (AsynReplyNotArrived&)
			{
				coutv << "Command not yet arrived" << std::endl;
				nb_not_arrived++;
			}
		}
		assert ( nb_not_arrived >= 4);
		assert ( cb.cb_executed == 1);
		assert ( cb.l == 8);

		cout << "   Aynchronous command_inout in blocking mode with call timeout --> OK" << std::endl;

// Send a command to check callback with blocking

		cb.cb_executed = 0;
		cb.l = 0;

		device->command_inout_asynch("IOShortSleep",din,cb);

// Check if command returned

		device->get_asynch_replies(0);

		assert( cb.l == 8 );
		assert( cb.cb_executed == 1);

		cout << "   Aynchronous command_inout in blocking mode without timeout --> OK" << std::endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------


// Send a new command

		std::vector<short> in;
		in.push_back(2);
		in.push_back(6);
		din << in;
		cb.l = 0;
		cb.cb_executed = 0;

		device->command_inout_asynch("IOShortSleep",din,cb);

// Check if command returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			nb_not_arrived++;
			coutv << "Command not yet arrived" << std::endl;
			Tango_sleep(1);

			device->get_asynch_replies();
		}

		assert ( cb.to == true );
		assert ( nb_not_arrived >= 2 );

		cout << "   Device timeout exception with non blocking get_asynch_replies --> OK" << std::endl;

// Send a command to check timeout with polling and blocking with timeout

		cb.cb_executed = 0;
		cb.l = 0;
		cb.to = false;

		device->command_inout_asynch("IOShortSleep",din,cb);

// Check if command returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(500);
			}
			catch (AsynReplyNotArrived&)
			{
				coutv << "Command not yet arrived" << std::endl;
				nb_not_arrived++;
			}
		}
		assert( cb.to == true );
		assert( nb_not_arrived >= 2);
		assert( cb.l == 0);

		cout << "   Device timeout with blocking get_asynch_replies with call timeout --> OK" << std::endl;

// Send a command to check polling with blocking

		cb.cb_executed = 0;
		cb.l = 0;
		cb.to = false;

		device->command_inout_asynch("IOShortSleep",din,cb);

// Check if command returned


		device->get_asynch_replies(0);

		assert(cb.to == true );
		assert( cb.l == 0);

		cout << "   Device timeout with blocking get_asynch_replies without timeout --> OK" << std::endl;

//---------------------------------------------------------------------------
//
//			Now test DevFailed exception sent by server
//
//---------------------------------------------------------------------------

		cout << "   Waiting for server to execute all previous requests" << std::endl;
		Tango_sleep(5);


// Send a new command

		short in_e = 2;
		din << in_e;
		cb.l = 0;
		cb.cb_executed = 0;
		cb.cmd_failed = false;

		device->command_inout_asynch("IOSleepExcept",din,cb);

// Check if command returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			nb_not_arrived++;
			coutv << "Command not yet arrived" << std::endl;
			Tango_sleep(1);

			device->get_asynch_replies();
		}
		assert ( cb.cmd_failed == true );
		assert ( nb_not_arrived >= 2);

		cout << "   Device exception with non blocking get_asynch_replies --> OK" << std::endl;

// Send a command to check timeout with polling and blocking with timeout

		cb.cb_executed = 0;
		cb.l = 0;
		cb.to = false;
		nb_not_arrived = 0;
		cb.cmd_failed = false;

		device->command_inout_asynch("IOSleepExcept",din,cb);

// Check if command returned

		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(500);
			}
			catch (AsynReplyNotArrived&)
			{
				coutv << "Command not yet arrived" << std::endl;
				nb_not_arrived++;
			}
		}
		assert( cb.cmd_failed == true );
		assert( cb.cb_executed == 1);
		assert( nb_not_arrived >= 2);

		cout << "   Device exception with blocking get_asynch_replies with call timeout --> OK" << std::endl;

// Send a command to check polling with blocking

		cb.cb_executed = 0;
		cb.l = 0;
		cb.to = false;
		cb.cmd_failed = false;

		device->command_inout_asynch("IOSleepExcept",din,cb);

// Check if command returned

		device->get_asynch_replies(0);

		assert(cb.cmd_failed == true );
		assert(cb.cb_executed == 1);
		assert(cb.l == 0);

		cout << "   Device exception with blocking get_asynch_replies without timeout --> OK" << std::endl;

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

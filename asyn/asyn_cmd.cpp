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
	
// Change timeout (with a useless name as a workaround for ORBacus bug)

//		device->set_timeout_millis(6000);
//		device->name();			
//		coutv << "Timeout set to 6 seconds" << endl;
				
// Send a command to check polling without blocking

		DeviceData din,dout;
		long id;
		vector<short> send;
		send.push_back(4);
		send.push_back(2);
		din << send;

		id = device->command_inout_asynch("IOShortSleep",din);
		
// Check if command returned

		bool finish = false;
		long nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				dout = device->command_inout_reply(id);
				short l;
				dout >> l;
				assert( l == 8 );
				finish = true;
			}
			catch (AsynReplyNotArrived)
			{
				finish = false;
				coutv << "Command not yet arrived" << endl;
				nb_not_arrived++;
			}
			if (finish == false)
				Tango_sleep(1);
		}
		assert ( nb_not_arrived >= 2);
		
		cout << "   Aynchronous command_inout in polling mode --> OK" << endl;

// Send a command to check polling with blocking with timeout

		id = device->command_inout_asynch("IOShortSleep",din);
//		assert( id == 2);
		
// Check if command returned

		finish = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				dout = device->command_inout_reply(id,200);
				short l;
				dout >> l;
				assert( l == 8 );
				finish = true;
			}
			catch (AsynReplyNotArrived)
			{
				coutv << "Command not yet arrived" << endl;
				nb_not_arrived++;
			}
		}
		assert ( nb_not_arrived >= 4);

		cout << "   Aynchronous command_inout in blocking mode with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		id = device->command_inout_asynch("IOShortSleep",din);
//		assert( id == 3);
		
// Check if command returned

		dout = device->command_inout_reply(id,0);
		short l;
		dout >> l;
		
		assert( l == 8 );

		cout << "   Aynchronous command_inout in blocking mode --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------
		
// Change timeout in order to test asynchronous calls and timeout

/*		device->set_timeout_millis(2000);*/

// Send a new command

		vector<short> in;
		in.push_back(2);
		in.push_back(6);
		din << in;
		id = device->command_inout_asynch("IOShortSleep",din);
		
// Check if command returned

		finish = false;
		bool to = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				dout = device->command_inout_reply(id);
				finish = true;
			}
			catch (AsynReplyNotArrived)
			{
				finish = false;
				nb_not_arrived++;
				coutv << "Command not yet arrived" << endl;
			}
			catch (CommunicationFailed &e)
			{
				finish = true;
				if (strcmp(e.errors[1].reason,"API_DeviceTimedOut") == 0)
				{
					to = true;
					coutv << "Timeout exception" << endl;
				}
				else
					coutv << "Comm exception" << endl;
			}
			if (finish == false)
				Tango_sleep(1);
		}
		assert ( to == true );
		assert ( nb_not_arrived >= 2 );
		
		cout << "   Device timeout exception with non blocking command_inout_reply --> OK" << endl;
		
// Send a command to check timeout with polling and blocking with timeout

		id = device->command_inout_asynch("IOShortSleep",din);
		
// Check if command returned

		finish = false;
		to = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				dout = device->command_inout_reply(id,500);
				finish = true;
			}
			catch (AsynReplyNotArrived)
			{
				coutv << "Command not yet arrived" << endl;
				nb_not_arrived++;
			}
			catch (CommunicationFailed &e)
			{
				finish = true;
				if (strcmp(e.errors[1].reason,"API_DeviceTimedOut") == 0)
				{
					to = true;
					coutv << "Timeout exception" << endl;
				}
				else
					coutv << "Comm exception" << endl;
			}
		}
		assert( to == true );
		assert( nb_not_arrived >= 2);
		
		cout << "   Device timeout with blocking command_inout_reply with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		id = device->command_inout_asynch("IOShortSleep",din);
		
// Check if command returned

		to = false;
		
		try
		{
			dout = device->command_inout_reply(id,0);
			finish = true;
		}
		catch (CommunicationFailed &e)
		{
			if (strcmp(e.errors[1].reason,"API_DeviceTimedOut") == 0)
			{
				to = true;
				coutv << "Timeout exception" << endl;
			}
			else
				coutv << "Comm exception" << endl;
		}
		assert(to == true );
		
		cout << "   Device timeout with blocking command_inout_reply --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test DevFailed exception sent by server
//
//---------------------------------------------------------------------------

		cout << "   Waiting for server to execute all previous requests" << endl;
		Tango_sleep(4);

// Change timeout in order to test asynchronous calls and DevFailed exception

//		device->set_timeout_millis(5000);

// Send a new command

		short in_e = 2;
		din << in_e;
		id = device->command_inout_asynch("IOSleepExcept",din);
		
// Check if command returned

		finish = false;
		bool failed = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				dout = device->command_inout_reply(id);
				finish = true;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				nb_not_arrived++;
				coutv << "Command not yet arrived" << endl;
			}
			catch (DevFailed &e)
			{
				finish = true;
				if (strcmp(e.errors[0].reason,"aaa") == 0)
				{
					failed = true;
					coutv << "Server exception" << endl;
				}
				else
					coutv << "Comm exception" << endl;
			}
			if (finish == false)
				Tango_sleep(1);
		}
		assert ( failed == true );
		assert ( nb_not_arrived >= 2);
		
		cout << "   Device exception with non blocking command_inout_reply --> OK" << endl;
		
// Send a command to check timeout with polling and blocking with timeout

		id = device->command_inout_asynch("IOSleepExcept",din);
		
// Check if command returned

		finish = false;
		failed = false;
		while (finish == false)
		{
			try
			{
				dout = device->command_inout_reply(id,500);
				finish = true;
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Command not yet arrived" << endl;
			}
			catch (DevFailed &e)
			{
				finish = true;
				if (strcmp(e.errors[0].reason,"aaa") == 0)
				{
					failed = true;
					coutv << "Server exception" << endl;
				}
				else
					coutv << "Comm exception" << endl;
			}
		}
		assert( failed == true );
		
		cout << "   Device exception with blocking command_inout_reply with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		id = device->command_inout_asynch("IOSleepExcept",din);
		
// Check if command returned

		failed = false;
		try
		{
			dout = device->command_inout_reply(id,0);
			finish = true;
		}
		catch (DevFailed &e)
		{
			if (strcmp(e.errors[0].reason,"aaa") == 0)
			{
				failed = true;
				coutv << "Server exception" << endl;
			}
			else
				coutv << "Comm exception" << endl;
		}
		assert(failed == true );
		
		cout << "   Device exception with blocking command_inout_reply --> OK" << endl;			
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

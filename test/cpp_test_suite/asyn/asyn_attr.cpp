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

	coutv << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

	try
	{
	
				
// Read one attribute

		long id;
		DeviceAttribute *received;

		id = device->read_attribute_asynch("attr_asyn");
		
// Check if attribute returned

		bool finish = false;
		long nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id);
				double db;
				*received >> db;
				assert( db == 5.55 );
				finish = true;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				coutv << "Attribute not yet read" << endl;
				nb_not_arrived++;
			}
			if (finish == false)
				Tango_sleep(1);
		}
		delete received;
		
		assert ( nb_not_arrived >= 2);
		
		cout << "   Asynchronous read_attribute in polling mode --> OK" << endl;
		
// Read one attribute of the DevEncoded data type
// The attribute used to test DevEncoded does not have any
// "sleep" in its code -> Do not check the nb_not_arrived data

#ifndef COMPAT
		id = device->read_attribute_asynch("encoded_attr");
		
// Check if attribute returned

		finish = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id);
				coutv << "Attribute result arrived" << endl;
				Tango::DevEncoded enc_data;
				*received >> enc_data;
				assert( ::strcmp(enc_data.encoded_format,"Which format?") == 0);
				assert( enc_data.encoded_data.length() == 4 );
				finish = true;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				coutv << "Attribute not yet read" << endl;
				nb_not_arrived++;
			}
			if (finish == false)
				Tango_sleep(1);
		}
		delete received;
		
		cout << "   Asynchronous read_attribute (DevEncoded data type) in polling mode --> OK" << endl;
#endif

// Read attribute to check polling with blocking with timeout

		id = device->read_attribute_asynch("attr_asyn");
//		assert( id == 2);
		
// Check if attribute returned

		finish = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id,200);
				double l;
				*received >> l;
				assert( l == 5.55 );
				finish = true;
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet read" << endl;
				nb_not_arrived++;
			}
		}
		delete received;
		
		assert ( nb_not_arrived >= 4);

		cout << "   Asynchronous read_attribute in blocking mode with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		id = device->read_attribute_asynch("attr_asyn");
//		assert( id == 3);
		
// Check if command returned

		received = device->read_attribute_reply(id,0);
		double l;
		*received >> l;
		delete received;
		
		assert( l == 5.55 );

		cout << "   Asynchronous read_attribute in blocking mode --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------
		
// Change timeout in order to test asynchronous calls and timeout

//		device->set_timeout_millis(2000);

// Read an attribute

		id = device->read_attribute_asynch("attr_asyn_to");
		
// Check if attribute returned

		finish = false;
		bool to = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id);
				finish = true;
				delete received;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				nb_not_arrived++;
				coutv << "Attribute not yet read" << endl;
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
		
// Read an attribute to check timeout with polling and blocking with timeout

		id = device->read_attribute_asynch("attr_asyn_to");
		
// Check if command returned

		finish = false;
		to = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id,500);
				finish = true;
				delete received;
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet read" << endl;
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
		
// Read an attribute to check polling with blocking

		id = device->read_attribute_asynch("attr_asyn_to");
		
// Check if attribute returned

		to = false;
		try
		{
			received = device->read_attribute_reply(id,0);
			finish = true;
			delete received;
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
		Tango_sleep(5);

// Change timeout in order to test asynchronous calls and DevFailed exception

//		device->set_timeout_millis(5000);

// Read attribute

		id = device->read_attribute_asynch("attr_asyn_except");
		
// Check if attribute returned

		finish = false;
		bool failed = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id);
				finish = true;
				double db;
				(*received) >> db;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				nb_not_arrived++;
				coutv << "Attribute not yet read" << endl;
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
		delete received;
		
		assert ( failed == true );
		assert ( nb_not_arrived >= 2);
		
		cout << "   Device exception with non blocking read_attribute_reply --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		id = device->read_attribute_asynch("attr_asyn_except");
		
// Check if attribute returned

		finish = false;
		failed = false;
		while (finish == false)
		{
			try
			{
				received = device->read_attribute_reply(id,500);
				finish = true;
				double db;
				(*received) >> db;
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet read" << endl;
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
		
		delete received;
		assert( failed == true );
		
		cout << "   Device exception with blocking read_attribute_reply with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		id = device->read_attribute_asynch("attr_asyn_except");
		
// Check if attribute returned

		failed = false;
		try
		{
			received = device->read_attribute_reply(id,0);
			finish = true;
			double db;
			(*received) >> db;
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
		
		delete received;
		assert(failed == true );
		
		cout << "   Device exception with blocking read_attribute_reply --> OK" << endl;
						
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

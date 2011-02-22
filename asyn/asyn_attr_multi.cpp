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
	
// Change timeout (with a useless name as a workaround for ORBacus bug)

//		device->set_timeout_millis(6000);
				
// Read one attribute

		long id;
		vector<DeviceAttribute> *received;
		vector<string> names;
		names.push_back("attr_asyn");
		names.push_back("Short_attr");
		id = device->read_attributes_asynch(names);
		
// Check if attribute returned

		bool finish = false;
		long nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attributes_reply(id);
				double db;
				short sh;
				(*received)[0] >> db;
				assert( db == 5.55 );
				(*received)[1] >> sh;
				assert( sh == 12 );
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
		
		cout << "   Asynchronous read_attributes in polling mode --> OK" << endl;

// Read attribute to check polling with blocking with timeout

		id = device->read_attributes_asynch(names);
//		assert( id == 2);
		
// Check if attribute returned

		finish = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attributes_reply(id,200);
				double l;
				short s;
				(*received)[0] >> l;
				assert( l == 5.55 );
				(*received)[1] >> s;
				assert( s == 12 );
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

		cout << "   Asynchronous read_attributes in blocking mode with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		id = device->read_attributes_asynch(names);
//		assert( id == 3);
		
// Check if command returned

		received = device->read_attributes_reply(id,0);
		double l;
		(*received)[0] >> l;
		short s1;
		(*received)[1] >> s1;
		delete received;
		
		assert( l == 5.55 );
		assert( s1 == 12 );

		cout << "   Asynchronous read_attributes in blocking mode --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------
		
// Change timeout in order to test asynchronous calls and timeout

//		device->set_timeout_millis(2000);

// Read an attribute

		names[0] = "attr_asyn_to";
		id = device->read_attributes_asynch(names);
		
// Check if attribute returned

		finish = false;
		bool to = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attributes_reply(id);
				finish = true;
				delete received;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				nb_not_arrived++;
				coutv << "Attributes not yet read" << endl;
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
		
		cout << "   Device timeout exception with non blocking read_attributes_reply --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		id = device->read_attributes_asynch(names);
		
// Check if command returned

		finish = false;
		to = false;
		nb_not_arrived = 0;
		while (finish == false)
		{
			try
			{
				received = device->read_attributes_reply(id,500);
				finish = true;
				delete received;
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attributes not yet read" << endl;
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
		
		cout << "   Device timeout with blocking read_attributes_reply with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		id = device->read_attributes_asynch(names);
		
// Check if attribute returned

		to = false;
		try
		{
			received = device->read_attributes_reply(id,0);
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
		
		cout << "   Device timeout with blocking read_attributes_reply --> OK" << endl;

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

		names[0] = "attr_asyn_except";
		id = device->read_attributes_asynch(names);
		
// Check if attribute returned

		finish = false;
		bool failed = false;
		bool strange_err = false;
		nb_not_arrived = 0;
		short sh;
		while (finish == false)
		{
			try
			{
				received = device->read_attributes_reply(id);
				finish = true;
				if ((*received)[0].has_failed() == true)
				{
					DevErrorList err = (*received)[0].get_err_stack();
					if (strcmp(err[0].reason,"aaa") == 0)
					{
						failed = true;
						coutv << "Server exception" << endl;
					}
				}
				if ((*received)[1].has_failed() == true)
					strange_err = true;
				else
					(*received)[1] >> sh;					
				delete received;
			}
			catch (AsynReplyNotArrived )
			{
				finish = false;
				nb_not_arrived++;
				coutv << "Attributes not yet read" << endl;
			}
			catch (DevFailed &e)
			{
				coutv << "Comm exception" << endl;
			}
			if (finish == false)
				Tango_sleep(1);
		}
		assert ( failed == true );
		assert ( sh == 12 );
		assert (strange_err == false );
		assert ( nb_not_arrived >= 2 );
		
		cout << "   Device exception with non blocking read_attributes_reply --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		id = device->read_attributes_asynch(names);
		
// Check if attribute returned

		finish = false;
		failed = false;
		while (finish == false)
		{
			try
			{
				received = device->read_attributes_reply(id,500);
				finish = true;
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attributes not yet read" << endl;
			}
			catch (DevFailed &e)
			{
				coutv << "Comm exception" << endl;
			}
		}
		bool all_extracted = false;
		int i;
		long nb_extracted;
		sh = 0;
		
		while (all_extracted == false)
		{
			nb_extracted = 0;
			try
			{
				double db;
				nb_extracted++;
				(*received)[0] >> db;
				nb_extracted++;
				(*received)[1] >> sh;
				delete received;
				all_extracted = true;
			}
			catch (DevFailed &e)
			{
				for (i = 0;i < nb_extracted;i++)
					(*received)[i].reset_exceptions(DeviceAttribute::failed_flag);
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
		assert( sh == 12 );
		
		cout << "   Device exception with blocking read_attributes_reply with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		id = device->read_attributes_asynch(names);
		
// Check if attribute returned

		failed = false;
		sh = 0;
		try
		{
			received = device->read_attributes_reply(id,0);
			finish = true;
			for (i = 0;i < received->size();i++)
				(*received)[i].reset_exceptions(DeviceAttribute::failed_flag);
			
			double db;
			if (((*received)[0] >> db) == false)
			{
				DevErrorList err = (*received)[0].get_err_stack();
				if (strcmp(err[0].reason,"aaa") == 0)
				{
					failed = true;
					coutv << "Server exception" << endl;
				}
			}
			if (((*received)[1] >> sh) == false)
				strange_err = true;

			delete received;
		}
		catch (DevFailed &e)
		{
			coutv << "Comm exception" << endl;
		}
		

		assert( failed == true );
		assert( sh == 12 );
		
		cout << "   Device exception with blocking read_attributes_reply --> OK" << endl;
						
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

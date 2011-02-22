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
	MyCallBack():cb_executed(0),to(false),attr_failed(false) {};
	
	virtual void attr_read(AttrReadEvent *);
	
	long cb_executed;
	bool to;
	bool attr_failed;
	double db;
	short sh;
	long nb_attr;
};

void MyCallBack::attr_read(AttrReadEvent *att)
{
	coutv << "In attr_read method for device " << att->device->dev_name() << endl;
	for (int i = 0;i < att->attr_names.size();i++)
		coutv << "Attribute read = " << att->attr_names[i] << endl;
		
	nb_attr = att->attr_names.size();
	
	if (att->err == false)
	{
		try
		{
			(*(att->argout))[0] >> db;
			if (nb_attr == 2)
				(*(att->argout))[1] >> sh;
		}
		catch (DevFailed &e)
		{
			long nb_err = e.errors.length();
			if (strcmp(e.errors[nb_err - 1].reason,"API_AttributeFailed") == 0)
			{
				attr_failed = true;
				coutv << "Read attributes failed error" << endl;
			}
			else
				coutv << "Unknown error" << endl;		
		}
	}
	else
	{
		long nb_err = att->errors.length();
		coutv << "read_attributes returns error" << endl;
		coutv << "error length = " << nb_err << endl;
		for (int i = 0;i < nb_err;i++)
			coutv << "error[" << i << "].reason = " << att->errors[i].reason << endl;
		
		attr_failed = true;
		if (strcmp(att->errors[nb_err - 1].reason,"API_DeviceTimedOut") == 0)
		{
			to = true;
			coutv << "Timeout error" << endl;
		}
	}
	
	cb_executed++;
	delete att->argout;
}	

int main(int argc, char **argv)
{
	DeviceProxy *device;
	long loop = 10;
	
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

	MyCallBack cb;
	
	try
	{
	
// Change timeout (with a useless name as a workaround for ORBacus bug)

//		device->set_timeout_millis(6000);
				
// Read one attribute

		
		device->read_attribute_asynch("attr_asyn",cb);
		
// Check if attribute returned

		long nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			coutv << "Attribute not read yet" << endl;
			nb_not_arrived++;
			Tango_sleep(1);
			
			device->get_asynch_replies();

		}
		
		assert( cb.db == 5.55 );
		assert ( nb_not_arrived >= 2);
		
		cout << "   Asynchronous read_attribute in callback mode --> OK" << endl;

// Read attribute to check polling with blocking with timeout

		cb.cb_executed = 0;
		cb.db = 0;
		device->read_attribute_asynch("attr_asyn",cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(200);
			}
			catch (AsynReplyNotArrived &e)
			{
				coutv << "Attribute not yet read" << endl;
				nb_not_arrived++;
			}
		}
		
		assert ( cb.db == 5.55 );
		assert ( nb_not_arrived >= 4);
		assert ( cb.cb_executed == 1);
		
		cout << "   Asynchronous read_attribute (callback) in blocking mode with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		cb.cb_executed = 0;
		cb.db = 0;
		device->read_attribute_asynch("attr_asyn",cb);
		
// Check if command returned

		device->get_asynch_replies(0);
		
		assert( cb.db == 5.55 );
		assert( cb.cb_executed == 1);

		cout << "   Asynchronous read_attribute (callback) in blocking mode --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------
		
// Change timeout in order to test asynchronous calls and timeout

//		device->set_timeout_millis(2000);

// Read an attribute

		cb.cb_executed = 0;
		cb.db = 0;
		device->read_attribute_asynch("attr_asyn_to",cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			nb_not_arrived++;
			coutv << "Attribute not yet read" << endl;
			Tango_sleep(1);
			
			device->get_asynch_replies();
		}

		assert ( cb.to == true );
		assert ( nb_not_arrived >= 2 );
		
		cout << "   Device timeout exception with non blocking get_asynch_replies --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		cb.cb_executed = 0;
		cb.db = 0.0;
		cb.to = false;
		device->read_attribute_asynch("attr_asyn_to",cb);
		
// Check if command returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(500);
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet read" << endl;
				nb_not_arrived++;
			}
		}
		assert( cb.to == true );
		assert( nb_not_arrived >= 2);
		
		cout << "   Device timeout with blocking get_asynch_replies with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		cb.cb_executed = 0;
		cb.db = 0.0;
		cb.to = false;
		
		device->read_attribute_asynch("attr_asyn_to",cb);
		
// Check if attribute returned

		device->get_asynch_replies(0);

		assert(cb.to == true );
		
		cout << "   Device timeout with blocking get_asynch_replies --> OK" << endl;

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

		cb.cb_executed = 0;
		cb.db = 0.0;
		cb.to = false;
		cb.attr_failed = false;
		device->read_attribute_asynch("attr_asyn_except",cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			nb_not_arrived++;
			coutv << "Attribute not yet read" << endl;
			Tango_sleep(1);
				
			device->get_asynch_replies();
		}

		assert ( cb.attr_failed == true );
		assert ( nb_not_arrived >= 2);
		
		cout << "   Device exception with non blocking get_asynch_replies --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		cb.cb_executed = 0;
		cb.db = 0.0;
		cb.attr_failed = false;
		nb_not_arrived = 0;
		
		device->read_attribute_asynch("attr_asyn_except",cb);
		
// Check if attribute returned

		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(500);
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet read" << endl;
				nb_not_arrived++;
			}

		}
		assert( cb.attr_failed == true );
		assert(cb.cb_executed == 1);
		assert( nb_not_arrived >= 2);
		
		cout << "   Device exception with blocking get_asynch_replies with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		cb.cb_executed = 0;
		cb.attr_failed = false;
		
		device->read_attribute_asynch("attr_asyn_except",cb);
		
// Check if attribute returned

		device->get_asynch_replies(0);

		assert(cb.attr_failed == true );
		assert(cb.cb_executed == 1);
				
		cout << "   Device exception with blocking get_asynch_replies --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test several attributes read in one call
//
//---------------------------------------------------------------------------

// Read several attributes in one call

		cb.nb_attr = 0;
		cb.db = 0.0;
		cb.cb_executed = 0;
		
		vector<string> na;
		na.push_back("attr_asyn");
		na.push_back("Short_attr");
				
		device->read_attributes_asynch(na,cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			coutv << "Attributes not read yet" << endl;
			nb_not_arrived++;
			Tango_sleep(1);
			
			device->get_asynch_replies();

		}
		
		assert( cb.db == 5.55 );
		assert ( nb_not_arrived >= 2);
		assert ( cb.nb_attr == 2);
		assert (cb.sh == 12);
		
		cout << "   Several attributes in read_attributes in callback mode --> OK" << endl;
								
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

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
	
	virtual void attr_written(AttrWrittenEvent *);
	
	long cb_executed;
	bool to;
	bool attr_failed;
	long nb_attr;
	long faulty_attr_nb;
};

void MyCallBack::attr_written(AttrWrittenEvent *att)
{
	coutv << "In attr_written method for device " << att->device->dev_name() << endl;
	for (int i = 0;i < att->attr_names.size();i++)
		coutv << "Attribute written = " << att->attr_names[i] << endl;
	
	nb_attr = att->attr_names.size();
	faulty_attr_nb = att->errors.get_faulty_attr_nb();
	
	if (att->err == true)
	{
		long nb_err = att->errors.errors.length();
		coutv << "write_attributes returns error" << endl;
		coutv << "error length = " << nb_err << endl;
		for (int i = 0;i < nb_err;i++)
			coutv << "error[" << i << "].reason = " << att->errors.errors[i].reason << endl;
		if (strcmp(att->errors.errors[nb_err - 1].reason,"API_DeviceTimedOut") == 0)
		{
			to = true;
			coutv << "Timeout error" << endl;
		}
		else if (strcmp(att->errors.errors[nb_err - 1].reason,"API_AttributeFailed") == 0)
		{
			attr_failed = true;
			coutv << "Write attributes failed error" << endl;
		}
		else
			coutv << "Unknown error" << endl;
	}
	else
	{
		if (faulty_attr_nb != 0)
		{
			if (strcmp(att->errors.errors[0].reason,"API_AttributeFailed") == 0)
			{
				if ((strcmp(att->errors.err_list[0].err_stack[0].reason,"aaa") == 0) &&
				    (att->errors.err_list[0].idx_in_call == 0) &&
				    (att->errors.err_list[0].name == "attr_asyn_write_except"))
				{
					attr_failed = true;
					coutv << "Write attributes failed error" << endl;
				}
			}
		}
	}
	
	cb_executed++;
}	

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
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
				
// Write one attribute

		MyCallBack cb;

		DeviceAttribute send;
		
		send.set_name("attr_asyn_write");
		DevLong lg = 222;
		send << lg;
				
		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		long nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			coutv << "Attribute not written yet" << endl;
			nb_not_arrived++;
			Tango_sleep(1);
			
			device->get_asynch_replies();
		}

		assert ( cb.cb_executed == 1 );
		assert ( nb_not_arrived >= 2);
		assert ( cb.nb_attr == 1);
		assert ( cb.faulty_attr_nb == 0);
		
		cout << "   Asynchronous write_attribute in callback mode --> OK" << endl;

// Read attribute to check polling with blocking with timeout

		cb.cb_executed = 0;
		cb.nb_attr = 0;
		cb.faulty_attr_nb = 10;

		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(200);
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet written" << endl;
				nb_not_arrived++;
			}
		}
		
		assert ( cb.nb_attr== 1 );
		assert ( nb_not_arrived >= 4);
		assert ( cb.cb_executed == 1);
		assert ( cb.faulty_attr_nb == 0);
		
		cout << "   Asynchronous write_attribute (callback) in blocking mode with call timeout --> OK" << endl;
		
// Send a command to check polling with blocking

		cb.cb_executed = 0;
		cb.nb_attr = 0;
		cb.faulty_attr_nb = 10;
		
		device->write_attribute_asynch(send,cb);
		
// Check if command returned

		device->get_asynch_replies(0);
		
		assert( cb.nb_attr == 1 );
		assert( cb.cb_executed == 1);
		assert ( cb.faulty_attr_nb == 0);

		cout << "   Asynchronous write_attribute (callback) in blocking mode --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------
		
// Change timeout in order to test asynchronous calls and timeout

//		device->set_timeout_millis(2000);

// Read an attribute

		cb.cb_executed = 0;
		cb.nb_attr = 0;
		cb.faulty_attr_nb = 10;

		send.set_name("attr_asyn_write_to");		
		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			nb_not_arrived++;
			coutv << "Attribute not yet written" << endl;
			Tango_sleep(1);
			
			device->get_asynch_replies();
		}

		assert ( cb.to == true );
		assert ( nb_not_arrived >= 2 );
		assert ( cb.nb_attr == 1);
		assert ( cb.faulty_attr_nb == 0);
		
		cout << "   Device timeout exception with non blocking get_asynch_replies --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		cb.cb_executed = 0;
		cb.to = false;
		cb.nb_attr = 0;
		cb.faulty_attr_nb = 10;
		
		device->write_attribute_asynch(send,cb);
		
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
				coutv << "Attribute not yet written" << endl;
				nb_not_arrived++;
			}
		}
		assert ( cb.to == true );
		assert ( nb_not_arrived >= 2);
		assert ( cb.nb_attr == 1);
		assert ( cb.faulty_attr_nb == 0);
		
		cout << "   Device timeout with blocking get_asynch_replies with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		cb.cb_executed = 0;
		cb.to = false;
		cb.nb_attr = 0;
		cb.faulty_attr_nb = 10;
		
		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		device->get_asynch_replies(0);

		assert ( cb.to == true );
		assert ( cb.nb_attr == 1);
		assert ( cb.faulty_attr_nb == 0);
		
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
		cb.to = false;
		cb.nb_attr = 0;
		cb.faulty_attr_nb = 10;
		
		send.set_name("attr_asyn_write_except");		
		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			nb_not_arrived++;
			coutv << "Attribute not yet written" << endl;
			Tango_sleep(1);
				
			device->get_asynch_replies();
		}

		assert ( cb.attr_failed == true );
		assert ( nb_not_arrived >= 2);
		assert ( cb.nb_attr == 1);
		assert ( cb.faulty_attr_nb = 1);
		
		cout << "   Device exception with non blocking get_asynch_replies --> OK" << endl;
		
// Read an attribute to check timeout with polling and blocking with timeout

		cb.cb_executed = 0;
		cb.attr_failed = false;
		cb.nb_attr = 0;
		nb_not_arrived = 0;
		
		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		while (cb.cb_executed == 0)
		{
			try
			{
				device->get_asynch_replies(500);
			}
			catch (AsynReplyNotArrived )
			{
				coutv << "Attribute not yet written" << endl;
				nb_not_arrived++;
			}

		}
		assert( cb.attr_failed == true );
		assert(cb.cb_executed == 1);
		assert( nb_not_arrived >= 2);
		assert( cb.nb_attr = 1);
		
		cout << "   Device exception with blocking get_asynch_replies with call timeout --> OK" << endl;
		
// Read an attribute to check polling with blocking

		cb.cb_executed = 0;
		cb.attr_failed = false;
		cb.nb_attr = 0;
		
		device->write_attribute_asynch(send,cb);
		
// Check if attribute returned

		device->get_asynch_replies(0);

		assert(cb.attr_failed == true );
		assert(cb.cb_executed == 1);
		assert( cb.nb_attr = 1);
				
		cout << "   Device exception with blocking get_asynch_replies --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test several attributes written in one call
//
//---------------------------------------------------------------------------

// Write several attributes in one call

		cb.nb_attr = 0;
		cb.cb_executed = 0;

		vector<DeviceAttribute> send_m(2);
		
		send_m[0].set_name("attr_asyn_write");
		lg = 222;
		send_m[0] << lg;
		
		send_m[1].set_name("Double_attr_w");
		double db = 4.55;
		send_m[1] << db;
				
		device->write_attributes_asynch(send_m,cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			coutv << "Attributes not read written" << endl;
			nb_not_arrived++;
			Tango_sleep(1);
			
			device->get_asynch_replies();

		}
		
		assert ( nb_not_arrived >= 2);
		assert ( cb.nb_attr == 2);
		
		cout << "   Several attributes in write_attributes in callback mode --> OK" << endl;
								
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

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
	MyCallBack():cb_executed(0),to(false),cmd_failed(false),attr_failed(false) {};
	
	virtual void cmd_ended(CmdDoneEvent *);
	virtual void attr_read(AttrReadEvent *);
	virtual void attr_written(AttrWrittenEvent *);
		
	long cb_executed;
	short l;
	double db;
	short sh;
	bool to;
	bool cmd_failed;
	bool attr_failed;
	long nb_attr;
};

void MyCallBack::cmd_ended(CmdDoneEvent *cmd)
{
	coutv << "In cmd_ended method for device " << cmd->device->dev_name() << endl;
	coutv << "Command = " << cmd->cmd_name << endl;
	
	if (cmd->errors.length() == 0)
	{
		cmd->argout >> l;
		coutv << "Command result = " << l << endl;
	}
	else
	{
		long nb_err = cmd->errors.length();
		coutv << "Command returns error" << endl;
		coutv << "error length = " << nb_err << endl;
		for (int i = 0;i < nb_err;i++)
			coutv << "error[" << i << "].reason = " << cmd->errors[i].reason.in() << endl;
		if (strcmp(cmd->errors[nb_err - 1].reason,"API_DeviceTimedOut") == 0)
		{
			to = true;
			coutv << "Timeout error" << endl;
		}
		else if (strcmp(cmd->errors[nb_err - 1].reason,"API_CommandFailed") == 0)
		{
			cmd_failed = true;
			coutv << "Command failed error" << endl;
		}
		else
			coutv << "Unknown error" << endl;
	}
	
	cb_executed++;
}	



void MyCallBack::attr_read(AttrReadEvent *att)
{
	coutv << "In attr_read method for device " << att->device->dev_name() << endl;
	for (int i = 0;i < att->attr_names.size();i++)
		coutv << "Attribute read = " << att->attr_names[i] << endl;
		
	nb_attr = att->attr_names.size();
	
	if (att->err == false)
	{
		(*(att->argout))[0] >> db;
		if (nb_attr == 2)
			(*(att->argout))[1] >> sh;
	}
	else
	{
		long nb_err = att->errors.length();
		coutv << "read_attributes returns error" << endl;
		coutv << "error length = " << nb_err << endl;
		for (int i = 0;i < nb_err;i++)
			coutv << "error[" << i << "].reason = " << att->errors[i].reason.in() << endl;
		if (strcmp(att->errors[nb_err - 1].reason,"API_DeviceTimedOut") == 0)
		{
			to = true;
			coutv << "Timeout error" << endl;
		}
		else if (strcmp(att->errors[nb_err - 1].reason,"API_AttributeFailed") == 0)
		{
			attr_failed = true;
			coutv << "Read attributes failed error" << endl;
		}
		else
			coutv << "Unknown error" << endl;
	}

	cb_executed++;
	delete att->argout;	
}

void MyCallBack::attr_written(AttrWrittenEvent *att)
{
	coutv << "In attr_written method for device " << att->device->dev_name() << endl;
	for (int i = 0;i < att->attr_names.size();i++)
		coutv << "Attribute written = " << att->attr_names[i] << endl;
		
	nb_attr = att->attr_names.size();
	
	if (att->err == true)
	{
		long nb_err = att->errors.get_faulty_attr_nb();
		coutv << "write_attributes returns error" << endl;
		coutv << "error length = " << nb_err << endl;
		for (long i = 0;i < nb_err;i++)
			coutv << "error[" << i << "].reason = " << att->errors.err_list[i].err_stack[0].reason.in() << endl;
		if (strcmp(att->errors.err_list[nb_err - 1].err_stack[0].reason.in(),"API_DeviceTimedOut") == 0)
		{
			to = true;
			coutv << "Timeout error" << endl;
		}
		else
			coutv << "Unknown error" << endl;
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
		{
			verbose = true;
		}
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
	

// Set the automatic callback mode

		ApiUtil::instance()->set_asynch_cb_sub_model(PUSH_CALLBACK);
						
// Send a command to check asynchronous callback

		long nb_not_arrived = 0;
		bool finish = false;
		
		DeviceData din,dout;
		vector<short> send;
		send.push_back(4);
		send.push_back(2);
		din << send;

		MyCallBack cb;
		
		device->command_inout_asynch("IOShortSleep",din,cb);
		
// Check if command returned

		while (cb.cb_executed == 0)
		{
			coutv << "Command not yet arrived" << endl;
			nb_not_arrived++;			
			Tango_sleep(1);			
		}
		assert ( nb_not_arrived >= 2);
		assert ( cb.cb_executed == 1);
		assert ( cb.l == 8);
		
		cout << "   Aynchronous command_inout in callback mode --> OK" << endl;

//---------------------------------------------------------------------------
//
//			Now test Timeout exception and asynchronous calls
//
//---------------------------------------------------------------------------
		
// Send a new command

		vector<short> in;
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
			coutv << "Command not yet arrived" << endl;
			Tango_sleep(1);			
		}
		
		assert ( cb.to == true );
		assert ( nb_not_arrived >= 2 );
		
		cout << "   Device timeout exception with non blocking get_asynch_replies --> OK" << endl;
		
//---------------------------------------------------------------------------
//
//			Now test DevFailed exception sent by server
//
//---------------------------------------------------------------------------

		cout << "   Waiting for server to execute all previous requests" << endl;
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
			coutv << "Command not yet arrived" << endl;
			Tango_sleep(1);
		}
		assert ( cb.cmd_failed == true );
		assert ( nb_not_arrived >= 2);
		
		cout << "   Device exception with non blocking get_asynch_replies --> OK" << endl;

//----------------------------------------------------------------------------
//
// 			Now, test writing attributes
//
//----------------------------------------------------------------------------
		
		DeviceAttribute send_w;
		
		send_w.set_name("attr_asyn_write");
		DevLong lg = 222;
		send_w << lg;

		cb.cb_executed = 0;
		cb.cmd_failed = false;
						
		device->write_attribute_asynch(send_w,cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			coutv << "Attribute not written yet" << endl;
			nb_not_arrived++;
			Tango_sleep(1);
		}

		assert( cb.cb_executed == 1 );
		assert ( nb_not_arrived >= 2);
		assert (cb.nb_attr == 1);
		
		cout << "   Asynchronous write_attribute in callback mode --> OK" << endl;

//----------------------------------------------------------------------------
//
// 			Now, test reading attributes
//
//----------------------------------------------------------------------------

// Read one attribute

		cb.cb_executed = 0;
				
		device->read_attribute_asynch("attr_asyn",cb);
		
// Check if attribute returned

		nb_not_arrived = 0;
		while (cb.cb_executed == 0)
		{
			coutv << "Attribute not read yet" << endl;
			nb_not_arrived++;
			Tango_sleep(1);
		}
		
		assert( cb.db == 5.55 );
		assert ( nb_not_arrived >= 2);
		
		cout << "   Asynchronous read_attribute in callback mode --> OK" << endl;

//----------------------------------------------------------------------------
//
// 			Now, all in one go
//
//----------------------------------------------------------------------------

		cb.cb_executed = 0;
		send.clear();
		send.push_back(4);
		send.push_back(1);
		din << send;
		send_w.set_name("Long_attr_w");
		
		device->command_inout_asynch("IOShortSleep",din,cb);
		device->read_attribute_asynch("Double_attr",cb);
		device->write_attribute_asynch(send_w,cb);
		
// Check if attribute returned

		long sleep_loop = 3;
		while (sleep_loop > 0)
		{
			coutv << "Not ready" << endl;
			Tango_sleep(1);
			sleep_loop--;
		}

		assert( cb.cb_executed == 3 );
		
		cout << "   All in one go (callback mode) --> OK" << endl;
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

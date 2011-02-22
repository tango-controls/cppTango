/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc < 2)
	{
		cout << "usage: %s device [-v]" << endl;
		exit(-1);
	}

	string device_name = argv[1];
	bool print = false;
	
	if (argc == 3)
	{
		string verb = argv[2];
		if (verb != "-v")
		{
			cout << "Usage: %s device [-v]" << endl;
			exit(-1);
		}
		else
			print = true;
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

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;

	int i;
	vector<DeviceDataHistory> *d_hist;
	vector<DeviceAttributeHistory> *a_hist;
	
	try
	{

// Set IOStr1 command to be polled with external trigger

		device->poll_command("IOStr1",0);
		
// Send three trigger

		DeviceData d_in;
		DevString ds = "IOStr1";
		d_in << ds;
		for (i = 0;i < 3;i++)
		{
			device->command_inout("IOTrigPoll",d_in);;
		}
			
// Test command_history (for strings)

		int hist_depth = 10;
		d_hist = device->command_history("IOStr1",hist_depth);

		string first_str;		
		for (i = 0;i < d_hist->size();i++)
		{
			string str;
			(*d_hist)[i] >> str;
			
			if (print == true)
			{
				cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
				cout << "Value = " << str << endl;
				TimeVal &t = (*d_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			assert( str == "Hello from IOStr1");
		}
		delete d_hist;	
				
		cout << "   Read command history for one externally triggered polled command --> OK" << endl;
	
		device->stop_poll_command("IOStr1");
		
// Set Poll_buff command to be polled with external trigger

		device->poll_attribute("Poll_buff",0);
		
// Fill polling buffer

		device->command_inout("IOFillPollBuffAttr");
		
// Test command_history (for strings)

		hist_depth = 10;
		a_hist = device->attribute_history("Poll_buff",hist_depth);

		assert ( a_hist->size()	== 3);
			
		for (i = 0;i < a_hist->size();i++)
		{
			if (print == true)
			{
				cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}

			if (i < 3)
			{			
				vector<string> str;
				(*a_hist)[i] >> str;
						
				assert( (*a_hist)[i].has_failed() == false);
				assert( (*a_hist)[i].get_err_stack().length() == 0);

				switch (i)
				{
				case 0 :
					assert( str[0] == "One_1");
					assert( str[1] == "Two_1");
					assert( str[2] == "Three_1");
					assert( str[3] == "Four_1");
					break;

				case 1 :
					assert( str[0] == "One_2");
					assert( str[1] == "Two_2");
					assert( str[2] == "Three_2");
					assert( str[3] == "Four_2");
					break;

				case 2 :
					assert( str[0] == "One_3");
					assert( str[1] == "Two_3");
					assert( str[2] == "Three_3");
					assert( str[3] == "Four_3");
					break;
				}
			}				
		}
		delete a_hist;


// Fill polling buffer once more

		device->command_inout("IOFillPollBuffAttr");
		
// Test command_history (for strings)

		hist_depth = 10;
		a_hist = device->attribute_history("Poll_buff",hist_depth);

		assert ( a_hist->size()	== 6);
			
		for (i = 0;i < a_hist->size();i++)
		{
			vector<string> str;
			(*a_hist)[i] >> str;
			
			if (print == true)
			{
				cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*a_hist)[i].has_failed() == false);
			assert( (*a_hist)[i].get_err_stack().length() == 0);
			
			switch (i)
			{
			case 0 :
				assert( str[0] == "One_1");
				assert( str[1] == "Two_1");
				assert( str[2] == "Three_1");
				assert( str[3] == "Four_1");
				break;
				
			case 1 :
				assert( str[0] == "One_2");
				assert( str[1] == "Two_2");
				assert( str[2] == "Three_2");
				assert( str[3] == "Four_2");
				break;
				
			case 2 :
				assert( str[0] == "One_3");
				assert( str[1] == "Two_3");
				assert( str[2] == "Three_3");
				assert( str[3] == "Four_3");
				break;
				
			case 3 :
				assert( str[0] == "One_1");
				assert( str[1] == "Two_1");
				assert( str[2] == "Three_1");
				assert( str[3] == "Four_1");
				break;
				
			case 4 :
				assert( str[0] == "One_2");
				assert( str[1] == "Two_2");
				assert( str[2] == "Three_2");
				assert( str[3] == "Four_2");
				break;
				
			case 5 :
				assert( str[0] == "One_3");
				assert( str[1] == "Two_3");
				assert( str[2] == "Three_3");
				assert( str[3] == "Four_3");
				break;
			}				
		}
		delete a_hist;
		
// Now read the attribute traditionally from CACHE and from DEVICE

		device->set_source(Tango::CACHE);
		
		DeviceAttribute da;
		da = device->read_attribute("Poll_buff");

		DevVarStringArray *str;
		da >> str;
		assert ( !strcmp((*str)[0],"One_3") );
		assert ( !strcmp((*str)[1],"Two_3") );
		
		delete str;
		
// Now from DEVICE

		device->set_source(Tango::DEV);

		bool failed = false;
		try
		{		
			da = device->read_attribute("Poll_buff");
		}
		catch (Tango::DevFailed &)
		{
			failed = true;
		}

		assert ( failed == false );
		assert ( da.has_failed() == true );

		const DevErrorList &errs = da.get_err_stack();
		assert ( !strncmp(errs[0].desc.in(),"Attribute Poll_buff value is available only by CACHE",52) );
		
		device->stop_poll_attribute("Poll_buff");

		cout << "   Read attribute history for one attribute with polling buffer externally filled --> OK" << endl;

	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
	try
	{
	
// Set IOStr1 command to be polled with external trigger

		device->poll_attribute("PollLong_attr",0);
		
// Send three trigger

		DeviceData d_in;
		DevString ds = "PollLong_attr";
		d_in << ds;
		for (i = 0;i < 3;i++)
		{
			device->command_inout("IOAttrTrigPoll",d_in);
		}
			
// Test attribute history (for strings)

		int hist_depth = 10;
		a_hist = device->attribute_history("PollLong_attr",hist_depth);

		int first;
		for (i = 0;i < a_hist->size();i++)
		{
			long val;
			(*a_hist)[i] >> val;
			
			if (print == true)
			{
				cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
				cout << "Value = " << val << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*a_hist)[i].has_failed() == false);
			assert( (*a_hist)[i].get_err_stack().length() == 0);
			if (i == 0)
			{
				if (val == 5555)
					first = 5;
				else
					first = 6;
			}
			if (first == 5)
			{
				if ((i % 2) == 0)
					assert( val == 5555);
				else
					assert( val == 6666);
			}
			else
			{
				if ((i % 2) == 0)
					assert( val == 6666);
				else
					assert( val == 5555);
			}
		}
		delete a_hist;	
				
		cout << "   Read attribute history for one externally triggered polled attribute --> OK" << endl;
	
		device->stop_poll_attribute("PollLong_attr");

// Set IOArray1 command to be polled with external trigger

		device->poll_command("IOArray1",0);
		
// Fill polling buffer

		device->command_inout("IOFillPollBuffCmd");
		
// Test command_history

		hist_depth = 10;
		d_hist = device->command_history("IOArray1",hist_depth);

		assert ( d_hist->size()	== 3);
			
		for (i = 0;i < d_hist->size();i++)
		{
			vector<long> v_long;
			(*d_hist)[i] >> v_long;
			
			if (print == true)
			{
				cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
				TimeVal &t = (*d_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			
			switch (i)
			{
			case 0 :
				assert( v_long[0] == 10);
				assert( v_long[1] == 11);
				assert( v_long[2] == 12);
				break;
				
			case 1 :
				assert( v_long[0] == 11);
				assert( v_long[1] == 12);
				assert( v_long[2] == 13);
				break;
				
			case 2 :
				assert( v_long[0] == 12);
				assert( v_long[1] == 13);
				assert( v_long[2] == 14);
				break;
			}				
		}
		delete d_hist;

// Fill polling buffer once more

		device->command_inout("IOFillPollBuffCmd");

// Test command_history

		hist_depth = 10;
		d_hist = device->command_history("IOArray1",hist_depth);

		assert ( d_hist->size()	== 6);
			
		for (i = 0;i < d_hist->size();i++)
		{
			vector<long> v_long;
			(*d_hist)[i] >> v_long;
			
			if (print == true)
			{
				cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
				TimeVal &t = (*d_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			
			switch (i)
			{
			case 0 :
				assert( v_long[0] == 10);
				assert( v_long[1] == 11);
				assert( v_long[2] == 12);
				break;
				
			case 1 :
				assert( v_long[0] == 11);
				assert( v_long[1] == 12);
				assert( v_long[2] == 13);
				break;
				
			case 2 :
				assert( v_long[0] == 12);
				assert( v_long[1] == 13);
				assert( v_long[2] == 14);
				break;
				
			case 3 :
				assert( v_long[0] == 10);
				assert( v_long[1] == 11);
				assert( v_long[2] == 12);
				break;
				
			case 4 :
				assert( v_long[0] == 11);
				assert( v_long[1] == 12);
				assert( v_long[2] == 13);
				break;
				
			case 5 :
				assert( v_long[0] == 12);
				assert( v_long[1] == 13);
				assert( v_long[2] == 14);
				break;
			}				
		}
		delete d_hist;	
					
// Now read the command traditionally from CACHE and from DEVICE

		device->set_source(Tango::CACHE);
		
		DeviceData dd;
		dd = device->command_inout("IOArray1");

		vector<long> ve;
		dd >> ve;

		assert ( ve[0] == 12 );
		assert ( ve[1] == 13 );
		assert ( ve[2] == 14 );
		
// Now from DEVICE

		device->set_source(Tango::DEV);
		
		dd = device->command_inout("IOArray1");

		dd >> ve;
		assert ( ve[0] == 10 );
		assert ( ve[1] == 20 );
		assert ( ve[2] == 30 );
		assert ( ve[3] == 40 );
		
		device->stop_poll_command("IOArray1");

		cout << "   Read command history for one command with polling buffer externally filled --> OK" << endl;		
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

			
	delete device;
	return 0;
}

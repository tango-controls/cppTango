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
	
	try
	{
	
// Test command_history (for strings)

		int hist_depth = 4;
		d_hist = device->command_history("IOPollStr1",hist_depth);

		string first_str;		
		for (i = 0;i < hist_depth;i++)
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

			if (i == 0)
				first_str = str;			
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			if (i != 0)
			{
				if (first_str == "Even value from IOPollStr1")
				{
					if ((i % 2) == 0)
						assert( str == "Even value from IOPollStr1");
					else
						assert( str == "Odd value from IOPollStr1");
				}
				else
				{
					if ((i % 2) == 0)
						assert( str == "Odd value from IOPollStr1");
					else
						assert( str == "Even value from IOPollStr1");
				}
			}
		}
		delete d_hist;	
				
		cout << "   Read command history (string) --> OK" << endl;
		
// Test command_history (array)

		d_hist = device->command_history("IOPollArray2",hist_depth);

		short first_val_first_rec;		
		for (i = 0;i < hist_depth;i++)
		{
			vector<short> vect;
			(*d_hist)[i] >> vect;
			
			if (print == true)
			{
				cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
				cout << "Value 0 = " << vect[0] << ", Value 1 = " << vect[1] << endl;
				TimeVal &t = (*d_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}

			if (i == 0)
				first_val_first_rec = vect[0];
							
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			if (i != 0)
			{
				if (first_val_first_rec == 100)
				{
					if ((i % 2) == 0)
					{
						assert( vect[0] == 100 );
						assert( vect[1] == 200 );
					}
					else
					{
						assert( vect[0] == 300 );
						assert( vect[1] == 400 );
					}
				}
				else
				{
					if ((i % 2) == 0)
					{
						assert( vect[0] == 300);
						assert( vect[1] == 400);
					}
					else
					{
						assert( vect[0] == 100 );
						assert( vect[1] == 200 );
					}
				}
			}
		}
		delete d_hist;	
				
		cout << "   Read command history (array) --> OK" << endl;
		
// Test command_history with exception

		d_hist = device->command_history("IOExcept",hist_depth);
		
		for (i = 0;i < hist_depth;i++)
		{
			if (print == true)
			{
				cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
				TimeVal &t = (*d_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*d_hist)[i].has_failed() == true);
			assert( (*d_hist)[i].get_err_stack().length() == 1);
			assert( !strcmp((*d_hist)[i].get_err_stack()[0].reason ,"API_ThrowException") );
		}
		delete d_hist;	
				
		cout << "   Read command history with exception --> OK" << endl;

		
// Test attribute_history (for long)

		vector<DeviceAttributeHistory> *a_hist;
		
		a_hist = device->attribute_history("PollLong_attr",hist_depth);

		long first_val;		
		for (i = 0;i < hist_depth;i++)
		{
			long lo;
			(*a_hist)[i] >> lo;
			
			if (i == 0)
				first_val = lo;
			
			if (print == true)
			{
				cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
				cout << "Value = " << lo << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*a_hist)[i].has_failed() == false);
			assert( (*a_hist)[i].get_err_stack().length() == 0);
			
//			AttributeDimension dim;
//			dim = (*a_hist)[i].get_r_dimension();
			
			assert ( (*a_hist)[i].get_dim_x() == 1);
			assert ( (*a_hist)[i].get_dim_y() == 0);
			
//			dim = (*a_hist)[i].get_w_dimension();
			
//			assert ( dim.dim_x == 0);
//			assert ( dim.dim_y == 0);
			
			
			if (first_val == 5555)
			{
				if ((i % 2) == 0)
					assert( lo == 5555 );
				else
					assert( lo == 6666 );
			}
			else
			{
				if ((i % 2) == 0)
					assert( lo == 6666 );
				else
					assert( lo == 5555 );
			}
		}
		delete a_hist;	
				
		cout << "   Read attribute history (long) --> OK" << endl;
		
// Test attribute_history (for strings spectrum)
		
		a_hist = device->attribute_history("PollString_spec_attr",hist_depth);

		string first_string;		
		for (i = 0;i < hist_depth;i++)
		{
			vector<string> str;
			(*a_hist)[i] >> str;

			if (i == 0)
				first_string = str[0];
							
			if (print == true)
			{
				cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
				cout << "Value = " << str[0] << ", Value = " << str[1] << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
			
			assert( (*a_hist)[i].has_failed() == false);
			assert( (*a_hist)[i].get_err_stack().length() == 0);
			
//			AttributeDimension dim;
//			dim = (*a_hist)[i].get_r_dimension();
			
			assert ( (*a_hist)[i].get_dim_x() == 2);
			assert ( (*a_hist)[i].get_dim_y() == 0);
			
//			dim = (*a_hist)[i].get_w_dimension();
			
//			assert ( dim.dim_x == 0);
//			assert ( dim.dim_y == 0);
			
			if (i != 0)
			{
				if (first_string == "Hello world")
				{
					if ((i % 2) == 0)
					{
						assert( str[0] == "Hello world" );
						assert( str[1] == "Hello universe" );
					}
					else
					{
						assert( str[0] == "Hello Grenoble" );
						assert( str[1] == "Hello Paris" );
					}
				}
				else
				{
					if ((i % 2) == 0)
					{
						assert( str[0] == "Hello Grenoble");
						assert( str[1] == "Hello Paris");
					}
					else
					{
						assert( str[0] == "Hello world" );
						assert( str[1] == "Hello universe" );
					}
				}
			}
		}
		delete a_hist;	
				
		cout << "   Read attribute history (string spectrum) --> OK" << endl;

// Test attribute_history with exception

		a_hist = device->attribute_history("attr_wrong_type",hist_depth);
		
		for (i = 0;i < hist_depth;i++)
		{
			if (print == true)
			{
				cout << "Command failed = " << (*a_hist)[i].has_failed() << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << "Error level 0 reason = " << ((*a_hist)[i].get_err_stack())[0].reason << endl;
				cout << endl;
			}
			
			assert( (*a_hist)[i].has_failed() == true);
			assert( (*a_hist)[i].get_err_stack().length() == 1);
			assert( !strcmp((*a_hist)[i].get_err_stack()[0].reason ,"API_AttrOptProp") );
//			AttributeDimension dim;
//			dim = (*a_hist)[i].get_r_dimension();
			
			assert ( (*a_hist)[i].get_dim_x() == 0);
			assert ( (*a_hist)[i].get_dim_y() == 0);
			
//			dim = (*a_hist)[i].get_w_dimension();
			
//			assert ( dim.dim_x == 0);
//			assert ( dim.dim_y == 0);

		}
		delete a_hist;	
				
		cout << "   Read attribute history with exception --> OK" << endl;
		
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

			
	delete device;
	return 0;
}

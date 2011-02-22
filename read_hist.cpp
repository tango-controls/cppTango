/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>

#include <sys/time.h>
#include <time.h>

typedef enum
{
	FIRST_EXCEPT = 0,
	SECOND_EXCEPT,
	FIRST_DATA,
	SECOND_DATA
}AttrResult;

typedef enum
{
	EXCEPT = 0,
	FIRST_STR,
	SECOND_STR
}CmdResult;

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

		int hist_depth = 10;
		d_hist = device->command_history("IOPollStr1",hist_depth);
		
		if (d_hist->size() < 4)
		{
			cout << "Not enought data in polling buffer, restart later" << endl;
			exit(-1);
		}

		CmdResult cr;

		if ((*d_hist)[0].has_failed() == true)
		{
			cr = EXCEPT;
		}
		else
		{
			string str;
			(*d_hist)[0] >> str;
			if (str[0] == 'E')
				cr = FIRST_STR;
			else
				cr = SECOND_STR;
		}

		for (i = 0;i < d_hist->size();i++)
		{
			string str;
			if ((*d_hist)[i].has_failed() == false)
				(*d_hist)[i] >> str;
			
			if (print == true)
			{
				cout << "Command failed = " << (*d_hist)[i].has_failed() << endl;
				if ((*d_hist)[i].has_failed() == false)
				{
					(*d_hist)[i] >> str;
					cout << "Value = " << str << endl;
				}
				TimeVal &t = (*d_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*d_hist)[i].get_err_stack().length() << endl;
				cout << endl;
			}
		}

		DevErrorList del;
		string simple_str;

		switch(cr)
		{
			case EXCEPT:
			assert( (*d_hist)[0].has_failed() == true);
			assert( (*d_hist)[0].get_err_stack().length() == 1);
			del = (*d_hist)[0].get_err_stack();
			assert( ::strcmp(del[0].desc.in(),"www") == 0);

			(*d_hist)[1] >> simple_str;
			assert( (*d_hist)[1].has_failed() == false);
			assert( (*d_hist)[1].get_err_stack().length() == 0);
			assert( simple_str == "Even value from IOPollStr1");

			(*d_hist)[2] >> simple_str;
			assert( (*d_hist)[2].has_failed() == false);
			assert( (*d_hist)[2].get_err_stack().length() == 0);
			assert( simple_str == "Odd value from IOPollStr1");
			break;

			case FIRST_STR:
			(*d_hist)[0] >> simple_str;
			assert( (*d_hist)[0].has_failed() == false);
			assert( (*d_hist)[0].get_err_stack().length() == 0);
			assert( simple_str == "Even value from IOPollStr1");

			(*d_hist)[1] >> simple_str;
			assert( (*d_hist)[1].has_failed() == false);
			assert( (*d_hist)[1].get_err_stack().length() == 0);
			assert( simple_str == "Odd value from IOPollStr1");

			assert( (*d_hist)[2].has_failed() == true);
			assert( (*d_hist)[2].get_err_stack().length() == 1);
			del = (*d_hist)[2].get_err_stack();
			assert( ::strcmp(del[0].desc.in(),"www") == 0);
			break;

			case SECOND_STR:
			(*d_hist)[0] >> simple_str;
			assert( (*d_hist)[0].has_failed() == false);
			assert( (*d_hist)[0].get_err_stack().length() == 0);
			assert( simple_str == "Odd value from IOPollStr1");

			assert( (*d_hist)[1].has_failed() == true);
			assert( (*d_hist)[1].get_err_stack().length() == 1);
			del = (*d_hist)[1].get_err_stack();
			assert( ::strcmp(del[0].desc.in(),"www") == 0);

			(*d_hist)[2] >> simple_str;
			assert( (*d_hist)[2].has_failed() == false);
			assert( (*d_hist)[2].get_err_stack().length() == 0);
			assert( simple_str == "Even value from IOPollStr1");
			break;
		}

		delete d_hist;	
				
		cout << "   Read command history (string) --> OK" << endl;
		
// Test command_history (array)

		d_hist = device->command_history("IOPollArray2",hist_depth);

		if (d_hist->size() < 4)
		{
			cout << "Not enought data in polling buffer, restart later" << endl;
			exit(-1);
		}
		
		short first_val_first_rec;		
		for (i = 0;i < d_hist->size();i++)
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
		
		for (i = 0;i < d_hist->size();i++)
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

// Test command_history for State

		d_hist = device->command_history("State",hist_depth);
		
		for (i = 0;i < d_hist->size();i++)
		{
			if (print == true)
			{
				cout << (*d_hist)[i] << endl;
			}
			
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			Tango::DevState ds;
			(*d_hist)[i] >> ds;
			assert( ds == Tango::ON );
		}
		delete d_hist;	
				
		cout << "   Read command history (State) --> OK" << endl;

// Test command_history for Status

		d_hist = device->command_history("Status",hist_depth);
		
		for (i = 0;i < d_hist->size();i++)
		{
			if (print == true)
			{
				cout << (*d_hist)[i] << endl;
			}
			
			assert( (*d_hist)[i].has_failed() == false);
			assert( (*d_hist)[i].get_err_stack().length() == 0);
			string str;
			(*d_hist)[i] >> str;
			assert( ::strcmp(str.c_str(),"The device is in ON state.") == 0 );
		}
		delete d_hist;	
		
		cout << "   Read command history (Status) --> OK" << endl;

// Test attribute_history (for long)

		vector<DeviceAttributeHistory> *a_hist;
hist_depth = 10;
		a_hist = device->attribute_history("PollLong_attr",hist_depth);

		DevLong first_val;		
		for (i = 0;i < a_hist->size();i++)
		{
			DevLong lo;
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
			
			assert ( (*a_hist)[i].get_dim_x() == 1);
			assert ( (*a_hist)[i].get_dim_y() == 0);
			
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
		AttrResult ar;		

		vector<string> str;

		if ((*a_hist)[0].has_failed() == true)
		{
			if (::strcmp(((*a_hist)[0].get_err_stack())[0].reason.in(),"aaaa") == 0)
				ar = FIRST_EXCEPT;
			else
				ar = SECOND_EXCEPT;
		}
		else
		{
			(*a_hist)[0] >> str;
			if (str.size() == 2)
				ar = FIRST_DATA;
			else
				ar = SECOND_DATA;
		}

		for (i = 0;i < a_hist->size();i++)
		{							
			if (print == true)
			{
				cout << "Attribute failed = " << (*a_hist)[i].has_failed() << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				if ((*a_hist)[i].has_failed() == false)
				{
					(*a_hist)[i] >> str;
					cout << "Value = " << str[0];
					if (str.size() == 2)
						cout << ", Value = " << str[1];
					cout << endl;
				}
				else
				{
					cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
					cout << "Error level 0 reason = " << ((*a_hist)[i].get_err_stack())[0].reason << endl;
					cout << "Error level 0 desc = " << ((*a_hist)[i].get_err_stack())[0].desc << endl;
				}
				cout << endl;
			}
		}

		switch(ar)
		{
			case FIRST_EXCEPT:
			assert( (*a_hist)[0].has_failed() == true );
			assert( (*a_hist)[0].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[0].get_err_stack())[0].desc.in(),"bbb") == 0 );
			assert( ::strcmp(((*a_hist)[0].get_err_stack())[0].reason.in(),"aaaa") == 0 );
			
			assert( (*a_hist)[1].has_failed() == true );
			assert( (*a_hist)[1].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[1].get_err_stack())[0].desc.in(),"yyy") == 0 );
			assert( ::strcmp(((*a_hist)[1].get_err_stack())[0].reason.in(),"xxx") == 0 );

			(*a_hist)[2] >> str;
			assert( str.size() == 2 );
			assert( str[0] == "Hello world" );
			assert( str[1] == "Hello universe");

			(*a_hist)[3] >> str;
			assert( str.size() == 1 );
			assert( str[0] == "Hello Grenoble" );
			break;

			case SECOND_EXCEPT:
			assert( (*a_hist)[0].has_failed() == true );
			assert( (*a_hist)[0].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[0].get_err_stack())[0].desc.in(),"yyy") == 0 );
			assert( ::strcmp(((*a_hist)[0].get_err_stack())[0].reason.in(),"xxx") == 0 );

			(*a_hist)[1] >> str;
			assert( str.size() == 2 );
			assert( str[0] == "Hello world" );
			assert( str[1] == "Hello universe");

			(*a_hist)[2] >> str;
			assert( str.size() == 1 );
			assert( str[0] == "Hello Grenoble" );
			
			assert( (*a_hist)[3].has_failed() == true );
			assert( (*a_hist)[3].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[3].get_err_stack())[0].desc.in(),"bbb") == 0 );
			assert( ::strcmp(((*a_hist)[3].get_err_stack())[0].reason.in(),"aaaa") == 0 );
			break;

			case FIRST_DATA:
			(*a_hist)[0] >> str;
			assert( str.size() == 2 );
			assert( str[0] == "Hello world" );
			assert( str[1] == "Hello universe");

			(*a_hist)[1] >> str;
			assert( str.size() == 1 );
			assert( str[0] == "Hello Grenoble" );
			
			assert( (*a_hist)[2].has_failed() == true );
			assert( (*a_hist)[2].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[2].get_err_stack())[0].desc.in(),"bbb") == 0 );
			assert( ::strcmp(((*a_hist)[2].get_err_stack())[0].reason.in(),"aaaa") == 0 );

			assert( (*a_hist)[3].has_failed() == true );
			assert( (*a_hist)[3].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[3].get_err_stack())[0].desc.in(),"yyy") == 0 );
			assert( ::strcmp(((*a_hist)[3].get_err_stack())[0].reason.in(),"xxx") == 0 );
			break;

			case SECOND_DATA:
			(*a_hist)[0] >> str;
			assert( str.size() == 1 );
			assert( str[0] == "Hello Grenoble" );
			
			assert( (*a_hist)[1].has_failed() == true );
			assert( (*a_hist)[1].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[1].get_err_stack())[0].desc.in(),"bbb") == 0 );
			assert( ::strcmp(((*a_hist)[1].get_err_stack())[0].reason.in(),"aaaa") == 0 );

			assert( (*a_hist)[2].has_failed() == true );
			assert( (*a_hist)[2].get_err_stack().length() == 1);
			assert( ::strcmp(((*a_hist)[2].get_err_stack())[0].desc.in(),"yyy") == 0 );
			assert( ::strcmp(((*a_hist)[2].get_err_stack())[0].reason.in(),"xxx") == 0 );

			(*a_hist)[3] >> str;
			assert( str.size() == 2 );
			assert( str[0] == "Hello world" );
			assert( str[1] == "Hello universe");
			break;
		}
		delete a_hist;	
				
		cout << "   Read attribute history (string spectrum) --> OK" << endl;
		
// Test attribute_history (for DevEncoded)

#ifndef COMPAT
		vector<DeviceAttributeHistory> *enc_hist;
		hist_depth = 10;
		enc_hist = device->attribute_history("Encoded_attr",hist_depth);
	
		for (i = 0;i < enc_hist->size();i++)
		{
			
			if (print == true)
			{
				cout << "Value = " << (*enc_hist)[i] << endl;
				cout << endl;
			}
			
			assert( (*enc_hist)[i].has_failed() == false);
			assert( (*enc_hist)[i].get_err_stack().length() == 0);
			
			assert ( (*enc_hist)[i].get_dim_x() == 1);
			assert ( (*enc_hist)[i].get_dim_y() == 0);

			DevEncoded enc;
			(*enc_hist)[i] >> enc;
			
			assert (::strcmp(enc.encoded_format,"Which format?") == 0);
			assert (enc.encoded_data[0] == 97);
			assert (enc.encoded_data[1] == 98);
			assert (enc.encoded_data[2] == 99);
			assert (enc.encoded_data[3] == 100);
		}
		delete enc_hist;	
				
		cout << "   Read attribute history (DevEncoded) --> OK" << endl;
#endif

// Test attribute_history with exception

		a_hist = device->attribute_history("attr_wrong_type",hist_depth);
		
		for (i = 0;i < a_hist->size();i++)
		{
			if (print == true)
			{
				cout << "Command failed = " << (*a_hist)[i].has_failed() << endl;
				TimeVal &t = (*a_hist)[i].get_date();
				cout << "Date : " << t.tv_sec << " sec, " << t.tv_usec << " usec" << endl;
				cout << "Error stack depth = " << (*a_hist)[i].get_err_stack().length() << endl;
				cout << "Error level 0 reason = " << ((*a_hist)[i].get_err_stack())[0].reason << endl;
				cout << "Error level 0 desc = " << ((*a_hist)[i].get_err_stack())[0].desc << endl;
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

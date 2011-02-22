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
	
	if (argc != 3)
	{
		cout << "usage: %s device loop" << endl;
		exit(-1);
	}

	string device_name = argv[1];
	int loop = atoi(argv[2]);

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

// Test void

	for (i = 0;i < loop;i++)
	{
		try
		{	
			device->command_inout("IOVoid");
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
	}
	cout << "   Void --> OK" << endl;
		
// Test boolean

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		bool in = true;
		din << in;
		try
		{	
			dout = device->command_inout("IOBool",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		bool received;
		int data_type = dout.get_type();
		dout >> received;
		assert( in != received );
		assert( data_type == Tango::DEV_BOOLEAN);
	}
	cout << "   Boolean --> OK" << endl;

// test short

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		short in = 2;
		din << in;
		try
		{	
			dout = device->command_inout("IOShort",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		short received;
		dout >> received;
		int data_type = dout.get_type();
		assert( received == (in * 2) );
		assert(data_type == Tango::DEV_SHORT );
	}
	cout << "   Short --> OK" << endl;

// test long

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		long in = 3;
		din << in;
		try
		{	
			dout = device->command_inout("IOLong",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		long received;
		dout >> received;
		assert( received == (in * 2) );
	}
	cout << "   Long --> OK" << endl;

// test float

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		float in = (float)3.1;
		din << in;
		try
		{	
			dout = device->command_inout("IOFloat",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		float received;
		dout >> received;
		assert( received == (in * 2) );
	}
	cout << "   Float --> OK" << endl;
			
// test double

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		double in = 3.1;
		din << in;
		try
		{	
			dout = device->command_inout("IODouble",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		double received;
		dout >> received;
		assert( received == (in * 2) );
	}
	cout << "   Double --> OK" << endl;

// test unsigned short

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		unsigned short in = 100;
		din << in;
		try
		{	
			dout = device->command_inout("IOUShort",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned short received;
		dout >> received;
		assert( received == (in * 2) );
	}
	cout << "   Unsigned Short --> OK" << endl;

// test unsigned long

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		unsigned long in = 1000;
		din << in;
		try
		{	
			dout = device->command_inout("IOULong",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		unsigned long received;
		dout >> received;
		assert( received == (in * 2) );
	}
	cout << "   Unsigned Long --> OK" << endl;

// test C++ string

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		string str("abc");
		din << str;
		try
		{	
			dout = device->command_inout("IOString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		string received;
		dout >> received;
		assert( received == "cba" );
	}
	cout << "   C++ string --> OK" << endl;
			
// test classical C string

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		const char *str = "abcd";
		din << str;
		try
		{	
			dout = device->command_inout("IOString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const char *received;
		dout >> received;
		assert( strcmp(str,"dcba") );
	}
	cout << "   C string --> OK" << endl;

// test char array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<unsigned char> in;
		in.push_back(1);
		in.push_back(2);
		din << in;
		try
		{	
			dout = device->command_inout("IOCharArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned char> received;
		dout >> received;
		int data_type = dout.get_type();
		assert( in[0] == received[1] );
		assert( in[1] == received[0] );
		assert( data_type == Tango::DEVVAR_CHARARRAY );
	}
	cout << "   vector of unsigned char --> OK" << endl;
	
// test DevVarCharArray

			
	for (i = 0;i < loop;i++)
	{
		const DevVarCharArray *received;
		DeviceData din,dout;
		
		DevVarCharArray *in = new DevVarCharArray(2);
		in->length(2);
		(*in)[0] = 10;
		(*in)[1] = 20;		
		din << in;

		try
		{	
			dout = device->command_inout("IOCharArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> received;

		assert( 10 == (*received)[1] );
		assert( 20 == (*received)[0] );		
	}

	for (i = 0;i < loop;i++)
	{
		const DevVarCharArray *received;
		DeviceData din,dout;
		
		DevVarCharArray in(2);
		in.length(2);
		in[0] = 10;
		in[1] = 20;		
		din << in;

		try
		{	
			dout = device->command_inout("IOCharArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		dout >> received;

		assert( in[0] == (*received)[1] );
		assert( in[1] == (*received)[0] );		
	}
	cout << "   DevVarCharArray (by pointer and reference) --> OK" << endl;
	
// test short array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<short> in;
		in.push_back(10);
		in.push_back(20);
		din << in;
		try
		{	
			dout = device->command_inout("IOShortArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<short> received;
		dout >> received;
		assert( received[0] == (in[0] * 2) );
		assert( received[1] == (in[1] * 2) );
	}
	cout << "   vector of short --> OK" << endl;
	
// test DevVarShortArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarShortArray *in = new DevVarShortArray(2);
		in->length(2);
		(*in)[0] = 1;
		(*in)[1] = 2;
		
		din << in;
		try
		{	
			dout = device->command_inout("IOShortArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarShortArray *received;
		dout >> received;
		assert( (*received)[0] == (1 * 2) );
		assert( (*received)[1] == (2 * 2) );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarShortArray in(2);
		in.length(2);
		in[0] = 1;
		in[1] = 2;
		
		din << in;
		try
		{	
			dout = device->command_inout("IOShortArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarShortArray *received;
		dout >> received;
		assert( (*received)[0] == (in[0] * 2) );
		assert( (*received)[1] == (in[1] * 2) );
	}
	cout << "   DevVarShortArray (by pointer and reference) --> OK" << endl;
	
// test long array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<long> in;
		in.push_back(100);
		in.push_back(200);
		din << in;
		try
		{	
			dout = device->command_inout("IOLongArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<long> received;
		dout >> received;
		assert( received[0] == (in[0] * 2) );
		assert( received[1] == (in[1] * 2) );
	}
	cout << "   vector of long --> OK" << endl;
	
// test DevVarLongArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarLongArray *in = new DevVarLongArray(2);
		in->length(2);
		(*in)[0] = 11;
		(*in)[1] = 22;
		din << in;
		try
		{	
			dout = device->command_inout("IOLongArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarLongArray *received;
		dout >> received;
		assert( (*received)[0] == (11 * 2) );
		assert( (*received)[1] == (22 * 2) );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarLongArray in(2);
		in.length(2);
		in[0] = 11;
		in[1] = 22;
		din << in;
		try
		{	
			dout = device->command_inout("IOLongArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarLongArray *received;
		dout >> received;
		assert( (*received)[0] == (in[0] * 2) );
		assert( (*received)[1] == (in[1] * 2) );
	}
	cout << "   DevVarLongArray (by pointer and reference) --> OK" << endl;
					
// test float array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<float> in;
		in.push_back((float)100.1);
		in.push_back((float)200.2);
		din << in;
		try
		{	
			dout = device->command_inout("IOFloatArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<float> received;
		dout >> received;
		assert( received[0] == (in[0] * 2) );
		assert( received[1] == (in[1] * 2) );
	}
	cout << "   vector of float --> OK" << endl;
	
// test DevVarFloatArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarFloatArray *in = new DevVarFloatArray(2);
		in->length(2);
		(*in)[0] = (float)1.11;
		(*in)[1] = (float)2.22;
		din << in;
		try
		{	
			dout = device->command_inout("IOFloatArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarFloatArray *received;
		dout >> received;
		assert( (*received)[0] == ((float)1.11 * 2) );
		assert( (*received)[1] == ((float)2.22 * 2) );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarFloatArray in(2);
		in.length(2);
		in[0] = (float)1.11;
		in[1] = (float)2.22;
		din << in;
		try
		{	
			dout = device->command_inout("IOFloatArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarFloatArray *received;
		dout >> received;
		assert( (*received)[0] == (in[0] * 2) );
		assert( (*received)[1] == (in[1] * 2) );
	}
	cout << "   DevVarFloatArray (by pointer and reference) --> OK" << endl;
		
// test double array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<double> in;
		in.push_back(1.234);
		in.push_back(2.111);
		din << in;
		try
		{	
			dout = device->command_inout("IODoubleArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<double> received;
		dout >> received;
		assert( received[0] == (in[0] * 2) );
		assert( received[1] == (in[1] * 2) );
	}
	cout << "   vector of double --> OK" << endl;

// test DevVarDoubleArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarDoubleArray *in = new DevVarDoubleArray(2);
		in->length(2);
		(*in)[0] = 1.12;
		(*in)[1] = 3.45;
		din << in;
		try
		{	
			dout = device->command_inout("IODoubleArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarDoubleArray *received;
		dout >> received;
		assert( (*received)[0] == ( 1.12 * 2) );
		assert( (*received)[1] == ( 3.45 * 2) );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarDoubleArray in(2);
		in.length(2);
		in[0] = 1.12;
		in[1] = 3.45;
		din << in;
		try
		{	
			dout = device->command_inout("IODoubleArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarDoubleArray *received;
		dout >> received;
		assert( (*received)[0] == (in[0] * 2) );
		assert( (*received)[1] == (in[1] * 2) );
	}
	cout << "   DevVarDoubleArray (by pointer and reference) --> OK" << endl;
		
// test unsigned short array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<unsigned short> in;
		in.push_back(100);
		in.push_back(200);
		din << in;
		try
		{	
			dout = device->command_inout("IOUShortArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned short> received;
		dout >> received;
		assert( received[0] == (in[0] * 2) );
		assert( received[1] == (in[1] * 2) );
	}
	cout << "   vector of unsigned short --> OK" << endl;
	
// test DevVarUShortArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarUShortArray *in = new DevVarUShortArray(2);
		in->length(2);
		(*in)[0] = 11;
		(*in)[1] = 22;
		din << in;
		try
		{	
			dout = device->command_inout("IOUShortArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarUShortArray *received;
		dout >> received;
		assert( (*received)[0] == (11 * 2) );
		assert( (*received)[1] == (22 * 2) );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarUShortArray in(2);
		in.length(2);
		in[0] = 11;
		in[1] = 22;
		din << in;
		try
		{	
			dout = device->command_inout("IOUShortArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarUShortArray *received;
		dout >> received;
		assert( (*received)[0] == (in[0] * 2) );
		assert( (*received)[1] == (in[1] * 2) );
	}
	cout << "   DevVarUShortArray (by pointer and reference) --> OK" << endl;
		
// test unsigned long array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<unsigned long> in;
		in.push_back(1000);
		in.push_back(2001);
		din << in;
		try
		{	
			dout = device->command_inout("IOULongArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<unsigned long> received;
		dout >> received;
		assert( received[0] == (in[0] * 2) );
		assert( received[1] == (in[1] * 2) );
	}
	cout << "   vector of unsigned long --> OK" << endl;
	
// test DevVarULongArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarULongArray *in = new DevVarULongArray(2);
		in->length(2);
		(*in)[0] = 111;
		(*in)[1] = 222;
		din << in;
		try
		{	
			dout = device->command_inout("IOULongArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarULongArray *received;
		dout >> received;
		assert( (*received)[0] == (111 * 2) );
		assert( (*received)[1] == (222 * 2) );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarULongArray in(2);
		in.length(2);
		in[0] = 111;
		in[1] = 222;
		din << in;
		try
		{	
			dout = device->command_inout("IOULongArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarULongArray *received;
		dout >> received;
		assert( (*received)[0] == (in[0] * 2) );
		assert( (*received)[1] == (in[1] * 2) );
	}
	cout << "   DevVarULongArray (by pointer and reference) --> OK" << endl;
		
// test string array

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<string> in;
		in.push_back("abc");
		in.push_back("wxyz");
		din << in;
		try
		{	
			dout = device->command_inout("IOStringArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<string> received;
		int data_type = dout.get_type();
		dout >> received;
		
		assert( received[0] == in[1] );
		assert( received[1] == in[0] );
		assert( data_type == Tango::DEVVAR_STRINGARRAY );
	}
	cout << "   vector of string --> OK" << endl;
	
// test DevVarStringArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarStringArray *in = new DevVarStringArray(2);
		in->length(2);
		(*in)[0] = CORBA::string_dup("abc");
		(*in)[1] = CORBA::string_dup("def");
		din << in;
		try
		{	
			dout = device->command_inout("IOStringArray",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarStringArray *received;
		dout >> received;
		assert( !strcmp((*received)[0],"def") );
		assert( !strcmp((*received)[1],"abc") );
	}
	cout << "   DevVarStringArray --> OK" << endl;
	
// test vector of long and vector of string

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<long> in1;
		in1.push_back(1000);
		in1.push_back(2001);
		in1.push_back(2002);
		vector<string> in2;
		in2.push_back("abc");
		in2.push_back("def");
		din.insert(in1,in2);
		try
		{	
			dout = device->command_inout("IOLongString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<long> received1;
		vector<string> received2;
		dout.extract(received1,received2);
		
		assert( received2[0] == in2[0] );
		assert( received2[1] == in2[1] );
		assert( received1[0] == (in1[0] * 2) );
		assert( received1[1] == (in1[1] * 2) );
		assert( received1[2] == (in1[2] * 2) );
	}
	cout << "   vector of long, string --> OK" << endl;
	
// test DevVarLongStringArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarLongStringArray *in = new DevVarLongStringArray();
		in->lvalue.length(2);
		in->lvalue[0] = 111;
		in->lvalue[1] = 222;
		in->svalue.length(2);
		in->svalue[0] = CORBA::string_dup("zxc");
		in->svalue[1] = CORBA::string_dup("qwe");
		din << in;
		try
		{	
			dout = device->command_inout("IOLongString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarLongStringArray *received;
		dout >> received;
		
		assert( received->lvalue[0] == (111 * 2) );
		assert( received->lvalue[1] == (222 * 2) );
		assert( !strcmp(received->svalue[0],"zxc") );
		assert( !strcmp(received->svalue[1],"qwe") );
	}

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarLongStringArray in;
		in.lvalue.length(2);
		in.lvalue[0] = 111;
		in.lvalue[1] = 222;
		in.svalue.length(2);
		in.svalue[0] = CORBA::string_dup("zxc");
		in.svalue[1] = CORBA::string_dup("qwe");
		din << in;
		try
		{	
			dout = device->command_inout("IOLongString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarLongStringArray *received;
		dout >> received;
		
		assert( received->lvalue[0] == (in.lvalue[0] * 2) );
		assert( received->lvalue[1] == (in.lvalue[1] * 2) );
		assert( !strcmp(received->svalue[0],in.svalue[0]) );
		assert( !strcmp(received->svalue[1],in.svalue[1]) );
	}
	cout << "   DevVarLongStringArray (by pointer and reference) --> OK" << endl;
		
// test vector of double and vector of string

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		vector<double> in1;
		in1.push_back(10.0);
		in1.push_back(20.1);
		in1.push_back(20.2);
		vector<string> in2;
		in2.push_back("abc");
		in2.push_back("def");
		din.insert(in1,in2);
		try
		{	
			dout = device->command_inout("IODoubleString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		vector<double> received1;
		vector<string> received2;
		dout.extract(received1,received2);
		int data_type = dout.get_type();
		
		assert( received2[0] == in2[0] );
		assert( received2[1] == in2[1] );
		assert( received1[0] == (in1[0] * 2) );
		assert( received1[1] == (in1[1] * 2) );
		assert( received1[2] == (in1[2] * 2) );
		assert( data_type == Tango::DEVVAR_DOUBLESTRINGARRAY );
	}
	cout << "   vector of double, string --> OK" << endl;
	
// test DevVarDoubleStringArray

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarDoubleStringArray *in = new DevVarDoubleStringArray();
		in->dvalue.length(2);
		in->dvalue[0] = 1.11;
		in->dvalue[1] = 22.2;
		in->svalue.length(3);
		in->svalue[0] = CORBA::string_dup("iop");
		in->svalue[1] = CORBA::string_dup("jkl");
		in->svalue[2] = CORBA::string_dup("bnm");
		din << in;
		try
		{	
			dout = device->command_inout("IODoubleString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarDoubleStringArray *received;
		dout >> received;
		
		assert( received->dvalue[0] == (1.11 * 2) );
		assert( received->dvalue[1] == (22.2 * 2) );
		assert( !strcmp(received->svalue[0],"iop") );
		assert( !strcmp(received->svalue[1],"jkl") );
		assert( !strcmp(received->svalue[2],"bnm") );
	}
	

	for (i = 0;i < loop;i++)
	{
		DeviceData din,dout;
		DevVarDoubleStringArray in;
		in.dvalue.length(2);
		in.dvalue[0] = 1.11;
		in.dvalue[1] = 22.2;
		in.svalue.length(3);
		in.svalue[0] = CORBA::string_dup("iop");
		in.svalue[1] = CORBA::string_dup("jkl");
		in.svalue[2] = CORBA::string_dup("bnm");
		din << in;
		try
		{	
			dout = device->command_inout("IODoubleString",din);
		}
		catch (CORBA::Exception &e)
		{
			Except::print_exception(e);
			exit(-1);
		}
		const DevVarDoubleStringArray *received;
		dout >> received;
		
		assert( received->dvalue[0] == (in.dvalue[0] * 2) );
		assert( received->dvalue[1] == (in.dvalue[1] * 2) );
		assert( !strcmp(received->svalue[0],in.svalue[0]) );
		assert( !strcmp(received->svalue[1],in.svalue[1]) );
		assert( !strcmp(received->svalue[2],in.svalue[2]) );
	}
	cout << "   DevVarDoubleStringArray (by pointer and reference) --> OK" << endl;
	
	delete device;
	
	return 0;
}

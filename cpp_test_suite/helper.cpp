/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>

#include <DeviceProxyHelper.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxyHelper *dev_helper;
	
	if (argc != 3)
	{
		cout << "usage: %s device loop" << endl;
		exit(-1);
	}

	string device_name = argv[1];
	long loop = atol(argv[2]);

	try 
	{
		dev_helper = new DeviceProxyHelper(device_name);
		cout << endl << "new DeviceProxy(" << dev_helper->get_device_proxy()->name() << ") returned" << endl << endl;


// test void
		for (int i = 0;i < loop;i++)
		{
			dev_helper->command("IOVoid");
		}
		cout << "   Void --> OK" << endl;

// test short

		for (int i = 0;i < loop;i++)
		{		
			short in_s = 2;
			short received_s;
			dev_helper->command_inout("IOShort",in_s,received_s);
			assert( received_s == (in_s * 2) );
		}
		cout << "   Short --> OK" << endl;
		
// tset long

		for (int i = 0;i < loop;i++)
		{
			long in_l = 3;
			long received_l;
			dev_helper->command_inout("IOLong",in_l,received_l);
			assert( received_l == (in_l * 2) );
		}
		cout << "   Long --> OK" << endl;

// test float

		for (int i = 0;i < loop;i++)
		{
			float in_f = (float)3.1;
			float received_f;
			dev_helper->command_inout("IOFloat",in_f,received_f);
			assert( received_f == (in_f * 2) );
		}
		cout << "   Float --> OK" << endl;

// test double

		for (int i = 0;i < loop;i++)
		{			
			double in_d = 3.1;
			double received_d;
			dev_helper->command_inout("IODouble",in_d,received_d);
			assert( received_d == (in_d * 2) );
		}
		cout << "   Double --> OK" << endl;


// test unsigned short

		for (int i = 0;i < loop;i++)
		{
			unsigned short in_us = 100;
			unsigned short received_us;
			dev_helper->command_inout("IOUShort",in_us,received_us);
			assert( received_us == (in_us * 2) );
		}
		cout << "   Unsigned Short --> OK" << endl;

// test unsigned long

		for (int i = 0;i < loop;i++)
		{
			unsigned long in_ul = 1000;
			unsigned long received_ul;
			dev_helper->command_inout("IOULong",in_ul,received_ul);
			assert( received_ul == (in_ul * 2) );
		}
		cout << "   Unsigned Long --> OK" << endl;

// test C++ string

		for (int i = 0;i < loop;i++)
		{
			string str("abc");
			string received_str;
			dev_helper->command_inout("IOString",str,received_str);
			assert( received_str == "cba" );
		}
		cout << "   C++ string --> OK" << endl;
			
// Test C string

		for (int i = 0;i < loop;i++)
		{
			const char *ch = "abcd";
			const char *received_ch;
			dev_helper->command_inout("IOString",ch,received_ch);
			assert( !strcmp(received_ch,"dcba") );
		}
		cout << "   C string --> OK" << endl;

// Test vector of unsigned char

		for (int i = 0;i < loop;i++)
		{
			vector<unsigned char> in_vuc;
			vector<unsigned char> received_vuc;
			in_vuc.push_back(1);
			in_vuc.push_back(2);
			dev_helper->command_inout("IOCharArray",in_vuc,received_vuc);
			assert( in_vuc[0] == received_vuc[1] );
			assert( in_vuc[1] == received_vuc[0] );
		}
		cout << "   vector of unsigned char --> OK" << endl;
	
// Test DevVarCharArray

		for (int i = 0;i < loop;i++)
		{
			const DevVarCharArray *received_dvca;		
			DevVarCharArray *in_dvca = new DevVarCharArray(2);
			in_dvca->length(2);
			(*in_dvca)[0] = 10;
			(*in_dvca)[1] = 20;		
			dev_helper->command_inout("IOCharArray",in_dvca,received_dvca);
			assert( 10 == (*received_dvca)[1] );
			assert( 20 == (*received_dvca)[0] );

			const DevVarCharArray *received_dvca2;		
			DevVarCharArray in_dvca2(2);
			in_dvca2.length(2);
			in_dvca2[0] = 10;
			in_dvca2[1] = 20;		
			dev_helper->command_inout("IOCharArray",in_dvca2,received_dvca2);
			assert( in_dvca2[0] == (*received_dvca2)[1] );
			assert( in_dvca2[1] == (*received_dvca2)[0] );		
		}
		cout << "   DevVarCharArray (by pointer and reference) --> OK" << endl;
	
// test short array

		for (int i = 0;i < loop;i++)
		{
			vector<short> in_vs;
			vector<short> received_vs;
			in_vs.push_back(10);
			in_vs.push_back(20);
			dev_helper->command_inout("IOShortArray",in_vs,received_vs);
			assert( received_vs[0] == (in_vs[0] * 2) );
			assert( received_vs[1] == (in_vs[1] * 2) );
		}
		cout << "   vector of short --> OK" << endl;
	
// test DevVarShortArray

		for (int i = 0;i < loop;i++)
		{
			DevVarShortArray *in_dvsa = new DevVarShortArray(2);
			const DevVarShortArray *received_dvsa;
			in_dvsa->length(2);
			(*in_dvsa)[0] = 1;
			(*in_dvsa)[1] = 2;
			dev_helper->command_inout("IOShortArray",in_dvsa,received_dvsa);
			assert( (*received_dvsa)[0] == (1 * 2) );
			assert( (*received_dvsa)[1] == (2 * 2) );

			DevVarShortArray in_dvsa2(2);
			const DevVarShortArray *received_dvsa2;
			in_dvsa2.length(2);
			in_dvsa2[0] = 1;
			in_dvsa2[1] = 2;
			dev_helper->command_inout("IOShortArray",in_dvsa2,received_dvsa2);
			assert( (*received_dvsa2)[0] == (in_dvsa2[0] * 2) );
			assert( (*received_dvsa2)[1] == (in_dvsa2[1] * 2) );
		}
		cout << "   DevVarShortArray (by pointer and reference) --> OK" << endl;
	
// test long array

		for (int i = 0;i < loop;i++)
		{
			vector<long> in_vl;
			vector<long> received_vl;
			in_vl.push_back(100);
			in_vl.push_back(200);
			dev_helper->command_inout("IOLongArray",in_vl,received_vl);
			assert( received_vl[0] == (in_vl[0] * 2) );
			assert( received_vl[1] == (in_vl[1] * 2) );
		}
		cout << "   vector of long --> OK" << endl;
	
// test DevVarLongArray

		for (int i = 0;i < loop;i++)
		{
			DevVarLongArray *in_dvla = new DevVarLongArray(2);
			const DevVarLongArray *received_dvla;
			in_dvla->length(2);
			(*in_dvla)[0] = 11;
			(*in_dvla)[1] = 22;
			dev_helper->command_inout("IOLongArray",in_dvla,received_dvla);
			assert( (*received_dvla)[0] == (11 * 2) );
			assert( (*received_dvla)[1] == (22 * 2) );

			DevVarLongArray in_dvla2(2);
			const DevVarLongArray *received_dvla2;
			in_dvla2.length(2);
			in_dvla2[0] = 11;
			in_dvla2[1] = 22;
			dev_helper->command_inout("IOLongArray",in_dvla2,received_dvla2);
			assert( (*received_dvla2)[0] == (in_dvla2[0] * 2) );
			assert( (*received_dvla2)[1] == (in_dvla2[1] * 2) );
		}
		cout << "   DevVarLongArray (by pointer and reference) --> OK" << endl;
					
// test float array

		for (int i = 0;i < loop;i++)
		{
			vector<float> in_vfl;
			vector<float> received_vfl;
			in_vfl.push_back((float)100.1);
			in_vfl.push_back((float)200.2);
			dev_helper->command_inout("IOFloatArray",in_vfl,received_vfl);
			assert( received_vfl[0] == (in_vfl[0] * 2) );
			assert( received_vfl[1] == (in_vfl[1] * 2) );
		}
		cout << "   vector of float --> OK" << endl;
	
// test DevVarFloatArray

		for (int i = 0;i < loop;i++)
		{
			DevVarFloatArray *in_dvfa = new DevVarFloatArray(2);
			const DevVarFloatArray *received_dvfa;
			in_dvfa->length(2);
			(*in_dvfa)[0] = (float)1.11;
			(*in_dvfa)[1] = (float)2.22;
			dev_helper->command_inout("IOFloatArray",in_dvfa,received_dvfa);
			assert( (*received_dvfa)[0] == ((float)1.11 * 2) );
			assert( (*received_dvfa)[1] == ((float)2.22 * 2) );

			DevVarFloatArray in_dvfa2(2);
			const DevVarFloatArray *received_dvfa2;
			in_dvfa2.length(2);
			in_dvfa2[0] = (float)1.11;
			in_dvfa2[1] = (float)2.22;
			dev_helper->command_inout("IOFloatArray",in_dvfa2,received_dvfa2);
			assert( (*received_dvfa2)[0] == (in_dvfa2[0] * 2) );
			assert( (*received_dvfa2)[1] == (in_dvfa2[1] * 2) );
		}
		cout << "   DevVarFloatArray (by pointer and reference) --> OK" << endl;
		
// test double array

		for (int i = 0;i < loop;i++)
		{
			vector<double> in_vdb;
			vector<double> received_vdb;
			in_vdb.push_back(1.234);
			in_vdb.push_back(2.111);
			dev_helper->command_inout("IODoubleArray",in_vdb,received_vdb);
			assert( received_vdb[0] == (in_vdb[0] * 2) );
			assert( received_vdb[1] == (in_vdb[1] * 2) );
		}
		cout << "   vector of double --> OK" << endl;

// test DevVarDoubleArray

		for (int i = 0;i < loop;i++)
		{
			DevVarDoubleArray *in_dvda = new DevVarDoubleArray(2);
			const DevVarDoubleArray *received_dvda;
			in_dvda->length(2);
			(*in_dvda)[0] = 1.12;
			(*in_dvda)[1] = 3.45;
			dev_helper->command_inout("IODoubleArray",in_dvda,received_dvda);
			assert( (*received_dvda)[0] == ( 1.12 * 2) );
			assert( (*received_dvda)[1] == ( 3.45 * 2) );

			DevVarDoubleArray in_dvda2(2);
			const DevVarDoubleArray *received_dvda2;
			in_dvda2.length(2);
			in_dvda2[0] = 1.12;
			in_dvda2[1] = 3.45;
			dev_helper->command_inout("IODoubleArray",in_dvda2,received_dvda2);
			assert( (*received_dvda2)[0] == (in_dvda2[0] * 2) );
			assert( (*received_dvda2)[1] == (in_dvda2[1] * 2) );
		}
		cout << "   DevVarDoubleArray (by pointer and reference) --> OK" << endl;

// test unsigned short array

		for (int i = 0;i < loop;i++)
		{
			vector<unsigned short> in_vus;
			vector<unsigned short> received_vus;
			in_vus.push_back(100);
			in_vus.push_back(200);
			dev_helper->command_inout("IOUShortArray",in_vus,received_vus);
			assert( received_vus[0] == (in_vus[0] * 2) );
			assert( received_vus[1] == (in_vus[1] * 2) );
		}
		cout << "   vector of unsigned short --> OK" << endl;
	
// test DevVarUShortArray

		for (int i = 0;i < loop;i++)
		{
			DevVarUShortArray *in_dvusa = new DevVarUShortArray(2);
			const DevVarUShortArray *received_dvusa;
			in_dvusa->length(2);
			(*in_dvusa)[0] = 11;
			(*in_dvusa)[1] = 22;
			dev_helper->command_inout("IOUShortArray",in_dvusa,received_dvusa);
			assert( (*received_dvusa)[0] == (11 * 2) );
			assert( (*received_dvusa)[1] == (22 * 2) );

			DevVarUShortArray in_dvusa2(2);
			const DevVarUShortArray *received_dvusa2;
			in_dvusa2.length(2);
			in_dvusa2[0] = 11;
			in_dvusa2[1] = 22;
			dev_helper->command_inout("IOUShortArray",in_dvusa2,received_dvusa2);
			assert( (*received_dvusa2)[0] == (in_dvusa2[0] * 2) );
			assert( (*received_dvusa2)[1] == (in_dvusa2[1] * 2) );
		}
		cout << "   DevVarUShortArray (by pointer and reference) --> OK" << endl;
		
// test unsigned long array

		for (int i = 0;i < loop;i++)
		{
			vector<unsigned long> in_vul;
			vector<unsigned long> received_vul;
			in_vul.push_back(1000);
			in_vul.push_back(2001);
			dev_helper->command_inout("IOULongArray",in_vul,received_vul);
			assert( received_vul[0] == (in_vul[0] * 2) );
			assert( received_vul[1] == (in_vul[1] * 2) );
		}
		cout << "   vector of unsigned long --> OK" << endl;
	
// test DevVarULongArray

		for (int i = 0;i < loop;i++)
		{
			DevVarULongArray *in_dvula = new DevVarULongArray(2);
			const DevVarULongArray *received_dvula;
			in_dvula->length(2);
			(*in_dvula)[0] = 111;
			(*in_dvula)[1] = 222;
			dev_helper->command_inout("IOULongArray",in_dvula,received_dvula);
			assert( (*received_dvula)[0] == (111 * 2) );
			assert( (*received_dvula)[1] == (222 * 2) );

			DevVarULongArray in_dvula2(2);
			const DevVarULongArray *received_dvula2;
			in_dvula2.length(2);
			in_dvula2[0] = 111;
			in_dvula2[1] = 222;
			dev_helper->command_inout("IOULongArray",in_dvula2,received_dvula2);
			assert( (*received_dvula2)[0] == (in_dvula2[0] * 2) );
			assert( (*received_dvula2)[1] == (in_dvula2[1] * 2) );
		}
		cout << "   DevVarULongArray (by pointer and reference) --> OK" << endl;
		
// test string array

		for (int i = 0;i < loop;i++)
		{
			vector<string> in_vstr;
			vector<string> received_vstr;
			in_vstr.push_back("abc");
			in_vstr.push_back("wxyz");
			dev_helper->command_inout("IOStringArray",in_vstr,received_vstr);
			assert( received_vstr[0] == in_vstr[1] );
			assert( received_vstr[1] == in_vstr[0] );
		}
		cout << "   vector of string --> OK" << endl;
	
// test DevVarStringArray

		for (int i = 0;i < loop;i++)
		{
			DevVarStringArray *in_dvstra = new DevVarStringArray(2);
			const DevVarStringArray *received_dvstra;
			in_dvstra->length(2);
			(*in_dvstra)[0] = CORBA::string_dup("abc");
			(*in_dvstra)[1] = CORBA::string_dup("def");
			dev_helper->command_inout("IOStringArray",in_dvstra,received_dvstra);
			assert( !strcmp((*received_dvstra)[0],"def") );
			assert( !strcmp((*received_dvstra)[1],"abc") );
		}
		cout << "   DevVarStringArray --> OK" << endl;
		


		
		delete dev_helper;
	}
        catch (CORBA::Exception &e)
        {
              	Except::print_exception(e);
		exit(1);
        }
}

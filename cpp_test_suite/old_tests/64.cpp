#include <iostream>
#include <tango.h>
#include <string>

using namespace std;

bool verbose = false;

#define	coutv	if (verbose == true) cout

typedef enum Client_Server
{
	C32_S32,
	C64_S64,
	C32_S64,
	C64_S32
};

int main(int argc,char *argv[])
{
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
		Tango::DeviceProxy dev(device_name);
		coutv << "DeviceProxy created" << endl;

// First get bus server computer bus size

		Tango::DeviceData bs;
		bs = dev.command_inout("GetLongSize");
		short b_size;
		bs >> b_size;

		Client_Server trans;				
#ifdef TANGO_LONG64
		if (b_size == 32)
		{
			trans = C64_S32;
			coutv << "Client 64 bits - Server 32 bits" << endl;
		}
		else
		{
			trans = C64_S64;
			coutv << "Client 64 bits - Server 64 bits" << endl;
		}
#else
		if (b_size == 64)
		{
			trans = C32_S64;
			coutv << "Client 32 bits - Server 64 bits" << endl;
		}
		else
		{
			coutv << "Client 32 bits - Server 32 bits" << endl;
			trans = C32_S32;
		}
#endif	

		cout << "Trying blackbox" << endl;
		vector<string> *vs;
		int nb = 2;
		vs = dev.black_box(nb);
		
		for (unsigned int l=0;l < vs->size();++l)
			cout << "Backbox elt = " << (*vs)[l] << endl;
		delete vs;
		
		Tango::DeviceData in,out;

// Trying long 
			
		bool found = false;
		long lg = 15;
		long received_long;
		in << lg;
		found = false;
		try
		{
			out = dev.command_inout("IOLong",in);
			out.set_exceptions(Tango::DeviceData::wrongtype_flag);
			out >> received_long;

			coutv << "Received value = " << received_long << endl;			
			if ((trans == C32_S32) || (trans == C64_S64))
				assert(received_long == 30);
		}
		catch(Tango::DevFailed &e)
		{
			if (verbose == true)
				Tango::Except::print_exception(e);
			string desc(e.errors[0].desc);
			if (trans == C64_S32)
			{
				if (desc.find("Data coming from 64 bits") != string::npos)
					found = true;
			}
			else
			{
				if (desc.find("64 bits") != string::npos)
					found = true;
			}
			assert (found == true);
		}
		cout << "DevLong --> OK" << endl;
		
// Trying unsigned long

		unsigned long ulg = 35;
		unsigned long received_ulong;
		in << ulg;
		found = false;
		try
		{
			out = dev.command_inout("IOULong",in);
			out.set_exceptions(Tango::DeviceData::wrongtype_flag);
			out >> received_ulong;

			coutv << "Received value = " << received_ulong << endl;			
			if ((trans == C32_S32) || (trans == C64_S64))
				assert(received_ulong == 70);
		}
		catch(Tango::DevFailed &e)
		{
			if (verbose == true)
				Tango::Except::print_exception(e);
			string desc(e.errors[0].desc);
			if (trans == C64_S32)
			{
				if (desc.find("Data coming from 64 bits") != string::npos)
					found = true;
			}
			else
			{
				if (desc.find("64 bits") != string::npos)
					found = true;
			}
			assert (found == true);
		}
		
		cout << "DevULong --> OK" << endl;

// Trying long array

		vector<long> vlg;
		vector<long> rec_vlg;
		vlg.push_back(77);
		vlg.push_back(88);
		vlg.push_back(99);
		in << vlg;
		found = false;
		try
		{
			out = dev.command_inout("IOLongArray",in);
			out.set_exceptions(Tango::DeviceData::wrongtype_flag);
			out >> rec_vlg;
			
			for (unsigned int i = 0;i < rec_vlg.size();i++)
				coutv << "Received value = " << rec_vlg[i] << endl;			
			if ((trans == C32_S32) || (trans == C64_S64))
			{
				assert(rec_vlg[0] == 154);
				assert(rec_vlg[1] == 176);
				assert(rec_vlg[2] == 198);
			}
		}
		catch(Tango::DevFailed &e)
		{
			if (verbose == true)
				Tango::Except::print_exception(e);
			string desc(e.errors[0].desc);
			if (trans == C64_S32)
			{
				if (desc.find("Data coming from 64 bits") != string::npos)
					found = true;
			}
			else
			{
				if (desc.find("64 bits") != string::npos)
					found = true;
			}
			assert (found == true);
			
		}
		
		cout << "DevVarLongArray --> OK" << endl;

// Trying unsigned long array			

		vector<unsigned long> vulg;
		vector<unsigned long> rec_vulg;
		vulg.push_back(777);
		vulg.push_back(888);
		vulg.push_back(999);
		in << vulg;
		found = false;
		try
		{
			out = dev.command_inout("IOULongArray",in);
			out.set_exceptions(Tango::DeviceData::wrongtype_flag);
			out >> rec_vulg;
			
			for (unsigned int i = 0;i < rec_vulg.size();i++)
				coutv << "Received value = " << rec_vulg[i] << endl;			
			if ((trans == C32_S32) || (trans == C64_S64))
			{
				assert(rec_vulg[0] == 1554);
				assert(rec_vulg[1] == 1776);
				assert(rec_vulg[2] == 1998);
			}
					}
		catch(Tango::DevFailed &e)
		{
			if (verbose == true)
				Tango::Except::print_exception(e);
			string desc(e.errors[0].desc);
			if (trans == C64_S32)
			{
				if (desc.find("Data coming from 64 bits") != string::npos)
					found = true;
			}
			else
			{
				if (desc.find("64 bits") != string::npos)
					found = true;
			}
			assert (found == true);
			
		}
		
		cout << "DevVarULongArray --> OK" << endl;

// Trying LongString array

		vector<string> vsc;
		vsc.push_back("First str");
		vector<long> vl;
		vl.push_back((long)10);
		vl.push_back((long)20);
		vl.push_back((long)30);
		in.insert(vl,vsc);
		
		const Tango::DevVarLongStringArray *dvlsa;
		try
		{
			out = dev.command_inout("IOLongString",in);
			out.set_exceptions(Tango::DeviceData::wrongtype_flag);
			out >> dvlsa;
			
			for (unsigned int i = 0;i < dvlsa->svalue.length();i++)
				coutv << "Received string value = " << dvlsa->svalue[i] << endl;			
			for (unsigned int i = 0;i < dvlsa->lvalue.length();i++)
				coutv << "Received long value = " << dvlsa->lvalue[i] << endl;
				
			if ((trans == C32_S32) || (trans == C64_S64))
			{
				string re(dvlsa->svalue[0]);
				assert(re == "First str");
				assert(dvlsa->lvalue[0] == 20);
				assert(dvlsa->lvalue[1] == 40);
				assert(dvlsa->lvalue[2] == 60);
			}
			
		}
		catch(Tango::DevFailed &e)
		{
			if (verbose == true)
				Tango::Except::print_exception(e);
			string desc(e.errors[0].desc);
			if (trans == C64_S32)
			{
				if (desc.find("Data coming from 64 bits") != string::npos)
					found = true;
			}
			else
			{
				if (desc.find("64 bits") != string::npos)
					found = true;
			}
			assert (found == true);
			
		}
		
		cout << "DevVarLongStringArray --> OK" << endl;
						
// Trying long 

		out = dev.command_inout("OLong");
		bool extract;
		extract = (out >> received_long);
		
		if (extract == true)
			coutv << "Received long = " << received_long << endl;
		
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert(received_long == 22);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Received long --> OK" << endl;
						
// Trying unsigned long 

		out = dev.command_inout("OULong");
		extract = (out >> received_ulong);
		
		if (extract == true)
			coutv << "Received u long = " << received_ulong << endl;

		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert(received_ulong == 333);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Received unsigned long --> OK" << endl;
			
// Trying long array with extraction with vectors and pointer

		out = dev.command_inout("OLongArray");
		extract = (out >> rec_vlg);
		
		if (extract == true)
		{
			coutv << "Received " << rec_vlg.size() << " elt in array" << endl;
			for (unsigned int i = 0;i < rec_vlg.size();i++)
				coutv << "Array elt = " << rec_vlg[i] << endl;
		}

		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert(rec_vlg[0] == 555);
			assert(rec_vlg[1] == 556);
			assert(rec_vlg[2] == 557);
			assert(rec_vlg[3] == 558);
		}
		else
		{
			assert (extract == false);
		}
		cout << "Received long array in vector --> OK" << endl;
		
					
		out = dev.command_inout("OLongArray");
		const Tango::DevVarLongArray *dvla;
		extract = (out >> dvla);
		
		if (extract == true)
			coutv << "Received : " << *dvla << endl;
			
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert((*dvla)[0] == 555);
			assert((*dvla)[1] == 556);
			assert((*dvla)[2] == 557);
			assert((*dvla)[3] == 558);
		}
		else
		{
			assert(extract == false);
		}
		
		cout << "Received long array in pointer --> OK" << endl;
				
// Trying Unsigned long array with extraction with vectors and pointer

		out = dev.command_inout("OULongArray");
		extract = (out >> rec_vulg);
		
		if (extract == true)
		{
			coutv << "Received " << rec_vulg.size() << " elt in array" << endl;
			for (unsigned int i = 0;i < rec_vulg.size();i++)
				coutv << "Array elt = " << rec_vulg[i] << endl;
		}
		
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert(rec_vulg[0] == 777);
			assert(rec_vulg[1] == 778);
			assert(rec_vulg[2] == 779);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Received unsigned long array in vector --> OK" << endl;
					
		out = dev.command_inout("OULongArray");
		const Tango::DevVarULongArray *dvula;
		extract = (out >> dvula);
		
		if (extract == true)
			coutv << "Received : " << *dvula << endl;
			
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert((*dvula)[0] == 777);
			assert((*dvula)[1] == 778);
			assert((*dvula)[2] == 779);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Received unsigned long array in pointer --> OK" << endl;
		
// Trying LongString array with extraction with vectors and pointer
		
		out = dev.command_inout("OLongString");
		vector<long> v1;
		vector<string> vs1;
		extract = out.extract(v1,vs1);
		
		if (extract == true)
		{
			coutv << "Received " << vs1.size() << " string elt in array" << endl;
			for (unsigned int i = 0;i < vs1.size();i++)
				coutv << "Array elt = " << vs1[i] << endl;
			coutv << "Received " << v1.size() << " long elt in array" << endl;
			for (unsigned int i = 0;i < v1.size();i++)
				coutv << "Array elt = " << v1[i] << endl;
		}
		
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			assert(vs1[0] == "Hola todos");
			assert(v1[0] == 999);
			assert(v1[1] == 1000);
			assert(v1[2] == 1001);
			assert(v1[3] == 1002);
			assert(v1[4] == 1003);
			assert(v1[5] == 1004);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Received long/string array in vectors --> OK" << endl;
					
		out = dev.command_inout("OLongString");
		const Tango::DevVarLongStringArray *d1;
		extract = (out >> d1);
		
		if (extract == true)
		{
			coutv << "Received string : " << d1->svalue << endl;
			coutv << "Received long : " << d1->lvalue << endl;
		}
		
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			string se(d1->svalue[0]);
			assert(se == "Hola todos");
			assert(d1->lvalue[0] == 999);
			assert(d1->lvalue[1] == 1000);
			assert(d1->lvalue[2] == 1001);
			assert(d1->lvalue[3] == 1002);
			assert(d1->lvalue[4] == 1003);
			assert(d1->lvalue[5] == 1004);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Received long/string array in pointer --> OK" << endl;					

//
// Try to read a long scalar READ attribute
//

		Tango::DeviceAttribute da;
		da = dev.read_attribute("Long_attr");
		long att;
//		da.set_exceptions(Tango::DeviceAttribute::wrongtype_flag);
		extract = da >> att;
		
		coutv << "Extract for long attribute : " << extract << endl;
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			coutv << "Attribute value : " << att << endl;
			assert(extract == true);
			assert(att == 1246);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Read SCALAR long attribute --> OK" << endl;					

//
// Try to read a long spectrum READ attribute
//

		Tango::DeviceAttribute db;
		db = dev.read_attribute("Long_spec_attr_rw");
		vector<long> v_att;
		da.set_exceptions(Tango::DeviceAttribute::wrongtype_flag);
		extract = db >> v_att;
		
		coutv << "Extract for long spectrum attribute : " << extract << endl;
		if ((trans == C32_S32) || (trans == C64_S64) || (trans == C64_S32))
		{
			coutv << "Attribute value length: " << v_att.size() << endl;
			for (unsigned int i = 0;i < v_att.size();i++)
				coutv << "Array elt = " << v_att[i] << endl;			assert(extract == true);
			assert(v_att[0] == 88);
			assert(v_att[1] == 99);
			assert(v_att[2] == 111);
			assert(v_att[3] == 0);
		}
		else
		{
			assert(extract == false);
		}
		cout << "Read SPECTRUM long attribute --> OK" << endl;					

//
// Try to write a long spectrum READ attribute
//

		vector<long> v_att_w;
		v_att_w.push_back(444);
		v_att_w.push_back(555);
		
		Tango::DeviceAttribute dw("Long_spec_attr_rw",v_att_w);
		try
		{
			dev.write_attribute(dw);
			if (trans != C32_S64)
			{
				db = dev.read_attribute("Long_spec_attr_rw");
				vector<long> v_att_r;
				db.set_exceptions(Tango::DeviceAttribute::wrongtype_flag);
				extract = db >> v_att_r;

				coutv << "Extract for long spectrum attribute (after write) : " << extract << endl;
				if ((trans == C32_S32) || (trans == C64_S64))
				{
					coutv << "Attribute value length: " << v_att_r.size() << endl;
					for (unsigned int i = 0;i < v_att_r.size();i++)
						coutv << "Array elt = " << v_att_r[i] << endl;			assert(extract == true);
					assert(v_att_r[0] == 88);
					assert(v_att_r[1] == 99);
					assert(v_att_r[2] == 111);
					assert(v_att_r[3] == 444);
					assert(v_att_r[4] == 555);
				}
				else
				{
					assert(extract == false);
				}
			}
			
			v_att_w.clear();
			v_att_w.push_back(0);
		
			dw << v_att_w;
			dev.write_attribute(dw);		
		}
		catch (Tango::DevFailed &e)
		{
			if (verbose == true)
				Tango::Except::print_exception(e);
			string desc(e.errors[0].desc);
			if (trans == C64_S32)
			{
				if (desc.find("Data sent by a 64 bits") != string::npos)
					found = true;
			}
			assert (found == true);
		}		
		cout << "Write SCALAR long attribute --> OK" << endl;

	}
	catch (Tango::DevFailed &e)
	{
		Tango::Except::print_exception(e);
	}
	
}

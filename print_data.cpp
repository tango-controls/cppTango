/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
		
// Test boolean

	DeviceData din;
	bool in = true;
	bool out;
	din << in;
	cout << "Data = " << din << endl;
	din >> out;
	assert (out == true);

	cout << "   Boolean --> OK" << endl;

// test short

	short s_in = 2;
	short s_out;
	din << s_in;
	cout << "Data = " << din << endl;
	din >> s_out;
	assert (s_out == 2);
	
	cout << "   Short --> OK" << endl;

// test long

	DevLong l_in = 3;
	DevLong l_out;
	din << l_in;
	cout << "Data = " << din << endl;
	din >> l_out;
	assert (l_out == 3);
	
	cout << "   Long --> OK" << endl;

// test float

	float f_in = (float)3.1;
	float f_out;
	din << f_in;
	cout << "Data = " << din << endl;
	din >> f_out;
	assert (f_out == (float)3.1);
	
	cout << "   Float --> OK" << endl;
			
// test double


	double db_in = 1.3;
	double db_out;
	din << db_in;
	cout << "Data = " << din << endl;
	din >> db_out;
	assert (db_out == 1.3);
	
	cout << "   Double --> OK" << endl;

// test unsigned short

	unsigned short us_in = 100;
	unsigned short us_out;
	din << us_in;
	cout << "Data = " << din << endl;
	din >> us_out;
	assert (us_out == 100);

	cout << "   Unsigned Short --> OK" << endl;

// test unsigned long

	DevULong ul_in = 1000;
	DevULong ul_out;
	din << ul_in;
	cout << "Data = " << din << endl;
	din >> ul_out;
	assert (ul_out == 1000);
	
	cout << "   Unsigned Long --> OK" << endl;

// test string

	string str("abc");
	string str_out;
	din << str;
	cout << "Data = " << din << endl;
	din >> str_out;
	assert (str_out == "abc");
	
	cout << "   String --> OK" << endl;

// test DevVarCharArray

	vector<unsigned char> ch_in;
	vector<unsigned char> ch_out;
	ch_in.push_back(0);
	ch_in.push_back(1);
	din << ch_in;
	cout << din << endl;
	din >> ch_out;
	assert (ch_out[0] == 0);
	assert (ch_out[1] == 1);
	
	cout << "   DevVarCharArray --> OK" << endl;
	
// test DevVarShortArray

	DevVarShortArray *sh_in = new DevVarShortArray(2);
	vector<short> sh_out;
	sh_in->length(2);
	(*sh_in)[0] = 10;
	(*sh_in)[1] = 20;
	din << sh_in;
	cout << din << endl;
	din >> sh_out;
	assert (sh_out[0] == 10);
	assert (sh_out[1] == 20);
	
	cout << "   DevVarShortArray --> OK" << endl;
	
// test DevVarLongArray

	DevVarLongArray *lg_arr = new DevVarLongArray(2);
	vector<DevLong> lg_arr_out;
	lg_arr->length(2);
	(*lg_arr)[0] = 111;
	(*lg_arr)[1] = 222;
	din << lg_arr;
	cout << din << endl;
	din >> lg_arr_out;
	assert (lg_arr_out[0] == 111);
	assert (lg_arr_out[1] == 222);
	
	cout << "   DevVarLongArray --> OK" << endl;
	
// test DevVarFloatArray

	DevVarFloatArray *fl_arr = new DevVarFloatArray(2);
	vector<float> fl_arr_out;
	fl_arr->length(2);
	(*fl_arr)[0] = (float)1.11;
	(*fl_arr)[1] = (float)2.22;
	din << fl_arr;
	cout << din << endl;
	din >> fl_arr_out;
	assert (fl_arr_out[0] == (float)1.11);
	assert (fl_arr_out[1] == (float)2.22);
	
	cout << "   DevVarFloatArray --> OK" << endl;

// test DevVarDoubleArray

	DevVarDoubleArray *db_arr = new DevVarDoubleArray(2);
	vector<double> db_arr_out;
	db_arr->length(2);
	(*db_arr)[0] = 1.12;
	(*db_arr)[1] = 3.45;
	din << db_arr;
	cout << din << endl;
	din >> db_arr_out;
	assert (db_arr_out[0] == 1.12);
	assert (db_arr_out[1] == 3.45);
	
	cout << "   DevVarDoubleArray --> OK" << endl;
	
// test DevVarUShortArray

	DevVarUShortArray *us_arr = new DevVarUShortArray(2);
	vector<unsigned short>  us_arr_out;
	us_arr->length(2);
	(*us_arr)[0] = 11;
	(*us_arr)[1] = 22;
	din << us_arr;
	cout << din << endl;
	din >> us_arr_out;
	assert (us_arr_out[0] == 11);
	assert (us_arr_out[1] == 22);

	cout << "   DevVarUShortArray --> OK" << endl;
	
// test DevVarULongArray

	DevVarULongArray *ul_arr = new DevVarULongArray(2);
	vector<DevULong> ul_arr_out;
	ul_arr->length(2);
	(*ul_arr)[0] = 1111;
	(*ul_arr)[1] = 2222;
	din << ul_arr;
	cout << din << endl;
	din >> ul_arr_out;
	assert (ul_arr_out[0] == 1111);
	assert (ul_arr_out[1] == 2222);
	
	cout << "   DevVarULongArray --> OK" << endl;
	
// test DevVarStringArray

	DevVarStringArray *str_arr = new DevVarStringArray(2);
	vector<string> str_arr_out;
	str_arr->length(3);
	(*str_arr)[0] = CORBA::string_dup("abc");
	(*str_arr)[1] = CORBA::string_dup("def");
	(*str_arr)[2] = CORBA::string_dup("ghi");
	din << str_arr;
	cout << din << endl;
	din >> str_arr_out;
	assert (str_arr_out[0] == "abc");
	assert (str_arr_out[1] == "def");
	assert (str_arr_out[2] == "ghi");

	cout << "   DevVarStringArray --> OK" << endl;
	
// test DevVarLongStringArray

	DevVarLongStringArray *lgstr_arr = new DevVarLongStringArray();
	vector<DevLong> lg_lgstr;
	vector<string> str_lgstr;
	lgstr_arr->lvalue.length(2);
	lgstr_arr->lvalue[0] = 1110;
	lgstr_arr->lvalue[1] = 2220;
	lgstr_arr->svalue.length(2);
	lgstr_arr->svalue[0] = CORBA::string_dup("zxc");
	lgstr_arr->svalue[1] = CORBA::string_dup("qwe");
	din << lgstr_arr;
	cout << din << endl;
	din.extract(lg_lgstr,str_lgstr);
	assert (lg_lgstr[0] == 1110);
	assert (lg_lgstr[1] == 2220);
	assert (str_lgstr[0] == "zxc");
	assert (str_lgstr[1] == "qwe");
	
	cout << "   DevVarLongStringArray --> OK" << endl;
	
// test DevVarDoubleStringArray


	DevVarDoubleStringArray *dbstr_arr = new DevVarDoubleStringArray();
	vector<double> db_dbstr;
	vector<string> str_dbstr;
	dbstr_arr->dvalue.length(2);
	dbstr_arr->dvalue[0] = 1.11;
	dbstr_arr->dvalue[1] = 22.2;
	dbstr_arr->svalue.length(3);
	dbstr_arr->svalue[0] = CORBA::string_dup("iop");
	dbstr_arr->svalue[1] = CORBA::string_dup("jkl");
	dbstr_arr->svalue[2] = CORBA::string_dup("bnm");
	din << dbstr_arr;
	cout << din << endl;
	din.extract(db_dbstr,str_dbstr);
	assert (db_dbstr[0] == 1.11);
	assert (db_dbstr[1] == 22.2);
	assert (str_dbstr[0] == "iop");
	assert (str_dbstr[1] == "jkl");
	assert (str_dbstr[2] == "bnm");
	
	cout << "   DevVarDoubleStringArray --> OK" << endl;
	
// test DevState


	DevState sta = Tango::STANDBY;
	DevState sta_out;
	din << sta;
	cout << "State = " << din << endl;
	din >> sta_out;
	assert (sta_out == Tango::STANDBY);
	
	cout << "   DevState --> OK" << endl;
		
	
// Attribute

	DeviceAttribute da;
	cout << da << endl;
	
	short s_attr = 20;
	da << s_attr;
	cout << da << endl;
	
	DeviceProxy dev("dev/test/10");
	
	da = dev.read_attribute("Long_attr");
	cout << da  << endl;
	long la;
	da >> la;
	assert (la == 1246);
	
	
	return 0;
}

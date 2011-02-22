/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
		
// Test empty 

	DeviceData da;
	long lo;
	
	bitset<DeviceData::numFlags> flags;
	flags.reset(DeviceData::isempty_flag);	
	da.exceptions(flags);
	
	
	bool ret = da >> lo;
	assert ( ret == false );
	
	cout << "   Extraction from empty object --> OK" << endl;


	flags.set(DeviceData::isempty_flag);
	da.exceptions(flags);
		
	try
	{
		da >> lo;
		assert(false);
	}
	catch (Tango::DevFailed &)
	{
	}
	
	cout << "   Extraction from empty object (exception) --> OK" << endl;
	flags.reset();
		
// Test wrong type

	DeviceData db;
	long l = 2;
	db << l;
	
	float fl;
	
	ret = db >> fl;
	assert ( ret == false );
	
	cout << "   Extraction with wrong type --> OK" << endl;

	flags.set(DeviceData::wrongtype_flag);
	db.exceptions(flags);
	
	try
	{
		db >> fl;
		assert(false);
	}
	catch (Tango::DevFailed &)
	{
	}
	
	cout << "   Extraction with wrong type (exception) --> OK" << endl;

// Test assignement operator

	DeviceData dd,dd_c;
	vector<string> v_str;
	v_str.push_back("abc");
	v_str.push_back("def");
	
	dd << v_str;
	dd_c = dd;
	
	vector<string> out;
	dd_c >> out;
	
	assert( out[0] == "abc");
	assert( out[1] == "def");
	
	cout << "   assignement operator --> OK" << endl;
	
// Test copy constructor

	DeviceData d;
	double db2 = 3.45;
	d << db2;
	
	DeviceData dc(d);
	
	double db_out;
	dc >> db_out;
	
	assert( db_out == db2 );
	
	cout << "   Copy constructor --> OK" << endl;	
	return 0;		
}

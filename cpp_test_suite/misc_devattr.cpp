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

	DeviceAttribute da;
	long lo;
	bitset<DeviceAttribute::numFlags> flags;
	flags.reset(DeviceAttribute::isempty_flag);
	da.exceptions(flags);
		
	bool ret = da >> lo;
	assert ( ret == false );
	
	cout << "   Extraction from empty object --> OK" << endl;

	flags.set(DeviceAttribute::isempty_flag);	
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

	DeviceAttribute db;
	long l = 2;
	db << l;
	
	short fl;
	
	ret = db >> fl;
	assert ( ret == false );
	
	cout << "   Extraction with wrong type --> OK" << endl;

	flags.set(DeviceAttribute::wrongtype_flag);
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

	DeviceAttribute dd,dd_c;
	string v_str("abc");
	
	dd << v_str;
	dd_c = dd;
	
	string out;
	dd_c >> out;
	
	assert( out == "abc");
	
	cout << "   assignement operator --> OK" << endl;
	
// Test copy constructor

	DeviceAttribute d;
	double db2 = 3.45;
	d << db2;
	
	DeviceAttribute dc(d);
	
	double db_out;
	dc >> db_out;
	
	assert( db_out == db2 );
	
	cout << "   Copy constructor --> OK" << endl;

// Test move assignement

#ifdef  HAS_RVALUE
	DeviceAttribute d_ma;
	float fl_move = 2.0;
	d_ma << fl_move;

	DeviceAttribute d_ma_out;
	d_ma_out = move(d_ma);

	float fl_move_out;
	d_ma_out >> fl_move_out;

	assert (fl_move == fl_move_out );

	cout << "   Move assignement --> OK" << endl;
#endif

	return 0;		
}

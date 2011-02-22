/* 
 * Example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>


using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc != 2)
	{
		cout << "usage: %s device" << endl;
		exit(-1);
	}

	string device_name = argv[1];

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

// Get attribute config

	AttributeInfo ai,old_ai;
	string att_name("Float_spec_attr_rw");
	try
	{	
		ai = device->get_attribute_config(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	old_ai = ai;
//	cout << ai << endl;

	assert (ai.min_value == "Not specified");
	assert (ai.max_value == "Not specified");
	assert (ai.min_alarm == "Not specified");
	assert (ai.max_alarm == "Not specified");


// Change min_value, max_value,min_alarm and max_alarm

	ai.min_value = "3.5";
	ai.max_value = "15.5";
	ai.min_alarm = "4.2";
	ai.max_alarm = "13.5";

	AttributeInfoList ai_list;
	ai_list.push_back(ai);	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

// Get config once more

	AttributeInfo new_ai;
	try
	{	
		new_ai = device->get_attribute_config(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << new_ai << endl;
	
	assert (new_ai.name == ai.name );
	assert (new_ai.data_type == ai.data_type );
	assert (new_ai.data_format == ai.data_format );
	assert (new_ai.max_dim_x == ai.max_dim_x );
	assert (new_ai.max_dim_y == ai.max_dim_y );
	assert (new_ai.writable == ai.writable );
	assert (new_ai.label == ai.label );
	assert (new_ai.description == ai.description );
	assert (new_ai.min_alarm == ai.min_alarm );
	assert (new_ai.max_alarm == ai.max_alarm );
	assert (new_ai.min_value == ai.min_value );
	assert (new_ai.max_value == ai.max_value );
	
// Reset old conf

	ai_list.clear();
	ai_list.push_back(old_ai);
	ai_list[0].min_value = "NaN";
	ai_list[0].max_value = "NaN";
	ai_list[0].min_alarm = "NaN";
	ai_list[0].max_alarm = "NaN";
	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

// Get config once again

	try
	{	
		new_ai = device->get_attribute_config(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << new_ai << endl;

	assert (new_ai.name == old_ai.name );
	assert (new_ai.data_type == old_ai.data_type );
	assert (new_ai.data_format == old_ai.data_format );
	assert (new_ai.max_dim_x == old_ai.max_dim_x );
	assert (new_ai.max_dim_y == old_ai.max_dim_y );
	assert (new_ai.writable == old_ai.writable );
	assert (new_ai.label == old_ai.label );
	assert (new_ai.description == old_ai.description );
	assert (new_ai.min_alarm == old_ai.min_alarm );
	assert (new_ai.max_alarm == old_ai.max_alarm );
	assert (new_ai.min_value == old_ai.min_value );
	assert (new_ai.max_value == old_ai.max_value );	
					
	cout << "   Setting/Getting attribute info --> OK" << endl;

// Try to write a value outside authorized limit.
// First, Change min_value and  max_value

	new_ai.min_value = "3.5";
	new_ai.max_value = "15.5";

	ai_list.clear();
	ai_list.push_back(new_ai);	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	vector<float> in;
	in.push_back(3.6);
	in.push_back(3.3);
	in.push_back(3.7);
	DeviceAttribute din(att_name,in);

	bool failed = false;
	string reason;	
	try
	{
		device->write_attribute(din);
	}
	catch (Tango::DevFailed &e)
	{
//		Except::print_exception(e);
		failed = true;
		reason = e.errors[0].reason;
	}

	assert (failed == true);
	assert (reason == "API_WAttrOutsideLimit");
	
	in[2] = 17.6;
	failed = false;
	din << in;
	
	try
	{
		device->write_attribute(din);
	}
	catch (Tango::DevFailed &e)
	{
//		Except::print_exception(e);
		failed = true;
		reason = e.errors[0].reason;
	}

	assert (failed == true);
	assert (reason == "API_WAttrOutsideLimit");

	cout << "   Writing outside attribute limits --> OK" << endl;

//*************************************************************************
//
//	Check min alarm detection
//
//*************************************************************************
	
// Reset min_value and max_value but set min alarm

	ai_list[0].min_value = "NaN";
	ai_list[0].max_value = "NaN";
	ai_list[0].min_alarm = "6.0";
	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

// Read device state

	DevState ds;
	string status;
	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ALARM);
	
	string::size_type idx;
	idx = status.find("ALARM");
	assert (idx != string::npos);
	
	idx = status.find(att_name);
	assert (idx != string::npos);

// Read the attribute to check its quality factor

	DeviceAttribute da;
	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	AttrQuality qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_ALARM);

	cout << "   Min alarm detection (on a float spectrum) --> OK" << endl;
		
// Reset min_alarm 

	ai_list[0].min_alarm = "NaN";	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ON);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_VALID);
	
	cout << "   Reset min alarm detection --> OK" << endl;
	
//*************************************************************************
//
//	Check max alarm detection
//
//*************************************************************************

// Reset min alarm a but set max alarm

	ai_list[0].min_alarm = "NaN";
	ai_list[0].max_alarm = "10.1";
	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ALARM);
	
	idx = status.find("ALARM");
	assert (idx != string::npos);
	
	idx = status.find(att_name);
	assert (idx != string::npos);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_ALARM);

	cout << "   Max alarm detection (on a float spectrum) --> OK" << endl;
		
// Reset min_alarm 

	ai_list[0].max_alarm = "NaN";	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ON);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_VALID);
	
	cout << "   Reset max alarm detection --> OK" << endl;

//*************************************************************************
//
//	Check min alarm detection for unsigned short spectrum
//
//*************************************************************************

	att_name = "UShort_spec_attr";
	try
	{	
		ai = device->get_attribute_config(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
		
// Reset min_value and max_value but set min alarm

	ai_list.clear();
	ai_list.push_back(ai);
	ai_list[0].min_alarm = "350";
	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ALARM);
	
	idx = status.find("ALARM");
	assert (idx != string::npos);
	
	idx = status.find(att_name);
	assert (idx != string::npos);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_ALARM);

	cout << "   Min alarm detection (on a unsigned short spectrum) --> OK" << endl;
		
// Reset min_alarm 

	ai_list[0].min_alarm = "NaN";	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ON);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_VALID);
	
	cout << "   Reset min alarm detection --> OK" << endl;
	
//*************************************************************************
//
//	Check max alarm detection
//
//*************************************************************************

// Reset min alarm a but set max alarm

	ai_list[0].min_alarm = "NaN";
	ai_list[0].max_alarm = "400";
	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ALARM);
	
	idx = status.find("ALARM");
	assert (idx != string::npos);
	
	idx = status.find(att_name);
	assert (idx != string::npos);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_ALARM);

	cout << "   Max alarm detection (on a unsigned short spectrum) --> OK" << endl;
		
// Reset min_alarm 

	ai_list[0].max_alarm = "NaN";	
	try
	{	
		device->set_attribute_config(ai_list);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}
	
// Read device state

	try
	{
		ds = device->state();
		status = device->status();
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

//	cout << "State = " << ds << endl;
//	cout << "Status = " << status << endl;
	
	assert (ds == Tango::ON);

// Read the attribute to check its quality factor

	try
	{
		da = device->read_attribute(att_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}

	qual = da.get_quality();
//	cout << "Quality = " << qual << endl;
	
	assert (qual == Tango::ATTR_VALID);
	
	cout << "   Reset max alarm detection --> OK" << endl;

//*************************************************************************
//
//	Check get/set config for Tango V5
//
//*************************************************************************


	try
	{	
		AttributeInfoEx ai,old_ai;
		string att_name("Float_spec_attr_rw");
		
		ai = device->get_attribute_config(att_name);
		
		assert (ai.min_value == "Not specified");
		assert (ai.max_value == "Not specified");
		assert (ai.min_alarm == "Not specified");
		assert (ai.max_alarm == "Not specified");
		
		assert (ai.alarms.min_alarm == "Not specified");
		assert (ai.alarms.max_alarm == "Not specified");
		assert (ai.alarms.min_warning == "Not specified");
		assert (ai.alarms.delta_t == "Not specified");
		
		assert (ai.events.ch_event.abs_change == "Not specified");
		assert (ai.events.ch_event.rel_change == "Not specified");
		assert (ai.events.per_event.period == "Not specified");
		assert (ai.events.arch_event.archive_period == "Not specified");
		
		old_ai = ai;
		
//		cout << ai << endl;

// Change some config parameters

		ai.min_value = "3.5";
		ai.max_value = "15.5";
		ai.alarms.min_alarm = "4.2";
		ai.min_alarm = "4.2";
		ai.max_alarm = "13.5";
		ai.alarms.max_alarm = "13.5";
		ai.events.per_event.period = "250";
		ai.events.ch_event.rel_change = "11";
		ai.events.arch_event.archive_abs_change = "13,14";

		AttributeInfoListEx ai_list;
		ai_list.push_back(ai);	
		device->set_attribute_config(ai_list);

// Get config once more

		AttributeInfoEx new_ai;

		new_ai = device->get_attribute_config(att_name);
	
		assert (new_ai.name == ai.name );
		assert (new_ai.data_type == ai.data_type );
		assert (new_ai.data_format == ai.data_format );
		assert (new_ai.max_dim_x == ai.max_dim_x );
		assert (new_ai.max_dim_y == ai.max_dim_y );
		assert (new_ai.writable == ai.writable );
		assert (new_ai.label == ai.label );
		assert (new_ai.description == ai.description );
		assert (new_ai.min_alarm == ai.min_alarm );
		
		assert (new_ai.max_alarm == ai.max_alarm );
		assert (new_ai.min_value == ai.min_value );
		assert (new_ai.max_value == ai.max_value );
		assert (new_ai.alarms.min_alarm == ai.alarms.min_alarm);
		assert (new_ai.alarms.max_alarm == ai.alarms.max_alarm);
		assert (new_ai.alarms.min_warning == ai.alarms.min_warning);
		assert (new_ai.alarms.max_warning == ai.alarms.max_warning);
		assert (new_ai.alarms.delta_t == ai.alarms.delta_t);
		assert (new_ai.alarms.delta_val == ai.alarms.delta_val);
		assert (new_ai.events.ch_event.abs_change == ai.events.ch_event.abs_change);
		assert (new_ai.events.ch_event.rel_change == ai.events.ch_event.rel_change);
		assert (new_ai.events.per_event.period == ai.events.per_event.period);
		assert (new_ai.events.arch_event.archive_abs_change == ai.events.arch_event.archive_abs_change);
		assert (new_ai.events.arch_event.archive_rel_change == ai.events.arch_event.archive_rel_change);
		assert (new_ai.events.arch_event.archive_period == ai.events.arch_event.archive_period);

// Reset old conf

		ai_list.clear();
		ai_list.push_back(old_ai);
		ai_list[0].min_value = "NaN";
		ai_list[0].max_value = "NaN";
		ai_list[0].alarms.min_alarm = "NaN";
		ai_list[0].alarms.max_alarm = "NaN";
		ai_list[0].events.per_event.period = "NaN";
		ai_list[0].events.ch_event.rel_change = "NaN";
		ai_list[0].events.arch_event.archive_abs_change = "NaN";
				
		device->set_attribute_config(ai_list);

// Get config once again
	
		new_ai = device->get_attribute_config(att_name);

//		cout << new_ai << endl;
//		cout << old_ai << endl;
			
		assert (new_ai.name == old_ai.name );
		assert (new_ai.data_type == old_ai.data_type );
		assert (new_ai.data_format == old_ai.data_format );
		assert (new_ai.max_dim_x == old_ai.max_dim_x );
		assert (new_ai.max_dim_y == old_ai.max_dim_y );
		assert (new_ai.writable == old_ai.writable );
		assert (new_ai.label == old_ai.label );
		assert (new_ai.description == old_ai.description );
		assert (new_ai.min_alarm == old_ai.min_alarm );
		assert (new_ai.max_alarm == old_ai.max_alarm );
		assert (new_ai.min_value == old_ai.min_value );
		assert (new_ai.max_value == old_ai.max_value );
		assert (new_ai.alarms.min_alarm == old_ai.alarms.min_alarm);
		assert (new_ai.alarms.max_alarm == old_ai.alarms.min_alarm);
		assert (new_ai.alarms.min_warning == old_ai.alarms.min_warning);
		assert (new_ai.alarms.max_warning == old_ai.alarms.max_warning);
		assert (new_ai.alarms.delta_t == old_ai.alarms.delta_t);
		assert (new_ai.alarms.delta_val == old_ai.alarms.delta_val);
		assert (new_ai.events.ch_event.abs_change == old_ai.events.ch_event.abs_change);
		assert (new_ai.events.ch_event.rel_change == old_ai.events.ch_event.rel_change);
		assert (new_ai.events.per_event.period == old_ai.events.per_event.period);
		assert (new_ai.events.arch_event.archive_abs_change == old_ai.events.arch_event.archive_abs_change);
		assert (new_ai.events.arch_event.archive_rel_change == old_ai.events.arch_event.archive_abs_change);
		assert (new_ai.events.arch_event.archive_period == old_ai.events.arch_event.archive_period);

		cout << "   Setting/Getting V5 attribute info --> OK" << endl;

//*************************************************************************
//
//	Check alarm and warning alarms
//
//*************************************************************************

// Change min alarm, min_warning, max_warning and max_alarm
// to have attribute in ALARM

		ai_list[0].alarms.min_alarm = "6";
		ai_list[0].alarms.max_alarm = "13";
		ai_list[0].alarms.min_warning = "7";
		ai_list[0].alarms.max_warning = "12";
		
		device->set_attribute_config(ai_list);

// Read device state and status

		Tango::DevState ds = device->state();
		string status = device->status();

//		cout << "State = " << DevStateName[ds] << endl;
//		cout << "Status = " << status << endl;
	
		assert (ds == Tango::ALARM);
	
		idx = status.find("ALARM");
		assert (idx != string::npos);
	
		idx = status.find(att_name);
		assert (idx != string::npos);

		idx = status.find("low");
		assert (idx != string::npos);
				
// Read the attribute to check its quality factor

		Tango::DeviceAttribute da = device->read_attribute(att_name);
		Tango::AttrQuality qual = da.get_quality();
		
//		cout << "Quality = " << qual << endl;
	
		assert (qual == Tango::ATTR_ALARM);
		
// Change min alarm, min_warning, max_warning and max_alarm
// to have attribute in WARNING

		ai_list[0].alarms.min_alarm = "4";
		ai_list[0].alarms.max_alarm = "13";
		ai_list[0].alarms.min_warning = "6";
		ai_list[0].alarms.max_warning = "12";
		
		device->set_attribute_config(ai_list);

// Read device state and status

		ds = device->state();
		status = device->status();

//		cout << "State = " << DevStateName[ds] << endl;
//		cout << "Status = " << status << endl;
	
		assert (ds == Tango::ALARM);
	
		idx = status.find("ALARM");
		assert (idx != string::npos);
	
		idx = status.find(att_name);
		assert (idx != string::npos);

		idx = status.find("low");
		assert (idx != string::npos);
				
// Read the attribute to check its quality factor

		da = device->read_attribute(att_name);
		qual = da.get_quality();
		
//		cout << "Quality = " << qual << endl;
	
		assert (qual == Tango::ATTR_WARNING);
		
// Change min alarm, min_warning, max_warning and max_alarm
// to have attribute VALID

		ai_list[0].alarms.min_alarm = "4";
		ai_list[0].alarms.max_alarm = "13";
		ai_list[0].alarms.min_warning = "5";
		ai_list[0].alarms.max_warning = "12";
		
		device->set_attribute_config(ai_list);

// Read device state and status

		ds = device->state();
		status = device->status();

//		cout << "State = " << DevStateName[ds] << endl;
//		cout << "Status = " << status << endl;
	
		assert (ds == Tango::ON);
		
// Read the attribute to check its quality factor

		da = device->read_attribute(att_name);
		qual = da.get_quality();
		
//		cout << "Quality = " << qual << endl;
	
		assert (qual == Tango::ATTR_VALID);
		
// Change min alarm, min_warning, max_warning and max_alarm
// to have attribute in high WARNING

		ai_list[0].alarms.min_alarm = "4";
		ai_list[0].alarms.max_alarm = "13";
		ai_list[0].alarms.min_warning = "5";
		ai_list[0].alarms.max_warning = "10";
		
		device->set_attribute_config(ai_list);

// Read device state and status

		ds = device->state();
		status = device->status();

//		cout << "State = " << DevStateName[ds] << endl;
//		cout << "Status = " << status << endl;
	
		assert (ds == Tango::ALARM);
	
		idx = status.find("ALARM");
		assert (idx != string::npos);
	
		idx = status.find(att_name);
		assert (idx != string::npos);

		idx = status.find("high");
		assert (idx != string::npos);
				
// Read the attribute to check its quality factor

		da = device->read_attribute(att_name);
		qual = da.get_quality();
		
//		cout << "Quality = " << qual << endl;
	
		assert (qual == Tango::ATTR_WARNING);
		
// Change min alarm, min_warning, max_warning and max_alarm
// to have attribute in high ALARM

		ai_list[0].alarms.min_alarm = "4";
		ai_list[0].alarms.max_alarm = "11";
		ai_list[0].alarms.min_warning = "5";
		ai_list[0].alarms.max_warning = "10";
		
		device->set_attribute_config(ai_list);

// Read device state and status

		ds = device->state();
		status = device->status();

//		cout << "State = " << DevStateName[ds] << endl;
//		cout << "Status = " << status << endl;
	
		assert (ds == Tango::ALARM);
	
		idx = status.find("ALARM");
		assert (idx != string::npos);
	
		idx = status.find(att_name);
		assert (idx != string::npos);

		idx = status.find("high");
		assert (idx != string::npos);
				
// Read the attribute to check its quality factor

		da = device->read_attribute(att_name);
		qual = da.get_quality();
		
//		cout << "Quality = " << qual << endl;
	
		assert (qual == Tango::ATTR_ALARM);
		
// Reset all alarms

		ai_list[0].alarms.min_alarm = "NaN";
		ai_list[0].alarms.max_alarm = "NaN";
		ai_list[0].alarms.min_warning = "NaN";
		ai_list[0].alarms.max_warning = "NaN";
		
		device->set_attribute_config(ai_list);
		
// Read device state and status

		ds = device->state();
		status = device->status();

//		cout << "State = " << DevStateName[ds] << endl;
//		cout << "Status = " << status << endl;
	
		assert (ds == Tango::ON);
		
// Read the attribute to check its quality factor

		da = device->read_attribute(att_name);
		qual = da.get_quality();
		
//		cout << "Quality = " << qual << endl;
	
		assert (qual == Tango::ATTR_VALID);
		
		cout << "   Alarm, Warning level detection --> OK" << endl;
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(-1);
	}


	
											
	delete device;
	return 0;	
}

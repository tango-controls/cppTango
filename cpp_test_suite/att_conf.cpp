// BUG Report:
// a change of the attribute configuration setting to "Not specified"
// is lost if a DevRestart or ServerRestart is done.
// This test checks this feature for: sys/tg_test/1/double_scalar
// The setting is also lost for events.per_event.period even if a DevRestart is not done.
//
// All data are inquired, so this tests works without using the event mechanism.
//
// This program has a exit code of:
//   0   if test is passed
//   >0  if test is not passed

// tango version: 7.1 cvs head (15.10.2009)
//
// contact: georg.kasper@frm2.tum.de

#include "tango.h"

const char * const notset = "Not specified";

// redundancy problem with:
// min_alarm, alarms.min_alarm, max_alarm and alarms.max_alarm in Tango::AttributeInfoEx
// if pedantic_test is false: Tango::AttributeInfoEx::mXX_alarm is not checked
// but Tango::AttributeInfoEx::alarms.mXX_alarm
const bool pedantic_test = false;


void setValues( Tango::AttributeInfoEx & ai, const bool doReset );
int	compare( Tango::AttributeInfoEx & ai_act, Tango::AttributeInfoEx & ai_expected );
int reset_to_default(Tango::AttributeInfoEx &,string &);

int main()
{
	int				res		= 0;
	int 				rc 		= 0;
	//string 			dnUrl		= "tango://taco24:10000/sys/tg_test/1";
	string 				dnUrl		= "dev/test/10";
	string				devnm		= "dev/test/10";
	string 				an		= "Double_attr";
	Tango::DeviceProxy  		*dev		= NULL;
	Tango::DeviceProxy  		*adm_dev	= NULL;
	Tango::AttributeInfoListEx 	ail;
	Tango::AttributeInfoEx 		ai_orig;
	Tango::AttributeInfoEx 		ai_act;
	Tango::AttributeInfoEx 		ai_expected;
	Tango::DeviceData		ddOut;
	Tango::DeviceData		ddIn;

	try
	{
		dev = new Tango::DeviceProxy( dnUrl );
		adm_dev = new Tango::DeviceProxy( dev->adm_name().c_str() );
//		cout << "connected to: " <<  dev->dev_name() << "\t";
//		cout << "with adm_device: " <<  dev->adm_name() << endl;
//		cout << "connected to: " <<  adm_dev->dev_name() << endl;

		ai_orig = dev->attribute_query( an );

		// test 1: change to numerical/string values:
//		cout << endl << "test 1:  change to numerical/string values" << endl;
		ai_expected = ai_orig;
		setValues( ai_expected, false );
		ail.clear();
        ail.push_back( ai_expected );
        dev->set_attribute_config( ail );
		sleep(1);
		ai_act = dev->attribute_query( an );
		res = compare( ai_act, ai_expected );
		if( res != 0 )
		{
			rc = 1;
//			cout << "test 1: FAILED, set and control numerical changes." << endl;
			
			ail.clear();
        	ail.push_back( ai_orig );
        	dev->set_attribute_config( ail );
			
			exit(-1);
		}
		else
			cout << "   Set and control numerical changes --> OK" << endl;

		// test 2: change to: Not specified
//		cout << endl << "test 2:  change to Not specified" << endl;
		ai_expected = ai_orig;
		setValues( ai_expected, true );
		ail.clear();
        ail.push_back( ai_expected );
        dev->set_attribute_config( ail );
		sleep(1);
		ai_act = dev->attribute_query( an );
		reset_to_default(ai_expected,an);
		res = compare( ai_act, ai_expected );
		if( res != 0 )
		{
			rc = rc*10 + 1;
//			cout << "test 2: FAILED, set all to '" << notset << "' control changes"  << endl;
			
			ail.clear();
        	ail.push_back( ai_orig );
        	dev->set_attribute_config( ail );
			
			exit(-1);
		}
		else
			cout << "   Set all to 'not specified' --> OK" << endl;

		// test 3: device restart, change to numerical/string values
//		cout << endl << "test 3:  change to numerical/string values with device restart" << endl;
		ai_expected = ai_orig;
		setValues( ai_expected, false );
		ail.clear();
        ail.push_back( ai_expected );
        dev->set_attribute_config( ail );

//		cout << "         make a DevRestart" << endl;
		ddIn << devnm;
		ddOut = adm_dev->command_inout( "DevRestart", ddIn );
		sleep( 5 );

		ai_act = dev->attribute_query( an );
		res = compare( ai_act, ai_expected );
		if( res != 0 )
		{
			rc = rc*10 + 1;
//			cout << "test 3: FAILED, set all to numerical/string values and control changes after a DevRestart"  << endl;

			ail.clear();
        	ail.push_back( ai_orig );
        	dev->set_attribute_config( ail );
			
			exit(-1);
		}
		else
			cout << "   Set all to numerical/string values and control changes after a DevRestart --> OK" << endl;


		// test 4: device restart, change to 'Not specified'
//		cout << endl << "test 4:  change to 'Not specified' with device restart" << endl;
		ai_expected = ai_orig;
		setValues( ai_expected, true );
		ail.clear();
        ail.push_back( ai_expected );
        dev->set_attribute_config( ail );
//		cout << "         make a DevRestart" << endl;
		ddIn << devnm;
		ddOut = adm_dev->command_inout( "DevRestart", ddIn );
		sleep( 5 );

		ai_act = dev->attribute_query( an );
		reset_to_default(ai_expected,an);
		res = compare( ai_act, ai_expected );
		if( res != 0 )
		{
			rc = rc*10 + 1;
//			cout << "test 4: FAILED, set all to '" << notset << "' control changes after a DevRestart"  << endl;
		}
		else
			cout << "   Set all to 'Not specified' + control changes after a DevRestart --> OK" << endl;


		// restore original setting:
		ail.clear();
        ail.push_back( ai_orig );
        dev->set_attribute_config( ail );
	}
	catch( Tango::DevFailed &ex )
	{
		rc = 9999;
		Tango::Except::print_exception( ex );
		exit (-1);
	}


	delete adm_dev;
	delete dev;

	return rc;
}


void setValues( Tango::AttributeInfoEx & ai, const bool doReset )
{
	ai.description		= doReset ? notset : "description";
	ai.label		= doReset ? notset : "label";
    ai.unit			= doReset ? notset : "unit";
    ai.standard_unit	= doReset ? notset : "11";
    ai.display_unit		= doReset ? notset : "22";
    ai.format		= doReset ? notset : "myformat";
    ai.min_value		= doReset ? notset : "33";
    ai.max_value		= doReset ? notset : "44";
    ai.min_alarm		= doReset ? notset : "55";		// i have a problem with this: redundancy alarms.min_alarm
    ai.max_alarm		= doReset ? notset : "66";		// i have a problem with this: redundancy alarms.max_alarm

	ai.alarms.min_alarm	= doReset ? notset : "77";
    ai.alarms.max_alarm	= doReset ? notset : "88";
    ai.alarms.min_warning	= doReset ? notset : "99";
    ai.alarms.max_warning	= doReset ? notset : "111";
    ai.alarms.delta_t	= doReset ? notset : "222";
    ai.alarms.delta_val	= doReset ? notset : "333";

	ai.events.ch_event.abs_change		 = doReset ? notset : "444";
	ai.events.ch_event.rel_change		 = doReset ? notset : "555";
	ai.events.per_event.period		 = doReset ? notset : "6666";
	ai.events.arch_event.archive_rel_change  = doReset ? notset : "777";
	ai.events.arch_event.archive_abs_change  = doReset ? notset : "888";
	ai.events.arch_event.archive_period	 = doReset ? notset : "9999";
}


int	compare( Tango::AttributeInfoEx & ai_act, Tango::AttributeInfoEx & ai_expected )
{
	int	rc = 0;

#	define CMP(x)   if( (ai_act.x != ai_expected.x) ) \
			{ 	rc = 1; \
				cout << #x \
				<< ":\tBUG got: " \
				<< ai_act.x \
				<< "\texpected: " \
				<< ai_expected.x \
				<< endl; \
			}

	CMP( description	);
	CMP( label		);
    CMP( unit		);
    CMP( standard_unit	);
    CMP( display_unit	);
    CMP( format		);
    CMP( min_value		);
    CMP( max_value		);

    if( ::pedantic_test ) CMP( min_alarm );
    if( ::pedantic_test ) CMP( max_alarm );

	CMP( alarms.min_alarm	);
    CMP( alarms.max_alarm	);
    CMP( alarms.min_warning	);
    CMP( alarms.max_warning	);
    CMP( alarms.delta_t	);
    CMP( alarms.delta_val	);

	CMP( events.ch_event.abs_change	);
	CMP( events.ch_event.rel_change	);
	CMP( events.per_event.period	);
	CMP( events.arch_event.archive_rel_change );
	CMP( events.arch_event.archive_abs_change );
	CMP( events.arch_event.archive_period	  );

	return rc;
}

int reset_to_default(Tango::AttributeInfoEx &ai,string &att_name)
{
	ai.description = "No description";
	ai.label = att_name.c_str();
	ai.unit = "No unit";
	ai.standard_unit = "No standard unit";
	ai.display_unit = "No display unit";
	ai.format = "%6.2f";
	ai.events.per_event.period = "1000";
}

// eof


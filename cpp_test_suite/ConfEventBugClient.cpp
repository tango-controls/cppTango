// Testprogram to demonstrate a server internal data update problem in 7.0.2-bugfix (snapshot: 24.7.2009)
//
// This client uses the server "TangoTest test"
// and attribute: tango://taco10:10000/sys/tg_test/1/double_scalar
// as provided by the distributation 7.0.2
//
// PROBLEM:
// server sent a correct configuration changed event with correct data, 
// but did not update the data internal or in the database.
//
// This problem occures for setting: abs_change and rel_change to control event generation.
//
// RUNNING:
// just call the program, no options.

// Contact me at:
// Georg.Kasper@frm2.tum.de 
// 7/2009

/// NO PRODUCTION CODE
/// NO ERROR CHECKS

#include "tango.h"

using namespace std;

std::string  abs_change_according_last_event = "-not yet set-";
std::string  rel_change_according_last_event = "-not yet set-";

class ConfigChangedEventCallback : public Tango::CallBack
{
public:
        ConfigChangedEventCallback() { };
        ~ConfigChangedEventCallback() { }
        void push_event( Tango::AttrConfEventData* ed );
};


void ConfigChangedEventCallback::push_event( Tango::AttrConfEventData* ed )
{
        abs_change_according_last_event = ed->attr_conf->events.ch_event.abs_change;
        rel_change_according_last_event = ed->attr_conf->events.ch_event.rel_change;

//	cout << "[ConfigChangedEventCallback]: push_event( AttrConfEventData ) entering for:" << endl
//             << "[ConfigChangedEventCallback]: " << ed->attr_name << endl
//             << "[ConfigChangedEventCallback]: event type: " << ed->event << endl
//             << "[ConfigChangedEventCallback]: err: " << ed->err << endl
//            << "[ConfigChangedEventCallback]: abs_change: " << abs_change_according_last_event << endl
//             << "[ConfigChangedEventCallback]: rel_change: " << rel_change_according_last_event << endl
//             << "[ConfigChangedEventCallback]: push_event( AttrConfEventData ) leaving." << endl;
}


main()
{
	int rc = 0;
	Tango::AttributeInfoEx ai;
	Tango::AttributeInfoListEx ail;
	Tango::DeviceProxy *dev;
	ConfigChangedEventCallback * configChangedEventCallback = new ConfigChangedEventCallback();

	string dn("dev/test/12");
	string an( "double_attr" );
	string resetValue = "Not specified";
//		string resetValue = "NaN";

//        cout << "[main]: This program assumes that  \"TangoTest test\"  is running" << endl;
//        cout << "[main]: connect to " << dn 
//             << "  TANGO_HOST=" 
//             << (getenv( "TANGO_HOST" ) != NULL ? getenv( "TANGO_HOST" ) : "not set")
//             << endl;

	dev = new Tango::DeviceProxy( dn );
//        cout << "[main]: ok, connected." << endl;

        // put into a defined state:
	ai = dev->attribute_query( an );
	ai.events.ch_event.abs_change = "33333";
	ai.events.ch_event.rel_change = "99.99";
//        cout << "[main]: for " << an << ": set abs_change to: " << ai.events.ch_event.abs_change << endl;
//        cout << "[main]: for " << an << ": set rel_change to: " << ai.events.ch_event.rel_change << endl;
	ail.push_back( ai );
	dev->set_attribute_config( ail );

//        cout << "[main]: make a subscribe_event() for: attribute configuration changed, " << an << endl;
	const vector< string >  filters;
	dev -> subscribe_event( an, Tango::ATTR_CONF_EVENT, configChangedEventCallback, filters );
//        cout << "[main]: subscribe_event() done." << endl << endl;

	sleep( 1 );
//        cout << "[main]: current setting according configuration changed event: abs_chang=" <<  abs_change_according_last_event << endl;
//        cout << "[main]: current setting according configuration changed event: rel_chang=" <<  rel_change_according_last_event << endl;

	// try to reset:
	ai = dev->attribute_query( an );
	ai.events.ch_event.abs_change = resetValue;
	ai.events.ch_event.rel_change = resetValue;
	ail.clear();
	ail.push_back( ai );
	dev->set_attribute_config( ail );

//        cout << "[main]: clearing setting for rel/abs_change.  Wait for a config changed event ..." << endl;
	sleep( 1 );

	// bug demonstration:
//	cout << "[main]: doing a query for configuration from server: dev->attribute_query() ..." << endl;
	ai = dev->attribute_query( an );
//	cout << "[main]: setting as seen by server:                                abs_change=" << ai.events.ch_event.abs_change << endl;
//	cout << "[main]: setting as seen by client due to the config change event: abs_change=" << abs_change_according_last_event << endl;
//	cout << "[main]: setting as seen by server:                                rel_change=" << ai.events.ch_event.rel_change << endl;
//	cout << "[main]: setting as seen by client due to the config change event: rel_change=" << rel_change_according_last_event << endl;

	assert (abs_change_according_last_event == resetValue);
	assert (rel_change_according_last_event == resetValue);

/*	if( abs_change_according_last_event != resetValue
     || rel_change_according_last_event != resetValue )
	{
		rc = -1;
		cout << endl << "[main]: BUG: could not reset setting." << endl;
	}*/

	assert (ai.events.ch_event.abs_change == abs_change_according_last_event);
	assert (ai.events.ch_event.rel_change == rel_change_according_last_event);

/*	if( ai.events.ch_event.abs_change != abs_change_according_last_event 
     || ai.events.ch_event.rel_change != rel_change_according_last_event )
	{
		rc = -1;
		cout << endl << "[main]: BUG: server sent a config changed event with correct data, but did not update the data internal." << endl;
	}*/

//	cout << endl;
	cout << "   Resetting attribute event configuration parameters --> OK" << endl;
	return rc;
}

// eof

/* run protocol:
georg@taco10:/home/projekte/tango/Tests/ConfEventBug> ConfEventBugClient
[main]: This program assumes that  "TangoTest test"  is running
[main]: connect to sys/tg_test/1  TANGO_HOST=taco10.taco.frm2:10000
[main]: ok, connected.
[main]: for double_scalar: set abs_change to: 33333
[main]: for double_scalar: set rel_change to: 99.99
[main]: make a subscribe_event() for: attribute configuration changed, double_scalar
[ConfigChangedEventCallback]: push_event( AttrConfEventData ) entering for:
[ConfigChangedEventCallback]: tango://taco10.taco.frm2:10000/sys/tg_test/1/double_scalar
[ConfigChangedEventCallback]: event type: attr_conf
[ConfigChangedEventCallback]: err: 0
[ConfigChangedEventCallback]: abs_change: 33333
[ConfigChangedEventCallback]: rel_change: 99.99
[ConfigChangedEventCallback]: push_event( AttrConfEventData ) leaving.
[main]: subscribe_event() done.

[main]: current setting according configuration changed event: abs_chang=33333
[main]: current setting according configuration changed event: rel_chang=99.99
[main]: clearing setting for rel/abs_change.  Wait for a config changed event ...
[ConfigChangedEventCallback]: push_event( AttrConfEventData ) entering for:
[ConfigChangedEventCallback]: sys/tg_test/1/double_scalar
[ConfigChangedEventCallback]: event type: attr_conf
[ConfigChangedEventCallback]: err: 0
[ConfigChangedEventCallback]: abs_change: Not specified
[ConfigChangedEventCallback]: rel_change: Not specified
[ConfigChangedEventCallback]: push_event( AttrConfEventData ) leaving.
[main]: doing a query for configuration from server: dev->attribute_query() ...
[main]: setting as seen by server:                                abs_change=33333
[main]: setting as seen by client due to the config change event: abs_change=Not specified
[main]: setting as seen by server:                                rel_change=99.99
[main]: setting as seen by client due to the config change event: rel_change=Not specified

[main]: BUG: server sent a config changed event with correct data, but did not update the data internal.

georg@taco10:/home/projekte/tango/Tests/ConfEventBug>        
*/


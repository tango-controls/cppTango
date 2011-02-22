// Testprogram to demonstrate a tango exit crash problem:
// memory double free or corruption.
//
// If this client locks a device and unlocks it, the client crashes after
// it has terminated main() in tango ApiUtil::clean_locking_threads()
// during the execution of exit() if events are used.
//
// This client uses the tango test server:
// TangoTest test; device: sys/tg_test/1; attribute: long_scalar
// as provided by the distributation 7.0.2-bugfix branch (14.7.2009).
//
// Polling and change event generation by the server must be activated
// for the attribute "long_scalar" (i used absolute change of 1).
//
// GJK, 7/2009,  georg.kasper@frm2.tum.de

#include "tango.h"

using namespace std;

const int NODATA = -9999;

class EventCallback : public Tango::CallBack
{
public:
    EventCallback()  { };
    ~EventCallback() { };
    void push_event( Tango::EventData *ed );

	int cb_executed;
	int cb_err;
};


void EventCallback::push_event( Tango::EventData *ed )
{
    if( 0 == ed->err )
		cb_executed++;
    else
		cb_err++;
}


main()
{
    Tango::DeviceProxy * dev;
    int eventID;
    const vector< string >  filters;
    EventCallback *eventCallback = new EventCallback();
	eventCallback->cb_executed = 0;
	eventCallback->cb_err = 0;

    string devName( "dev/test/10" );
	string att_name ("event_change_tst");

    try
    {
        dev = new Tango::DeviceProxy( devName );
		dev->poll_attribute(att_name,1000);

        eventID = NODATA;
        eventID = dev -> subscribe_event( att_name, Tango::CHANGE_EVENT, eventCallback, filters );

        dev->lock();
    
        int cnt = 0;
        while( cnt < 3 )
        {
            dev->command_inout("IOIncValue");
            sleep( 2 );
            cnt++;
        }

        dev->unlock();
    }
    catch( Tango::DevFailed &ex )
    {
        Tango::Except::print_exception( ex );
    }

    if( eventID != NODATA )
        dev->unsubscribe_event( eventID );

	dev->stop_poll_attribute(att_name);

    delete dev;

	cout << "   Memory corruption at process exit--> ??" << endl;

    return 0;
}

// eof

////////////////////////////////////////////////////////////////////

/*

Protocol of a run and crash of the client showing the problem:


georg@taco10:/home/projekte/tango/Tests/ExitCrashProblem> ExitCrashProblemClient
[main] connect to sys/tg_test/1  TANGO_HOST=taco10.taco.frm2:10000
[main] do subscribe_event() for: long_scalar
[EventCallback::push_event] entering  ...
[EventCallback::push_event] attr-name: tango://taco10.taco.frm2:10000/sys/tg_test/1/long_scalar
[EventCallback::push_event] event type=change
[EventCallback::push_event] err=0
[EventCallback::push_event] got: long_scalar=170
[EventCallback::push_event] leaving.

[main] subscribe_event done.

[main] locking the device ...
[main] new locking status: Device sys/tg_test/1 is locked by CPP or Python client with PID 17831 from host giop:tcp:[::ffff:172.25.2.10]:18156

[main] still alive. Waiting for a long_scalar event  [exiting after 3 trials] ...
[EventCallback::push_event] entering  ...
[EventCallback::push_event] attr-name: sys/tg_test/1/long_scalar
[EventCallback::push_event] event type=change
[EventCallback::push_event] err=0
[EventCallback::push_event] got: long_scalar=100
[EventCallback::push_event] leaving.

[main] still alive. Waiting for a long_scalar event  [exiting after 3 trials] ...
[EventCallback::push_event] entering  ...
[EventCallback::push_event] attr-name: sys/tg_test/1/long_scalar
[EventCallback::push_event] event type=change
[EventCallback::push_event] err=0
[EventCallback::push_event] got: long_scalar=145
[EventCallback::push_event] leaving.

[main] still alive. Waiting for a long_scalar event  [exiting after 3 trials] ...
[EventCallback::push_event] entering  ...
[EventCallback::push_event] attr-name: sys/tg_test/1/long_scalar
[EventCallback::push_event] event type=change
[EventCallback::push_event] err=0
[EventCallback::push_event] got: long_scalar=67
[EventCallback::push_event] leaving.

[EventCallback::push_event] entering  ...
[EventCallback::push_event] attr-name: sys/tg_test/1/long_scalar
[EventCallback::push_event] event type=change
[EventCallback::push_event] err=0
[EventCallback::push_event] got: long_scalar=139
[EventCallback::push_event] leaving.

[main] unlocking the device ...
[main] new locking status: Device sys/tg_test/1 is not locked
[main] unsubscribe_event ...
[main] deleting DeviceProxy ...
[main] leaving main() with return 0;
*** glibc detected *** ExitCrashProblemClient: double free or corruption (fasttop): 0x08074b38 ***
======= Backtrace: =========
/lib/libc.so.6[0xb7375fc4]
/lib/libc.so.6(cfree+0x9c)[0xb737795c]
/usr/lib/libstdc++.so.6(_ZdlPv+0x21)[0xb753bad1]
/home/local/tango/lib/libtango.so.7(_ZN5Tango7ApiUtil21clean_locking_threadsEb+0x72)[0xb7df5b82]
/home/local/tango/lib/libtango.so.7(leavefunc+0x41)[0xb7cb7361]
/lib/libc.so.6(exit+0xe1)[0xb73378f1]
/lib/libc.so.6(__libc_start_main+0xed)[0xb73205fd]
ExitCrashProblemClient(_ZNSt8ios_base4InitD1Ev+0x3d)[0x8048f61]
======= Memory map: ========
08048000-0804b000 r-xp 00000000 08:03 1188175    /home/projekte/tango/Tests/ExitCrashProblem/ExitCrashProblemClient
0804b000-0804c000 r--p 00002000 08:03 1188175    /home/projekte/tango/Tests/ExitCrashProblem/ExitCrashProblemClient
0804c000-0804d000 rw-p 00003000 08:03 1188175    /home/projekte/tango/Tests/ExitCrashProblem/ExitCrashProblemClient
0804d000-0808f000 rw-p 0804d000 00:00 0          [heap]
b42a7000-b42a8000 ---p b42a7000 00:00 0
b42a8000-b4aa8000 rw-p b42a8000 00:00 0
b4aa8000-b4aa9000 ---p b4aa8000 00:00 0
b4aa9000-b52a9000 rw-p b4aa9000 00:00 0
b52a9000-b52aa000 ---p b52a9000 00:00 0
b52aa000-b5aaa000 rw-p b52aa000 00:00 0
b5aaa000-b5aab000 ---p b5aaa000 00:00 0
b5aab000-b62ab000 rw-p b5aab000 00:00 0
b62ab000-b62ac000 ---p b62ab000 00:00 0
b62ac000-b6aac000 rw-p b62ac000 00:00 0
b6aac000-b6ab4000 r-xp 00000000 08:02 311938     /lib/libnss_nis-2.8.so
b6ab4000-b6ab5000 r--p 00007000 08:02 311938     /lib/libnss_nis-2.8.so
b6ab5000-b6ab6000 rw-p 00008000 08:02 311938     /lib/libnss_nis-2.8.so
b6ab6000-b6abd000 r-xp 00000000 08:02 311930     /lib/libnss_compat-2.8.so
b6abd000-b6abe000 r--p 00006000 08:02 311930     /lib/libnss_compat-2.8.so
b6abe000-b6abf000 rw-p 00007000 08:02 311930     /lib/libnss_compat-2.8.so
b6abf000-b6ace000 r-xp 00000000 08:02 311945     /lib/libresolv-2.8.so
b6ace000-b6acf000 r--p 0000f000 08:02 311945     /lib/libresolv-2.8.so
b6acf000-b6ad0000 rw-p 00010000 08:02 311945     /lib/libresolv-2.8.so
b6ad0000-b6ad2000 rw-p b6ad0000 00:00 0
b6af0000-b6af1000 ---p b6af0000 00:00 0
b6af1000-b72f3000 rw-p b6af1000 00:00 0
b72f3000-b7306000 r-xp 00000000 08:02 311928     /lib/libnsl-2.8.so
b7306000-b7307000 r--p 00012000 08:02 311928     /lib/libnsl-2.8.so
b7307000-b7308000 rw-p 00013000 08:02 311928     /lib/libnsl-2.8.so
b7308000-b730a000 rw-p b7308000 00:00 0
b730a000-b7447000 r-xp 00000000 08:02 311917     /lib/libc-2.8.so
b7447000-b7449000 r--p 0013d000 08:02 311917     /lib/libc-2.8.so
b7449000-b744a000 rw-p 0013f000 08:02 311917     /lib/libc-2.8.so
b744a000-b744e000 rw-p b744a000 00:00 0
b744e000-b745a000 r-xp 00000000 08:02 312010     /lib/libgcc_s.so.1
b745a000-b745b000 r--p 0000b000 08:02 312010     /lib/libgcc_s.so.1
b745b000-b745c000 rw-p 0000c000 08:02 312010     /lib/libgcc_s.so.1
b745c000-b7480000 r-xp 00000000 08:02 311925     /lib/libm-2.8.so
b7480000-b7481000 r--p 00023000 08:02 311925     /lib/libm-2.8.so
b7481000-b7482000 rw-p 00024000 08:02 311925     /lib/libm-2.8.so
b7482000-b7567000 r-xp 00000000 08:02 388093     /usr/lib/libstdc++.so.6.0.10
b7567000-b756b000 r--p 000e5000 08:02 388093     /usr/lib/libstdc++.so.6.0.10
b756b000-b756c000 rw-p 000e9000 08:02 388093     /usr/lib/libstdc++.so.6.0.10
b756c000-b7572000 rw-p b756c000 00:00 0
b7572000-b7586000 r-xp 00000000 08:02 311943     /lib/libpthread-2.8.so
b7586000-b7587000 r--p 00013000 08:02 311943     /lib/libpthread-2.8.so
b7587000-b7588000 rw-p 00014000 08:02 311943     /lib/libpthread-2.8.so
b7588000-b758a000 rw-p b7588000 00:00 0
b758a000-b758c000 r-xp 00000000 08:02 311923     /lib/libdl-2.8.so
b758c000-b758d000 r--p 00001000 08:02 311923     /lib/libdl-2.8.so
b758d000-b758e000 rw-p 00002000 08:02 311923     /lib/libdl-2.8.so
b758e000-b7706000 r-xp 00000000 08:03 1638830    /home/local/omniOrb/lib/libCOS4.so.1.3
b7706000-b7734000 r--p 00178000 08:03 1638830    /home/local/omniOrb/lib/libCAbgebrochen
georg@taco10:/home/projekte/tango/Tests/ExitCrashProblem>
georg@taco10:/home/projekte/tango/Tests/ExitCrashProblem>

*/



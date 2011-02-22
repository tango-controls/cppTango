/* 
 * example of a client using the TANGO device api.
 */

#include <errno.h>
#include <stdlib.h>
#include <tango.h>
#include <devapi.h>
#ifndef _HPUX_SOURCE
#include <iomanip>
#else
#include <iomanip.h>
#endif /* !_HPUX_SOURCE */
#include <time.h>
#include <signal.h>

using namespace Tango;
using namespace CORBA;

static long n_total, n_ok, n_notok;
static long t_min, t_max, t_total;
char *device_name;

void signal_handler(int signal)
{
        long t_avg, error;
/*
 *
 * ----orion.ctrm.esrf.fr PING Statistics----
 * 4 packets transmitted, 4 packets received, 0% packet loss
 * round-trip (ms)  min/avg/max = 0/0/1
 *
 */

        t_avg = t_total / n_total;

        cout << endl << "----" << device_name << " PING Statistics----" << endl;
        cout << n_total << " packets transmitted, " << n_ok << " packets successful, " << n_notok << " errors" << endl;
        cout << "round-trip (ms) min/avg/max = " << t_min << "," << t_avg << "," << t_max << endl;

        exit(0);
}

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if (argc < 2)
	{
		cout << "usage: %s device" << endl;
		exit(-1);
	}

	device_name = argv[1];

	device = new DeviceProxy(device_name);

	struct timeval t1, t2;
	int elapsed;

        (void) signal(SIGINT,  signal_handler);
        (void) signal(SIGQUIT, signal_handler);
        (void) signal(SIGTERM, signal_handler);

        n_total = n_ok = n_notok = 0;
        t_max = 0;
        t_min = 0;

        for (int j=0;;j++)
        {
                gettimeofday(&t1, NULL);
                try
                {
                        cout << "device->ping() " << device->ping() << endl;
                }
                catch (CORBA::Exception &e)
                {
//                        Tango::Except::print_exception(e);
                        cout << "ping to " << argv[1] << ": status=NOTOK sequence=" << j << " time=" << elapsed << " ms " << endl;
                        n_notok++;
                }
                gettimeofday(&t2, NULL);
                elapsed = (t2.tv_sec-t1.tv_sec)*1000000;
                elapsed = (t2.tv_usec-t1.tv_usec) + elapsed;

                n_total++;
                if (t_max == 0) t_max = elapsed;
                if (elapsed > t_max) t_max = elapsed;
                if (t_min == 0) t_min = elapsed;
                if (elapsed < t_min) t_min = elapsed;
                t_total = t_total + elapsed;
                cout << "ping to " << argv[1] << ": status=OK sequence=" << j << " time=" << elapsed << " us " << endl;
                n_ok++;
                sleep(1);
        }

}

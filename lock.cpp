/* 
 * example of a client using the TANGO device api.
 */

#include <tango.h>
#include <assert.h>
#include <wait.h>

#define	coutv	if (verbose == true) cout

bool verbose = false;

using namespace Tango;
using namespace std;

int main(int argc, char **argv)
{
	DeviceProxy *device;
	
	if ((argc == 1) || (argc > 3))
	{
		cout << "usage: %s device [-v] " << endl;
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
		device = new DeviceProxy(device_name);
	}
	catch (CORBA::Exception &e)
	{
		Except::print_exception(e);
		exit(1);
	}

	cout << endl << "new DeviceProxy(" << device->name() << ") returned" << endl << endl;


	try
	{

// Impossible to lock admin device

		string adm_name = device->adm_name();
		DeviceProxy *admin = new DeviceProxy(adm_name);
		bool except = false;
		try
		{
			admin->lock();
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"API_DeviceUnlockable") == 0)
				except = true;
		}

		assert ( except == true );
		cout << "  Admin device unlockable --> OK" << endl;

// Unlocking a non locked device does nothing

		device->unlock();
		cout << "  Unlock a non-locked device --> OK" << endl;

// Lock validity checks

		except = false;
		try
		{
			device->lock(-1);
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"API_MethodArgument") == 0)
				except = true;
		}

		assert ( except == true);

		except = false;
		try
		{
			device->lock(1);
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"API_MethodArgument") == 0)
				except = true;
		}

		assert ( except == true );

		cout << "  Basic test on lock validity argument --> OK" << endl;

// Lock the device, and do some basic test

		bool bool_ret;
		LockerInfo the_locker;

		bool_ret = device->is_locked();
		coutv << "Passed 10" << endl;
		assert ( bool_ret == false );

		bool_ret = device->is_locked_by_me();
		coutv << "Passed 11" << endl;
		assert ( bool_ret == false );

		bool_ret = device->get_locker(the_locker);
		coutv << "Passed 12" << endl;
		assert (bool_ret == false );

		device->lock();

		bool_ret = device->is_locked();
		coutv << "Passed 13" << endl;
		assert ( bool_ret == true );

		bool_ret = device->is_locked_by_me();
		coutv << "Passed 14" << endl;
		assert ( bool_ret == true );

		bool_ret = device->get_locker(the_locker);
		coutv << "Passed 15" << endl;
		assert ( bool_ret == true );
		assert ( the_locker.ll == Tango::CPP );
		assert ( the_locker.li.LockerPid == getpid() );
		char h_name[80];
		gethostname(h_name,80);
		string my_host(h_name);

		string::size_type pos;
		string only_host;
		if ((pos = the_locker.locker_host.find('.')) != string::npos)
			only_host = the_locker.locker_host.substr(0,pos);
		else
			only_host = the_locker.locker_host;
		assert ( only_host == my_host);

		except = false;
		try
		{
			device->lock(2);
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"API_MethodArgument") == 0)
				except = true;
		}

		assert ( except == true );

		DeviceData din,dout;
		din << (short)2;
		dout = device->command_inout("IOShort",din);
		short result;
		dout >> result;

		assert ( result == 4 );

		string sub_process_cmd("locked_device ");
		sub_process_cmd = sub_process_cmd + device_name;
		int ret = system(sub_process_cmd.c_str());

		coutv << "Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		device->unlock();
		ret = system(sub_process_cmd.c_str());

		coutv << "Locked_device returned value after unlock = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		bool_ret = device->is_locked();
		assert ( bool_ret == false );

		bool_ret = device->is_locked_by_me();
		assert ( bool_ret == false );

		bool_ret = device->get_locker(the_locker);
		assert (bool_ret == false);

		cout << "  Basic Lock/Unlock --> OK" << endl;

// Check the allowed commands

		device->lock();
		string sub_proc("allowed_cmd ");
		sub_proc = sub_proc + device_name;
		ret = system(sub_proc.c_str());

		coutv << "allowed_cmd returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		cout << "  Allowed command while device is locked --> OK" << endl;
		device->unlock();

// Re-entrant lock

		device->lock();
		device->lock();

		ret = system(sub_process_cmd.c_str());

		coutv << "(Re-entrant lock) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		device->unlock();

		ret = system(sub_process_cmd.c_str());

		coutv << "(Re-entrant lock) Locked_device returned value after first unlock = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		device->unlock();

		ret = system(sub_process_cmd.c_str());

		coutv << "(Re-entrant lock) Locked_device returned value after second unlock = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		cout << "  Re-Entrant Lock/Unlock --> OK" << endl;

// Destroying the device unlock it

		device->lock();
		device->lock();

		ret = system(sub_process_cmd.c_str());

		coutv << "(Destroying device) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		delete device;
		ret = system(sub_process_cmd.c_str());

		coutv << "(Destroying device) Locked_device returned value after the delete = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		cout << "  Destroy the DeviceProxy unlocks the device --> OK" << endl;
		device = new DeviceProxy(device_name);

// Restarting a device keep the lock

		device->lock(2);

		din << device_name;
		admin->command_inout("DevRestart",din);

		device->state();
		ret = system(sub_process_cmd.c_str());

		coutv << "(Restarting device) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		cout << "  Restart a locked device keeps the lock --> OK" << endl;	

// Impossible to restart a locked device by another client

		string restart_cmd("restart_device ");
		restart_cmd = restart_cmd + device_name + " DevRestart";

		ret = system(restart_cmd.c_str());

		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		cout << "  Impossible to restart a device locked by another client --> OK" << endl;

// Impossible to do polling related commands on a locked device

		string base_restart("restart_device ");
		base_restart = base_restart + device_name;

		string res = base_restart + " AddObjPolling";

		ret = system(restart_cmd.c_str());
		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		res = base_restart + " RemObjPolling";

		ret = system(restart_cmd.c_str());
		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		res = base_restart + " UpdObjPollingPeriod";

		ret = system(restart_cmd.c_str());
		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		cout << "  Impossible to change polling on a device locked by another client --> OK" << endl;

// Impossible to do logging related commands on a locked device

		res = base_restart + " AddLoggingTarget";

		ret = system(restart_cmd.c_str());
		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		res = base_restart + " RemoveLoggingTarget";

		ret = system(restart_cmd.c_str());
		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		res = base_restart + " SetLoggingLevel";

		ret = system(restart_cmd.c_str());
		coutv << "restart_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		cout << "  Impossible to change logging on a device locked by another client --> OK" << endl;	

// Check that the locking thread is doing periodic re-lock

		DeviceProxy *device2 = new DeviceProxy("dev/test/11");
		device2->lock(6);

		sleep(7);
		bool_ret = device->is_locked_by_me();
		assert ( bool_ret == true);
		device->unlock();
		delete device2;

		cout << "  Locking thread re-locks the device --> OK" << endl;

// Unlock a device using the back door

		string unlock_cmd("unlock ");
		unlock_cmd = unlock_cmd + device_name;

		device->lock();
		device->lock();
		device->lock();

		bool_ret = device->is_locked_by_me();
		assert (bool_ret == true);

		ret = system(unlock_cmd.c_str());

		bool_ret = device->is_locked_by_me();
		assert (bool_ret == false);

		bool_ret = device->is_locked();
		assert (bool_ret == false);

		din << (short)2;
		except = false;
		try
		{
			dout = device->command_inout("IOShort",din);
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[0].reason.in(),"API_DeviceUnlocked") == 0)
				except = true;
		}

		assert ( except == true );

		ret = system(sub_process_cmd.c_str());

		coutv << "(Back door) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		device->unlock();
		device->unlock();
		device->unlock();

		cout << "  Another client unlocks the device using the back door --> OK" << endl;

// Restarting a server breaks the lock

		device->lock();

		bool_ret = device->is_locked_by_me();
		assert (bool_ret == true);

		admin->command_inout("RestartServer");

		sleep(2);
		bool_ret = device->is_locked_by_me();
		assert (bool_ret == false);

		bool_ret = device->is_locked();
		assert (bool_ret == false);

		except = false;
		try
		{
			dout = device->command_inout("IOShort",din);
		}
		catch (Tango::DevFailed &e)
		{
			if (::strcmp(e.errors[1].reason.in(),"API_DeviceUnlocked") == 0)
				except = true;
		}
		assert ( except == true );

		except = false;
		try
		{
			dout = device->command_inout("IOShort",din);
		}
		catch (...)
		{
			except = true;
		}
		assert ( except == false );

		ret = system(sub_process_cmd.c_str());

		coutv << "(Restart server) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		device->unlock();

		cout << "  Restart the server breaks the lock --> OK" << endl;

// Locking without relock kills the lock

		device->lock(2);

		bool_ret = device->is_locked_by_me();
		assert (bool_ret == true);

		ret = system(sub_process_cmd.c_str());

		coutv << "(Without ReLock) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 1);

		ApiUtil *au = ApiUtil::instance();
		au->clean_locking_threads(false);

		sleep(5);

		bool_ret = device->is_locked_by_me();
		assert (bool_ret == false);

		ret = system(sub_process_cmd.c_str());

		coutv << "(Without ReLock) Locked_device returned value = " << WEXITSTATUS(ret) << endl;
		assert ( WEXITSTATUS(ret) == 0);

		cout << "  Lock validity --> OK" << endl;
	}
	catch (Tango::DevFailed &e)
	{
		Except::print_exception(e);
		exit(1);
	}
	
	delete device;
	return 0;
	
}

//+=============================================================================
//
// file :               utils_shut.cpp
//
// description :        C++ source for some methods of the Util class related
//						to server shutdown
//
// project :            TANGO
//
// author(s) :         	E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <eventconsumer.h>
#include <eventsupplier.h>

extern omni_thread::key_t key_py_data;

namespace Tango
{
//+----------------------------------------------------------------------------
//
// method : 		Util::shutdown_ds()
//
// description : 	This method sends command to the polling thread for
//			all cmd/attr with polling configuration stored in db.
//			This is done in separate thread in order to equally
//			spread all the polled objects polling time on the
//			smallest polling period.
//
//-----------------------------------------------------------------------------

void Util::shutdown_ds()
{

//
// Stopping a device server means :
//		- Mark the server as shutting down
//		- Send kill command to the polling thread
//		- Join with this polling thread
//		- Unregister server in database
//		- Delete devices (except the admin one)
//		- Stop the KeepAliveThread and the EventConsumer Thread when
//		  they have been started to receive events
//		- Force writing file database in case of
//		- Shutdown the ORB
//		- Cleanup Logging
//

	set_svr_shutting_down(true);

//
// send the exit command to all the polling threads in the pool
//

	stop_all_polling_threads();
	stop_heartbeat_thread();
	clr_heartbeat_th_ptr();

//
// Unregister the server in the database
//

	try
	{
		unregister_server();
	}
	catch(...) {}

//
// Delete the devices (except the admin one)
// Protect python data
//

	omni_thread::value_t *tmp_py_data = omni_thread::self()->get_value(key_py_data);
	PyLock *lock_ptr = (static_cast<PyData *>(tmp_py_data))->PerTh_py_lock;
	lock_ptr->Get();

	get_dserver_device()->delete_devices();

	lock_ptr->Release();

//
// 	Stop the KeepAliveThread and the EventConsumer thread when
//  they have been started to receive events.
//

	ApiUtil *au = ApiUtil::instance();
	if (au->is_notifd_event_consumer_created() == true)
	{
		NotifdEventConsumer *ec = ApiUtil::instance()->get_notifd_event_consumer();
		if (ec != NULL)
			ec->shutdown();
	}
	if (au->is_zmq_event_consumer_created() == true)
	{
		ZmqEventConsumer *ec = ApiUtil::instance()->get_zmq_event_consumer();
		if (ec != NULL)
			ec->shutdown();
	}

//
// Disconnect the server from the notifd, when it was connected
//

	NotifdEventSupplier *ev = get_notifd_event_supplier();
	if (ev != NULL)
		ev->disconnect_from_notifd();

//
// Delete ZmqEventSupplier
//

	ZmqEventSupplier *zev = get_zmq_event_supplier();
	delete zev;

//
// Close access to file database when used
//

	if (_FileDb == true)
	{
		Database *db_ptr = get_database();
		db_ptr->write_filedatabase();
		delete db_ptr;
		cout4 << "Database object deleted" << std::endl;
	}

//
// If the server uses its own event loop, do not call it any more
//

	if (is_server_event_loop_set())
		set_shutdown_server(true);

//
// Shutdown the ORB
//

	cout4 << "Going to shutdown ORB" << std::endl;
	CORBA::ORB_ptr loc_orb = get_orb();
	loc_orb->shutdown(true);

	cout4 << "ORB shutdown" << std::endl;

// clean-up the logging system

	Logging::cleanup();
	cout4 << "Logging cleaned-up" << std::endl;
}

} // End of Tango namespace

//=============================================================================
//
// file :               subdev_diag.h
//
// description :        Collect information on all used sub devices 
//                      in a device server. 
//
// project :            TANGO
//
// author(s) :          J.Meyer
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009
//                      European Synchrotron Radiation Facility
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
// $Revision$
//
// $Log$
// Revision 1.1  2009/02/03 15:22:20  jensmeyer
// The new class SubDevDiag for sub device diagnostics.
//
//
//=============================================================================

#ifndef _SUBDEV_DIAG_H
#define _SUBDEV_DIAG_H

#include <tango.h>

namespace Tango
{

class SubDevDiag
{
private:
	// Type definition and map to keep the
	// list of accessed sub devices in a device server.
	
	// List structure to store every device 
	// and its sub devices. 
	typedef struct sub_dev_list
	{
	bool modified;               // was the list modified?
	vector<string> sub_devices; // list of sub devices
	} SubDeviceList;
	
	// map and mutex to keep a list of sub devices per device
	map<std::string,SubDeviceList> sub_device_map;
	omni_mutex                     sub_dev_map_mutex;
	
	// map to keep a list of sub devices per device as read from
	// the database cache
	map<std::string,SubDeviceList> sub_device_startup_map;

public:
	// Constructor
	SubDevDiag() {};
	
	// destructor to free map data
	~SubDevDiag();
	
	// Set the device name that should be asscociated to a thread
	// in the device server
	void set_associated_device(string dev_name);
	
	// Get the device name that is asscociated 
	// with the current thread of the device server
	string get_associated_device();
	
	// Register a sub device for an associated device
	// in the list of sub devices of the device server
	void register_sub_device (string dev_name, string sub_dev_name);
	
	// Remove all sub devices for a device of the server
	void remove_sub_devices (string dev_name);
	
	// Remove all sub devices
	void remove_sub_devices();
	
	// Read the list of registered sub devices 
	Tango::DevVarStringArray *get_sub_devices();
	
	// Store the list of sub devices when modified into the database 
	void store_sub_devices();
	
	// Store the list sub devices at server start-up from the DB cache.
	void get_sub_devices_from_cache();
};

} // End of Tango namespace
		     
#endif /* SUBDEV_DIAG */

//+===================================================================================================================
//
// file :		dev_poll.cpp
//
// description :	C++ source code for part of the DeviceImpl class. This class is the root class for all derived
//				    Device classes. Tis file contains some polling related methods.
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2011,2012,2013,2014,2015
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License along with Tango.
// If not, see <http://www.gnu.org/licenses/>.
//
//
//-===================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

namespace Tango
{

//+-----------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::init_poll_no_db
//
// description :
//		Init polling info for device running without DB. In such a case, polling is available only for
//      object with polling defined in code. Fill in string vectors which are in case of DS using
//      database initialised from the db.
//
//------------------------------------------------------------------------------------------------------------------

void DeviceImpl::init_poll_no_db()
{
    bool old_set = false;

//
// A loop for all device attribute
//

    std::vector<Attribute *> &att_list = dev_attr->get_attribute_list();
    std::vector<Attribute *>::iterator ite;
    for (ite = att_list.begin();ite != att_list.end();++ite)
    {
        long poll_period = (*ite)->get_polling_period();
        if (poll_period != 0)
        {
            std::vector<std::string> &polled_attr_list = get_polled_attr();
            polled_attr_list.push_back((*ite)->get_name());
            std::stringstream ss;
            ss << poll_period;
            polled_attr_list.push_back(ss.str());

            if (old_set == false)
            {
               set_poll_old_factor(DEFAULT_POLL_OLD_FACTOR);
               old_set = true;
            }
        }
    }

//
// A loop for all device commands
//

    std::vector<Command *> &cmd_list = device_class->get_command_list();
    std::vector<Command *>::iterator ite_cmd;
    for (ite_cmd = cmd_list.begin();ite_cmd != cmd_list.end();++ite_cmd)
    {
        long poll_period = (*ite_cmd)->get_polling_period();
        if (poll_period != 0)
        {
            std::vector<std::string> &polled_cmd_list = get_polled_cmd();
            polled_cmd_list.push_back((*ite_cmd)->get_name());
            std::stringstream ss;
            ss << poll_period;
            polled_cmd_list.push_back(ss.str());

            if (old_set == false)
            {
               set_poll_old_factor(DEFAULT_POLL_OLD_FACTOR);
               old_set = true;
            }
        }
    }
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::is_attribute_polled
//
// description :
//		Returns true if the attribute is polled
//
// argument:
//		in :
//			- att_name : The attribute name
//
//-------------------------------------------------------------------------------------------------------------------

bool DeviceImpl::is_attribute_polled(const std::string &att_name)
{
	std::string att = att_name;
	std::transform(att.begin(),att.end(),att.begin(),::tolower);

	std::vector<std::string> &att_list = get_polled_attr();

	for (unsigned int i = 0;i < att_list.size();i = i+2)
	{

//
//	Convert to lower case before comparison
//

		std::string name_lowercase(att_list[i]);
		std::transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
		if ( att == name_lowercase )
		{

//
// when the polling buffer is externally filled (polling period == 0)
// mark the attribute as not polled! No events can be send by the polling thread!
//

			if ( att_list[i+1] == "0" )
			{
				return false;
			}
			else
				return true;
		}
	}

//
// now check wether a polling period is set (for example by pogo)
//

/*    Tango::Attribute &the_att = dev_attr->get_attr_by_name(att_name.c_str());
	if ( the_att.get_polling_period() > 0 )
	{

//
// check the list of non_auto_polled attributes to verify wether the polling was disabled
//

		std::vector<std::string> &napa = get_non_auto_polled_attr();
		for (unsigned int j = 0;j < napa.size();j++)
		{
#ifdef _TG_WINDOWS_
			if (_stricmp(napa[j].c_str(), att_name.c_str()) == 0)
#else
			if (strcasecmp(napa[j].c_str(), att_name.c_str()) == 0)
#endif
			{
				return false;
			}
		}

		return true;
	}*/

	return false;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::is_command_polled
//
// description :
//		Returns true if the command is polled
//
// argument:
//		in :
//			- cmd_name : The command name
//
//-------------------------------------------------------------------------------------------------------------------

bool DeviceImpl::is_command_polled(const std::string &cmd_name)
{
	std::string cmd = cmd_name;
	std::transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);

	std::vector<std::string> &cmd_list = get_polled_cmd();

	for (unsigned int i = 0;i < cmd_list.size();i = i+2)
	{

//
//	Convert to lower case before comparison
//

		std::string name_lowercase(cmd_list[i]);
		std::transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
		if ( cmd == name_lowercase )
		{

//
// when the polling buffer is externally filled (polling period == 0)
// mark the attribute as not polled! No events can be send by the polling thread!
//

			if ( cmd_list[i+1] == "0" )
			{
				return false;
			}
			else
				return true;
		}
	}

//
// now check wether a polling period is set (for example by pogo)
//

/*    Tango::Command &the_cmd = device_class->get_cmd_by_name(cmd_name);
	if ( the_cmd.get_polling_period() > 0 )
	{

//
// check the list of non_auto_polled attributes to verify wether the polling was disabled
//

		std::vector<std::string> &napa = get_non_auto_polled_cmd();
		for (unsigned int j = 0;j < napa.size();j++)
		{
#ifdef _TG_WINDOWS_
			if (_stricmp(napa[j].c_str(), cmd_name.c_str()) == 0)
#else
			if (strcasecmp(napa[j].c_str(), cmd_name.c_str()) == 0)
#endif
			{
				return false;
			}
		}

		return true;
	}*/

	return false;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::get_attribute_poll_period
//
// description :
//		Get the attribute polling period in mS (O if not polled)
//
// argument:
//		in :
//			- att_name : The attribute name
//
//--------------------------------------------------------------------------------------------------------------------

int DeviceImpl::get_attribute_poll_period(const std::string &att_name)
{
    int per = 0;

	std::string att = att_name;
	std::transform(att.begin(),att.end(),att.begin(),::tolower);

    bool found = false;
	std::vector<std::string> &att_list = get_polled_attr();
	for (unsigned int i = 0;i < att_list.size();i = i+2)
	{

//
//	Convert to lower case before comparison
//

		std::string name_lowercase(att_list[i]);
		std::transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
		if ( att == name_lowercase )
		{
            std::stringstream ss;
            ss << att_list[i + 1];
            ss >> per;
            found = true;

            break;
		}
	}

//
// now check wether a polling period is set (for example by pogo)
//

    if (found == false)
    {
    	Tango::Attribute &the_att = dev_attr->get_attr_by_name(att_name.c_str());
    	per = the_att.get_polling_period();
	}

	return per;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::get_command_poll_period
//
// description :
//		Get the command polling period in mS (0 if not polled)
//
// argument:
//		in :
//			- cmd_name : The command name
//
//--------------------------------------------------------------------------------------------------------------------

int DeviceImpl::get_command_poll_period(const std::string &cmd_name)
{
    int per = 0;

	std::string cmd = cmd_name;
	std::transform(cmd.begin(),cmd.end(),cmd.begin(),::tolower);

    bool found = false;
	std::vector<std::string> &cmd_list = get_polled_cmd();
	for (unsigned int i = 0;i < cmd_list.size();i = i+2)
	{

//
//	Convert to lower case before comparison
//

		std::string name_lowercase(cmd_list[i]);
		std::transform(name_lowercase.begin(),name_lowercase.end(),name_lowercase.begin(),::tolower);
		if ( cmd == name_lowercase )
		{
            std::stringstream ss;
            ss << cmd_list[i + 1];
            ss >> per;
            found = true;

            break;
		}
	}

//
// now check wether a polling period is set (for example by pogo)
//

    if (found == false)
    {
        Tango::Command &the_cmd = device_class->get_cmd_by_name(cmd_name);
        per = the_cmd.get_polling_period();
	}

	return per;
}

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::poll_attribute
//
// description :
//		Poll one attribute. If the attribute is already polled, update its polling period to the new value
//
// argument:
//		in 	:
//			- att_name : The attribute name
//			- period : The polling period
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::poll_attribute(const std::string &att_name,int period)
{
    poll_object(att_name,period,POLL_ATTR);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::poll_command
//
// description :
//		Poll one command. If the command is already polled, update its polling period to the new value
//
// argument:
//		in :
//			- cmd_name : The command name
//   		- period : The polling period
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::poll_command(const std::string &cmd_name,int period)
{
    poll_object(cmd_name,period,POLL_CMD);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::poll_object
//
// description :
//		Poll one object. If the object is already polled, update its polling period to the new value
//
// argument:
//		in :
//			- cmd_name : The object name
//          - period : The polling period
//          - type : Command or attribute
//
//-------------------------------------------------------------------------------------------------------------------

void DeviceImpl::poll_object(const std::string &obj_name,int period,PollObjType type)
{
    Tango::Util *tg = Tango::Util::instance();

	if (tg->is_svr_shutting_down() == true)
	{
		TANGO_THROW_EXCEPTION(API_NotSupported, "It's not supported to start polling on any device cmd/attr while the device is shutting down");
	}

    if (tg->is_svr_starting() == true)
    {

//
// If server is starting, we rely on the Util::polling_configure method to effectively start the polling
// Nevertheless, some tests are coded before doing the job
//

        if (period < MIN_POLL_PERIOD)
        {
            TangoSys_OMemStream o;
            o << period << " is below the min authorized period (" << MIN_POLL_PERIOD << " mS)" << std::ends;
            TANGO_THROW_EXCEPTION(API_NotSupported, o.str());
        }

//
// Just to be sure that the command/attribute exist. Also init ptr to the command/attribute polled list
//

        std::vector<std::string> *poll_obj;

        if (type == POLL_CMD)
        {
            device_class->get_cmd_by_name(obj_name);
            std::vector<std::string> &po = get_polled_cmd();
            poll_obj = &po;
        }
        else
        {
            dev_attr->get_attr_by_name(obj_name.c_str());
            std::vector<std::string> &po = get_polled_attr();
            poll_obj = &po;
        }

//
// Check if the command is not already in the polled command
// If yes, only update polling period in vector. Otherwise, add cmd name and polling period in vector
//
// Util::polling_configure will ask dserver polling command to store info in db only if polling period is negative.
//

        bool found = false;
        std::string obj_name_lower(obj_name);
        std::transform(obj_name_lower.begin(),obj_name_lower.end(),obj_name_lower.begin(),::tolower);

        for (unsigned int i = 0;i < poll_obj->size();i = i + 2)
        {
            std::string tmp_name((*poll_obj)[i]);
            std::transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
            if (tmp_name == obj_name_lower)
            {
                found = true;
                std::stringstream ss;
                std::string period_str;
                ss << period;
                ss >> period_str;
                if (ss)
                    (*poll_obj)[i + 1] = period_str;
            }
        }

        if (found == false)
        {
            std::stringstream ss;
            ss << -period;

            if (ss)
            {
                poll_obj->push_back(obj_name);
                poll_obj->push_back(ss.str());
            }
        }
    }
    else
    {

//
// Ask the admin device to do the work (simulating the classical way to tune polling)
// If the attribute is already polled, it's an update polling period. Otherwise, it's a add object polling command
//

        DServer *ds = tg->get_dserver_device();
        CORBA::Any the_any;

        DevVarLongStringArray *send = new DevVarLongStringArray();
        send->lvalue.length(1);
        send->svalue.length(3);

        send->svalue[0] = Tango::string_dup(get_name().c_str());
        std::string obj_type;
        if (type == POLL_ATTR)
            obj_type = "attribute";
        else
            obj_type = "command";
		obj_type = obj_type + LOCAL_POLL_REQUEST;
		send->svalue[1] = Tango::string_dup(obj_type.c_str());
        send->svalue[2] = Tango::string_dup(obj_name.c_str());
        send->lvalue[0] = period;

        the_any <<= send;

        CORBA::Any *received_any = NULL;

        if (type == POLL_CMD)
        {
            if (is_command_polled(obj_name) == true)
            {
                if (get_command_poll_period(obj_name) != period)
                    received_any = ds->command_inout("UpdObjPollingPeriod",the_any);
            }
            else
                received_any = ds->command_inout("AddObjPolling",the_any);
        }
        else
        {
            if (is_attribute_polled(obj_name) == true)
            {
                if (get_attribute_poll_period(obj_name) != period)
                    received_any = ds->command_inout("UpdObjPollingPeriod",the_any);
            }
            else
                received_any = ds->command_inout("AddObjPolling",the_any);
        }

        delete received_any;
    }
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::stop_poll_attribute
//
// description :
//		Stop polling one attribute. Does nothing if the attribute is not polled
//
// argument:
//		in :
//			- att_name : The attribute name
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::stop_poll_attribute(const std::string &att_name)
{
    stop_poll_object(att_name,POLL_ATTR);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::stop_poll_command
//
// description :
//		Stop polling one command. Does nothing if the command is not polled
//
// argument:
//		in :
//			- cmd_name : The command name
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::stop_poll_command(const std::string &cmd_name)
{
    stop_poll_object(cmd_name,POLL_CMD);
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		DeviceImpl::stop_poll_object
//
// description :
//		Stop polling one object. Does nothing if the object is not polled
//
// argument:
//		in :
//			- obj_name : The object name
//
//--------------------------------------------------------------------------------------------------------------------

void DeviceImpl::stop_poll_object(const std::string &obj_name,PollObjType type)
{
    Tango::Util *tg = Tango::Util::instance();

    if (tg->is_svr_starting() == true)
    {

//
// Just to be sure that the attribute/command exist
//

        std::vector<std::string> *poll_obj;

        if (type == POLL_CMD)
        {
            device_class->get_cmd_by_name(obj_name);
            std::vector<std::string> &po = get_polled_cmd();
            poll_obj = &po;
        }
        else
        {
            dev_attr->get_attr_by_name(obj_name.c_str());
            std::vector<std::string> &po = get_polled_attr();
            poll_obj = &po;
        }

//
// Remove object info in vector of polled attributes/commands
//

        std::string obj_name_lower(obj_name);
        std::transform(obj_name_lower.begin(),obj_name_lower.end(),obj_name_lower.begin(),::tolower);

        std::vector<std::string>::iterator ite;
        for (ite = poll_obj->begin();ite != poll_obj->end();ite = ite + 2)
        {
            std::string tmp_name(*ite);
            std::transform(tmp_name.begin(),tmp_name.end(),tmp_name.begin(),::tolower);
            if (tmp_name == obj_name_lower)
            {
                ite = poll_obj->erase(ite,ite+2);
                if (ite == poll_obj->end())
                    break;
                else
                    ite = ite - 2;
            }
        }
    }
    else
    {

		if (tg->is_device_restarting(device_name) == false)
		{

//
// Ask the admin device to do the work (simulating the classical way to tune polling)
//

			DServer *ds = tg->get_dserver_device();
			CORBA::Any the_any;

			DevVarStringArray *send = new DevVarStringArray();
			send->length(3);

			(*send)[0] = Tango::string_dup(get_name().c_str());
			std::string str_type;
			if (type == POLL_CMD)
				str_type = "command";
			else
				str_type = "attribute";
			str_type = str_type + LOCAL_POLL_REQUEST;
			(*send)[1] = Tango::string_dup(str_type.c_str());
			(*send)[2] = Tango::string_dup(obj_name.c_str());

			the_any <<= send;

			CORBA::Any *received_any;
			received_any = ds->command_inout("RemObjPolling",the_any);

			delete received_any;
		}
    }
}


} // End of Tango namespace

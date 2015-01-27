
static const char *RcsId = "$Id$\n$Name$";

//+=================================================================================================================
//
// file :               ClassPipe.cpp
//
// description :        C++ source code for the MultiClassPipe
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2014,2015
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
// $Revision$
//
//-==================================================================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <classpipe.h>

namespace Tango
{

//+------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassPipe::MultiClassPipe
//
// description :
//		destructor for the MultiClassPipe class
//
//-------------------------------------------------------------------------------------------------------------------

MultiClassPipe::~MultiClassPipe()
{
/*	long nb_attr = attr_list.size();
	for (int i = 0;i < nb_attr;i++)
		delete attr_list[i];*/
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassPipe::MultiClassPipe
//
// description :
//		Constructor for the MultiClassPipe class
//
//--------------------------------------------------------------------------------------------------------------------

MultiClassPipe::MultiClassPipe()
{
	cout4 << "Entering MultiClassPipe constructor" << endl;
}

//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassPipe::init_class_pipe
//
// description :
//		Ask the database for properties defined at class level and build the ClassPipe object for each pipe
//		with defined properties
//
// argument :
//		in :
//			- cl_ptr : The device class object pointer
//
//-------------------------------------------------------------------------------------------------------------------

void MultiClassPipe::init_class_pipe(DeviceClass *cl_ptr)
{
	cout4 << "Entering MultiClassPipe::init_class_pipe" << endl;

	Tango::Util *tg = Tango::Util::instance();

	vector<Pipe *> &pi_list = cl_ptr->get_pipe_list();
	size_t nb_pipe = pi_list.size();

//
// Get class attribute(s) properties stored in DB. No need to implement a retry here (in case of db server restart)
// because the db reconnection is forced by the get_property call executed during xxxClass construction
// before we reach this code.
//

	if ((nb_pipe != 0) && (Tango::Util::_UseDb == true))
	{
		size_t i;
		Tango::DbData db_list;
		string &class_name = cl_ptr->get_name();
		size_t nb_db_requested_pipe = 0;

//
// Ask for class default prop in DB only for pipe with label or desc not already set to some user default value
//

		for(i = 0;i < nb_pipe;i++)
		{
			Pipe *pi_ptr = pi_list[i];
			string &pi_name = pi_ptr->get_name();
			if (pi_ptr->get_label() == pi_name || pi_ptr->get_desc() == DescNotSpec)
			{
				db_list.push_back(DbDatum(pi_name));
				nb_db_requested_pipe++;
			}
		}

		try
		{
			tg->get_database()->get_class_pipe_property(class_name,db_list,tg->get_db_cache());
		}
		catch (Tango::DevFailed &e)
		{
			stringstream ss;
			ss << "Can't get class pipe properties for class " << class_name;

			Except::re_throw_exception(e,API_DatabaseAccess,ss.str(),
				                "MultiClassPipe::init_class_pipe");
		}

//
// Sort property for each pipe
//

		long ind = 0;
		for (i = 0;i < nb_db_requested_pipe;i++)
		{
			vector<PipeProperty> prop_list;

			string pipe_name = db_list[ind].name;
			long nb_prop = 0;
			db_list[ind] >> nb_prop;

			ind++;
			for (long j = 0;j < nb_prop;j++)
			{
				if (db_list[ind].size() > 1)
				{
					string tmp(db_list[ind].value_string[0]);
					long nb = db_list[ind].size();
					for (int k = 1;k < nb;k++)
					{
						tmp = tmp + " ";
						tmp = tmp + db_list[ind].value_string[k];
					}
					prop_list.push_back(PipeProperty(db_list[ind].name,tmp));
				}
				else
					prop_list.push_back(PipeProperty(db_list[ind].name,db_list[ind].value_string[0]));
				ind++;
			}

			if (nb_prop != 0)
			{
#ifdef INIT_LIST
				pipe_prop_list.insert({pipe_name,prop_list});
#else
				pipe_prop_list.insert(make_pair(pipe_name,prop_list));
#endif
			}
		}
	}

	cout4 << "Leaving MultiClassPipe::init_class_pipe" << endl;
}


//+-------------------------------------------------------------------------------------------------------------------
//
// method :
//		MultiClassPipe::get_prop_list
//
// description :
//		Get list of pipe property (defined at class level) for the pipe specified as input arg
//
// argument :
// 		in :
//			- pipe_name : The pipe name
//
// return :
//		Reference to the vector of pipe properties or throw an exception if the pipe is not found
//
//--------------------------------------------------------------------------------------------------------------------

vector<Tango::PipeProperty> &MultiClassPipe::get_prop_list(const string &pipe_name)
{
	map<string,vector<PipeProperty> >::iterator ite;
	ite = pipe_prop_list.find(pipe_name);
	if (ite == pipe_prop_list.end())
	{
		stringstream ss;
		ss << "Pipe " << pipe_name << " not found in class pipe(s) properties" << ends;

		Except::throw_exception(API_PipeNotFound,ss.str(),"MultiClassPipe::get_prop_list");
	}

	return ite->second;
}

} // End of Tango namespace

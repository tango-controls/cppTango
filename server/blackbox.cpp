static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               BlackBox.cpp
//
// description :        C++ source code for the BlackBoxElt and BlackBox
//			classes. These classes are used to implement the
//			tango device server black box. There is one
//			black box for each Tango device. This black box
//			keeps info. on all the activities on a device.
//			A client is able to retrieve these data via a Device
//			attribute
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
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
// $Revision$
//
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <blackbox.h>

#include <stdio.h>

#ifdef _TG_WINDOWS_
#include <sys/types.h>
#include <sys/timeb.h>
#include <ws2tcpip.h>
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif /* _TG_WINDOWS_ */

#include <omniORB4/omniInterceptors.h>
#include <omniORB4/internal/giopStrand.h>
#include <omniORB4/internal/giopStream.h>
#include <omniORB4/internal/GIOP_S.h>


namespace Tango
{

//
// The per thread data storage key (The client IP is stored in thread specific storage)
// defined in utils.cpp
//

extern omni_thread::key_t key;

//
// The function called by the interceptor
//

CORBA::Boolean get_client_addr(omni::omniInterceptors::serverReceiveRequest_T::info_T &info)
{
	omni_thread::self()->set_value(key,new client_addr(((omni::giopStrand &)info.giop_s).connection->peeraddress()));
	return true;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBoxElt::BlackBoxElt
//
// description : 	Constructor for the BlackBoxElt class.
//			This constructor simply set the internal value to their
//			default
//
//--------------------------------------------------------------------------

BlackBoxElt::BlackBoxElt()
{
	req_type = Req_Unknown;
	attr_type = Attr_Unknown;
	op_type = Op_Unknown;
	when.tv_sec = when.tv_usec = 0;
	host_ip_str[0] = '\0';
	client_ident = false;
	attr_names.reserve(DEFAULT_ATTR_NB);
}

BlackBoxElt::~BlackBoxElt()
{
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::BlackBox
//
// description : 	Two constructors for the BlackBox class. The first one
//			does not take any argument and construct a black box
//			with the default depth.
//			The second one create a black box with a depth defined
//			by the argument.
//
// argument : in : 	- max_size : The black box depth
//
//--------------------------------------------------------------------------

BlackBox::BlackBox():box(DefaultBlackBoxDepth)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = DefaultBlackBoxDepth;
}

BlackBox::BlackBox(long max_size):box(max_size)
{
	insert_elt = 0;
	nb_elt = 0;
	max_elt = max_size;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_corba_attr
//
// description : 	This method insert a new element in the black box when
//			this element is a attribute
//
// argument : in : 	- attr : The attribute type
//
//--------------------------------------------------------------------------


void BlackBox::insert_corba_attr(BlackBoxElt_AttrType attr)
{

//
// Take mutex
//

	sync.lock();

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Attribute;
	box[insert_elt].attr_type = attr;
	box[insert_elt].op_type = Op_Unknown;
	box[insert_elt].client_ident = false;

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();

//
// Release mutex
//

	sync.unlock();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_cmd
//
// description : 	This method insert a new element in the black box when
//			this element is a call to the operation command_inout
//
// argument : in : 	- cmd : The command name
//					- vers : The IDL device version
//					- sour : The source parameter (DEV, CACHE...)
//
//--------------------------------------------------------------------------


void BlackBox::insert_cmd(const char *cmd,long vers,DevSource sour)
{
	sync.lock();

	insert_cmd_nl(cmd,vers,sour);

	sync.unlock();
}

void BlackBox::insert_cmd_nl(const char *cmd,long vers,DevSource sour)
{

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	if (vers == 1)
		box[insert_elt].op_type = Op_Command_inout;
	else if (vers <= 3)
		box[insert_elt].op_type = Op_Command_inout_2;
	else
		box[insert_elt].op_type = Op_Command_inout_4;
	box[insert_elt].cmd_name = cmd;
	box[insert_elt].source = sour;
	box[insert_elt].client_ident = false;
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_cmd_cl_ident
//
// description : 	This method insert a new element in the black box when
//			this element is a call to the operation command_inout
//
// argument : in : 	- cmd : The command name
//					- cl_id : The client identification data
//					- sour : The source parameter (DEV, CACHE...)
//
//--------------------------------------------------------------------------


void BlackBox::insert_cmd_cl_ident(const char *cmd,const ClntIdent &cl_id,long vers,DevSource sour)
{
	sync.lock();

//
// Add basic info in the box
//

	insert_cmd_nl(cmd,vers,sour);

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}

//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

	sync.unlock();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::add_cl_ident
//
// description : 	Add client identification data to the client address
//					instance
//
// argument : in : 	- cl_ident : The client identificator
//					- cl_addr : The client address instance
//
//--------------------------------------------------------------------------

void BlackBox::add_cl_ident(const ClntIdent &cl_ident,client_addr *cl_addr)
{
	cl_addr->client_ident = true;
	Tango::LockerLanguage cl_lang = cl_ident._d();
	cl_addr->client_lang = cl_lang;
	if (cl_lang == Tango::CPP)
	{
		cl_addr->client_pid = cl_ident.cpp_clnt();
		string str(cl_addr->client_ip);
		if (str.find(":unix:") != string::npos)
		{
		    string::size_type pos = str.find(' ');
		    if (pos != string::npos)
                cl_addr->client_ip[pos] = '\0';
		}
	}
	else
	{
		Tango::JavaClntIdent jci = cl_ident.java_clnt();
		cl_addr->java_main_class = jci.MainClass;
		cl_addr->java_ident[0] = jci.uuid[0];
		cl_addr->java_ident[1] = jci.uuid[1];
	}
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::update_client_host
//
// description : 	Add client identification data to one of the BlackBox
//					element
//
// argument : in : - ip : The client address instance
//
//--------------------------------------------------------------------------

void BlackBox::update_client_host(client_addr *ip)
{
	long local_insert_elt = insert_elt;
	if (local_insert_elt == 0)
		local_insert_elt = max_elt - 1;
	else
		local_insert_elt--;

	box[local_insert_elt].client_ident = true;
	box[local_insert_elt].client_lang = ip->client_lang;
	box[local_insert_elt].client_pid = ip->client_pid;
	box[local_insert_elt].java_main_class = ip->java_main_class;
}


//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_op
//
// description : 	This method insert a new element in the black box when
//			this element is a call to an operation which is not
//			the command_inout operation
//
// argument : in : 	- cmd : The operation type
//
//--------------------------------------------------------------------------

void BlackBox::insert_op(BlackBoxElt_OpType op)
{
	sync.lock();

	insert_op_nl(op);

	sync.unlock();
}

void BlackBox::insert_op(BlackBoxElt_OpType op,const ClntIdent &cl_id)
{
	sync.lock();

	insert_op_nl(op);

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}

//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

	sync.unlock();
}

void BlackBox::insert_op_nl(BlackBoxElt_OpType op)
{
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = op;
	box[insert_elt].client_ident = false;
#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_attr
//
// description : 	This method insert a new element in the black box when
//			this element is a call to the CORBA operation
//			read_attributes
//
// argument : in : 	- names : The attribute(s) name
//					- vers : The device IDl version
//					- sour : The device source parameter (CACHE, DEV,...)
//
//--------------------------------------------------------------------------


void BlackBox::insert_attr(const Tango::DevVarStringArray &names,long vers,DevSource sour)
{

//
// Take mutex
//

	sync.lock();

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	switch (vers)
	{
	case 1 :
		box[insert_elt].op_type = Op_Read_Attr;
		break;

	case 2 :
		box[insert_elt].op_type = Op_Read_Attr_2;
		break;

	case 3 :
		box[insert_elt].op_type = Op_Read_Attr_3;
		break;

	case 4 :
		box[insert_elt].op_type = Op_Read_Attr_4;
		break;
	}
	box[insert_elt].source = sour;
	box[insert_elt].client_ident = false;


	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < names.length();i++)
	{
		string tmp_str(names[i]);
		box[insert_elt].attr_names.push_back(tmp_str);
	}

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();

//
// Release mutex
//

	sync.unlock();
}

void BlackBox::insert_attr(const Tango::DevVarStringArray &names,const ClntIdent &cl_id,TANGO_UNUSED(long vers),DevSource sour)
{

//
// Take mutex
//

	sync.lock();

//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;

	box[insert_elt].op_type = Op_Read_Attr_4;

	box[insert_elt].source = sour;
	box[insert_elt].client_ident = false;


	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < names.length();i++)
	{
		string tmp_str(names[i]);
		box[insert_elt].attr_names.push_back(tmp_str);
	}

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}

//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

//
// Release mutex
//

	sync.unlock();
}

void BlackBox::insert_attr(const Tango::AttributeValueList &att_list, long vers)
{
	sync.lock();

	insert_attr_nl(att_list,vers);

	sync.unlock();
}

void BlackBox::insert_attr(const Tango::AttributeValueList_4 &att_list, const ClntIdent &cl_id,TANGO_UNUSED(long vers))
{
	sync.lock();

	insert_attr_nl_4(att_list);

//
// Check if the command is executed due to polling
// If true, simply return
//

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}

//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

	sync.unlock();
}

void BlackBox::insert_attr_nl(const Tango::AttributeValueList &att_list, long vers)
{
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	if (vers == 1)
		box[insert_elt].op_type = Op_Write_Attr;
	else if (vers < 4)
		box[insert_elt].op_type = Op_Write_Attr_3;
	else
		box[insert_elt].op_type = Op_Write_Attr_4;

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < att_list.length();i++)
	{
		string tmp_str(att_list[i].name);
		box[insert_elt].attr_names.push_back(tmp_str);
	}
	box[insert_elt].client_ident = false;

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();
}

void BlackBox::insert_attr_nl_4(const Tango::AttributeValueList_4 &att_list)
{
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	box[insert_elt].op_type = Op_Write_Attr_4;

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < att_list.length();i++)
	{
		string tmp_str(att_list[i].name);
		box[insert_elt].attr_names.push_back(tmp_str);
	}

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::insert_wr_attr
//
// description : 	This method insert a new element in the black box when
//			this element is a call to the CORBA operation
//			write_read_attributes
//
// argument : in : 	- names : The attribute(s) name
//
//--------------------------------------------------------------------------

void BlackBox::insert_wr_attr(const Tango::AttributeValueList_4 &att_list, const ClntIdent &cl_id,long vers)
{
	sync.lock();

	insert_attr_wr_nl(att_list,vers);

	omni_thread::value_t *ip = omni_thread::self()->get_value(key);
	if (ip == NULL)
	{
		sync.unlock();
		return;
	}

//
// Add client ident info into the client_addr instance
// and into the box
//

	add_cl_ident(cl_id,(client_addr *)ip);
	update_client_host((client_addr *)ip);

	sync.unlock();
}

void BlackBox::insert_attr_wr_nl(const Tango::AttributeValueList_4 &att_list, long vers)
{
//
// Insert elt in the box
//

	box[insert_elt].req_type = Req_Operation;
	box[insert_elt].attr_type = Attr_Unknown;
	if (vers >= 4)
		box[insert_elt].op_type = Op_Write_Read_Attributes_4;

	box[insert_elt].attr_names.clear();
	for (unsigned long i = 0;i < att_list.length();i++)
	{
		string tmp_str(att_list[i].name);
		box[insert_elt].attr_names.push_back(tmp_str);
	}

#ifdef _TG_WINDOWS_
//
// Note that the exact conversion between milli-sec and u-sec will be done
// only when data is send back to user. This save some times in unnecessary
// computation
//
	struct _timeb t;
	_ftime(&t);

	box[insert_elt].when.tv_usec = (long)t.millitm;
	box[insert_elt].when.tv_sec = (unsigned long)t.time;
#else
	struct timezone tz;
	gettimeofday(&box[insert_elt].when,&tz);
#endif

//
// get client address
//

	get_client_host();

//
// manage insert and read indexes
//

	inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::inc_indexes
//
// description : 	This private method increment the indexes used to acces
//			the box itself. This is necessary because the box must
//			be managed as a circular buffer
//
//--------------------------------------------------------------------------


void BlackBox::inc_indexes()
{
	insert_elt++;
	if (insert_elt == max_elt)
		insert_elt = 0;

	if (nb_elt != max_elt)
		nb_elt++;
}

//+-------------------------------------------------------------------------
//
// method : 		get_client_host
//
// description : 	This private method retrieves the client host IP
//			address (the number). IT USES OMNIORB SPECIFIC
//			INTERCEPTOR
//
//--------------------------------------------------------------------------

void BlackBox::get_client_host()
{
	omni_thread *th_id = omni_thread::self();
	if (th_id == NULL)
		th_id = omni_thread::create_dummy();

	omni_thread::value_t *ip = th_id->get_value(key);
	if (ip == NULL)
    {
        Tango::Util *tg = Tango::Util::instance();
        if (tg->is_svr_starting() == true)
            strcpy(box[insert_elt].host_ip_str,"init");
        else
            strcpy(box[insert_elt].host_ip_str,"polling");
    }
	else
		strcpy(box[insert_elt].host_ip_str,((client_addr *)(ip))->client_ip);
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::build_info_as_str
//
// description : 	Translate all the info stored in a black box element
//			into a readable string.
//
// argument : in : 	- index : The black box element index
//
//--------------------------------------------------------------------------

void BlackBox::build_info_as_str(long index)
{
	char date_str[25];
//
// Convert time to a string
//

	date_ux_to_str(box[index].when,date_str);
	elt_str = date_str;

//
// Add request type and command name in case of
//

	elt_str = elt_str + " : ";
	unsigned long nb_in_vect;
	if (box[index].req_type == Req_Operation)
	{
		elt_str = elt_str + "Operation ";
		unsigned long i;
		switch (box[index].op_type)
		{
		case Op_Command_inout :
			elt_str = elt_str + "command_inout (cmd = " + box[index].cmd_name + ") from ";
			add_source(index);
			break;

		case Op_Ping :
			elt_str = elt_str + "ping ";
			break;

		case Op_Info :
			elt_str = elt_str + "info ";
			break;

		case Op_BlackBox :
			elt_str = elt_str + "blackbox ";
			break;

		case Op_Command_list :
			elt_str = elt_str + "command_list_query ";
			break;

		case Op_Command :
			elt_str = elt_str + "command_query ";
			break;

		case Op_Get_Attr_Config :
			elt_str = elt_str + "get_attribute_config ";
			break;

		case Op_Set_Attr_Config :
			elt_str = elt_str + "set_attribute_config ";
			break;

		case Op_Read_Attr :
			elt_str = elt_str + "read_attributes (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;

		case Op_Write_Attr :
			elt_str = elt_str + "write_attributes (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;

		case Op_Write_Attr_3 :
			elt_str = elt_str + "write_attributes_3 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;

		case Op_Command_inout_2 :
			elt_str = elt_str + "command_inout_2 (cmd = " + box[index].cmd_name + ") from ";
			add_source(index);
			break;

		case Op_Command_list_2 :
			elt_str = elt_str + "command_list_query_2 ";
			break;

		case Op_Command_2 :
			elt_str = elt_str + "command_query_2 ";
			break;

		case Op_Get_Attr_Config_2 :
			elt_str = elt_str + "get_attribute_config_2 ";
			break;

		case Op_Read_Attr_2 :
			elt_str = elt_str + "read_attributes_2 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;

		case Op_Read_Attr_3 :
			elt_str = elt_str + "read_attributes_3 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;

		case Op_Command_inout_history_2 :
			elt_str = elt_str + "command_inout_history_2 ";
			break;

		case Op_Read_Attr_history_2 :
			elt_str = elt_str + "read_attribute_history_2 ";
			break;

		case Op_Read_Attr_history_3 :
			elt_str = elt_str + "read_attribute_history_3 ";
			break;

		case Op_Info_3 :
			elt_str = elt_str + "info_3 ";
			break;

		case Op_Get_Attr_Config_3 :
			elt_str = elt_str + "get_attribute_config_3 ";
			break;

		case Op_Set_Attr_Config_3 :
			elt_str = elt_str + "set_attribute_config_3 ";
			break;

		case Op_Read_Attr_history_4 :
			elt_str = elt_str + "read_attribute_history_4 ";
			break;

		case Op_Command_inout_history_4 :
			elt_str = elt_str + "command_inout_history_4 ";
			break;

		case Op_Command_inout_4 :
			elt_str = elt_str + "command_inout_4 (cmd = " + box[index].cmd_name + ") from ";
			add_source(index);
			break;

		case Op_Read_Attr_4 :
			elt_str = elt_str + "read_attributes_4 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") from ";
			add_source(index);
			break;

		case Op_Write_Attr_4 :
			elt_str = elt_str + "write_attributes_4 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;

		case Op_Set_Attr_Config_4 :
			elt_str = elt_str + "set_attribute_config_4 ";
			break;

		case Op_Write_Read_Attributes_4 :
			elt_str = elt_str + "write_read_attributes_4 (";
			nb_in_vect = box[index].attr_names.size();
			for (i = 0;i < nb_in_vect;i++)
			{
				elt_str = elt_str + box[index].attr_names[i];
				if (i != nb_in_vect - 1)
					elt_str = elt_str + ", ";
			}
			elt_str = elt_str + ") ";
			break;

		case Op_Unknown :
			elt_str = elt_str + "unknown operation !!!!!";
			return;
		}
	}
	else if (box[index].req_type == Req_Attribute)
	{
		elt_str = elt_str + "Attribute ";
		switch (box[index].attr_type)
		{
		case Attr_Name :
			elt_str = elt_str + "name ";
			break;

		case Attr_Description :
			elt_str = elt_str + "description ";
			break;

		case Attr_Status :
			elt_str = elt_str + "status ";
			break;

		case Attr_State :
			elt_str = elt_str + "state ";
			break;

		case Attr_AdmName :
			elt_str = elt_str + "adm_name ";
			break;

		case Attr_Unknown :
			elt_str = elt_str + "unknown attribute !!!!!";
			return;
		}
	}
	else
	{
		elt_str = elt_str + "Unknown CORBA request type !!!!!";
		return;
	}

//
// Add client host name.
// Extract only IP numbers from the omni address format (giop:tcp:xx.yy.zz.kk:port)
// Return in case of badly formed address
//

	bool ipv6=false;
	if ((box[index].host_ip_str[0] != '\0') &&
	    (box[index].host_ip_str[0] != 'p') &&
		(box[index].host_ip_str[5] != 'u') &&
        (box[index].host_ip_str[0] != 'i'))
	{
		string omni_addr = box[index].host_ip_str;
		string::size_type pos;
		if ((pos = omni_addr.find(':')) == string::npos)
			return;
		pos++;
		if ((pos = omni_addr.find(':',pos)) == string::npos)
			return;
		pos++;
		string ip_str = omni_addr.substr(pos);
		if (ip_str[0] == '[')
			ipv6 = true;

		string full_ip_str;
		if (ipv6 == false)
		{
			if ((pos = ip_str.find(':')) == string::npos)
				return;
			full_ip_str = ip_str.substr(0,pos);
		}
		else
		{
			if ((pos = ip_str.find(':')) == string::npos)
				return;
			pos++;
			if ((pos = ip_str.find(':',pos)) == string::npos)
				return;
			pos++;
			if ((pos = ip_str.find(':',pos)) == string::npos)
				return;
			pos++;
			string tmp_ip = ip_str.substr(pos);
			if ((pos = tmp_ip.find(']')) == string::npos)
				return;
			full_ip_str = tmp_ip.substr(0,pos);
		}

		if ((pos = full_ip_str.find('.')) == string::npos)
			return;
		string ip1_str = full_ip_str.substr(0,pos);
		string::size_type old_pos;
		pos++;
		old_pos = pos;
		if ((pos = full_ip_str.find('.',pos)) == string::npos)
			return;
		string ip2_str = full_ip_str.substr(old_pos,pos - old_pos);
		pos++;
		old_pos = pos;
		if ((pos = full_ip_str.find('.',pos)) == string::npos)
			return;
		string ip3_str = full_ip_str.substr(old_pos,pos - old_pos);
		pos++;
		string ip4_str = full_ip_str.substr(pos);

//
// Finally, get host name
//

		struct sockaddr_in si;
		si.sin_family = AF_INET;
		si.sin_port = 0;
#ifdef _TG_WINDOWS_
		int slen = sizeof(si);
		WSAStringToAddress((char *)full_ip_str.c_str(),AF_INET,NULL,(SOCKADDR *)&si,&slen);
#else
		inet_pton(AF_INET,full_ip_str.c_str(),&si.sin_addr);
#endif

		char host[512];

		int res = getnameinfo((const sockaddr *)&si,sizeof(si),host,512,0,0,0);
#ifdef _TG_WINDOWS_
		for (int i = 0;i < ::strlen(host);i++)
			host[i] = ::tolower(host[i]);
#endif

		if (res == 0)
		{
			elt_str = elt_str + "requested from ";
			elt_str = elt_str + host;
		}
		else
		{
			elt_str = elt_str + "requested from ";
			elt_str = elt_str + full_ip_str;
		}

//
// Add client identification if available
//

		if (box[index].client_ident == true)
		{
			if (box[index].client_lang == Tango::CPP)
			{
				elt_str = elt_str + " (CPP/Python client with PID ";
				TangoSys_MemStream o;
				o << box[index].client_pid;
				elt_str = elt_str + o.str() + ")";
			}
			else
			{
				elt_str = elt_str + " (Java client with main class ";
				elt_str = elt_str + box[index].java_main_class + ")";
			}
		}
	}
	else if (box[index].host_ip_str[5] == 'u')
	{
		Tango::Util *tg = Tango::Util::instance();
		elt_str = elt_str + "requested from " + tg->get_host_name();

//
// Add client identification if available
//

		if (box[index].client_ident == true)
		{
			if (box[index].client_lang == Tango::CPP)
			{
				elt_str = elt_str + " (CPP/Python client with PID ";
				TangoSys_MemStream o;
				o << box[index].client_pid;
				elt_str = elt_str + o.str() + ")";
			}
			else
			{
				elt_str = elt_str + " (Java client with main class ";
				elt_str = elt_str + box[index].java_main_class + ")";
			}
		}
	}
	else if (box[index].host_ip_str[0] == 'p')
	{
        elt_str = elt_str + "requested from polling";
	}
	else if (box[index].host_ip_str[0] == 'i')
	{
        elt_str = elt_str + "requested during device server process init sequence";
	}

	return;
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::add_source
//
// description : 	Read black box element as strings. The newest element
//			is return in the first position
//
// argument : in : 	- index : The number of element to read
//
//--------------------------------------------------------------------------

void BlackBox::add_source(long index)
{
	switch (box[index].source)
	{
	case DEV :
		elt_str = elt_str + "device ";
		break;

	case CACHE :
		elt_str = elt_str + "cache ";
		break;

	case CACHE_DEV :
		elt_str = elt_str + "cache_device ";
		break;

	default :
		elt_str = elt_str + "unknown source (!) ";
		break;
	}
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::read
//
// description : 	Read black box element as strings. The newest element
//			is return in the first position
//
// argument : in : 	- index : The number of element to read
//
//--------------------------------------------------------------------------

Tango::DevVarStringArray *BlackBox::read(long wanted_elt)
{


//
// Take mutex
//

	sync.lock();

//
// Throw exeception if the wanted element is stupid and if there is no element
// stored in the black box
//

	if (wanted_elt <= 0)
	{
		sync.unlock();

		Except::throw_exception((const char *)"API_BlackBoxArgument",
				      (const char *)"Argument to read black box out of range",
				      (const char *)"BlackBox::read");
	}
	if (nb_elt == 0)
	{
		sync.unlock();

		Except::throw_exception((const char *)"API_BlackBoxEmpty",
				      (const char *)"Nothing stored yet in black-box",
				      (const char *)"BlackBox::read");
	}

//
// Limit wanted element to a reasonable value
//

	if (wanted_elt > max_elt)
		wanted_elt = max_elt;

	if (wanted_elt > nb_elt)
		wanted_elt = nb_elt;

//
// Read black box elements
//

	Tango::DevVarStringArray *ret = NULL;
	try
	{

		ret = new Tango::DevVarStringArray(wanted_elt);
                ret->length(wanted_elt);

		long read_index;
		if (insert_elt == 0)
			read_index = max_elt - 1;
		else
			read_index = insert_elt - 1;
		for (long i = 0;i < wanted_elt;i++)
		{
			build_info_as_str(read_index);
			(*ret)[i] = elt_str.c_str();

			read_index--;
			if (read_index < 0)
				read_index = max_elt - 1;
		}
	}
	catch (bad_alloc)
	{
		sync.unlock();

		Except::throw_exception((const char *)"API_MemoryAllocation",
				      (const char *)"Can't allocate memory in server",
				      (const char *)"BlackBox::read");
	}

//
// Release mutex
//

	sync.unlock();

	return(ret);
}

//+-------------------------------------------------------------------------
//
// method : 		BlackBox::date_ux_to_str
//
// description : 	Convert a UNIX date (number of seconds since EPOCH)
//			to a string of the following format :
//			dd/mm/yyyy hh24:mi:ss:xx
//
// argument : in : 	- ux_date : The UNIX date in a timeval structure
//			- str_date : Pointer to char array where the date will
//				     be stored (must be allocated)
//
//--------------------------------------------------------------------------

void BlackBox::date_ux_to_str(timeval &ux_date,char *str_date)
{
	long i;
	char month[5];
	char *ux_str;
	char unix_date[30];

/* Convert UNIX date to a string in UNIX format */

#ifdef _TG_WINDOWS_
	time_t win_time;
	win_time = (time_t)ux_date.tv_sec;
	ux_str = ctime(&win_time);
#else
	ux_str = ctime((time_t *)&(ux_date.tv_sec));
#endif

	strcpy(unix_date,ux_str);

/* Copy day */

	for (i = 0;i < 2;i++)
		str_date[i] = unix_date[i + 8];
	str_date[2] = '/';
	str_date[3] = '\0';
	if (str_date[0] == ' ')
		str_date[0] = '0';

/* Copy month */

	for (i = 0;i < 3;i++)
		month[i] = unix_date[i + 4];
	month[3] = '\0';

	switch(month[0])
	{
		case 'J' : if (month[1] == 'u')
			   {
				if (month[2] == 'n')
					strcat(str_date,"06/");
				else
					strcat(str_date,"07/");
			   }
			   else
				strcat(str_date,"01/");
			   break;

		case 'F' : strcat(str_date,"02/");
			   break;

		case 'M' : if (month[2] == 'r')
				strcat(str_date,"03/");
			   else
				strcat(str_date,"05/");
			   break;

		case 'A' : if (month[1] == 'p')
				strcat(str_date,"04/");
			   else
				strcat(str_date,"08/");
			   break;

		case 'S' : strcat(str_date,"09/");
			   break;

		case 'O' : strcat(str_date,"10/");
		           break;

		case 'N' : strcat(str_date,"11/");
			   break;

		case 'D' : strcat(str_date,"12/");
			   break;
	}

	str_date[6] = '\0';

/* Copy year */

	strcat(str_date,&(unix_date[20]));
	str_date[10] = '\0';

/* Copy date remaining */

	strcat(str_date," ");
	for (i = 0;i < 8;i++)
		str_date[i + 11] = unix_date[i + 11];
	str_date[19] = '\0';

/* Add milliseconds */

#ifdef _TG_WINDOWS_
	sprintf(&(str_date[19]),":%.2d",(int)(ux_date.tv_usec/10));
#else
	sprintf(&(str_date[19]),":%.2d",(int)(ux_date.tv_usec/10000));
#endif

}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::client_addr
//
// description : 	Copy ctor of the client_addr class
//
//--------------------------------------------------------------------------

client_addr::client_addr(const client_addr &rhs)
{
	client_ident = rhs.client_ident;
	client_lang = rhs.client_lang;
	client_pid = rhs.client_pid;
	java_main_class = rhs.java_main_class;
	java_ident[0] = rhs.java_ident[0];
	java_ident[1] = rhs.java_ident[1];
	memcpy(client_ip,rhs.client_ip,IP_ADDR_BUFFER_SIZE);
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::operator=()
//
// description : 	Assignement operator of the client_addr class
//
//--------------------------------------------------------------------------

client_addr & client_addr::operator=(const client_addr &rhs)
{
	client_ident = rhs.client_ident;
	client_lang = rhs.client_lang;
	client_pid = rhs.client_pid;
	java_main_class = rhs.java_main_class;
	java_ident[0] = rhs.java_ident[0];
	java_ident[1] = rhs.java_ident[1];
	memcpy(client_ip,rhs.client_ip,IP_ADDR_BUFFER_SIZE);
	return *this;
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::operator==()
//
// description : 	Equality operator of the client_addr class
//
//--------------------------------------------------------------------------

bool client_addr::operator==(const client_addr &rhs)
{
	if (client_ident != rhs.client_ident)
		return false;

	if (client_lang != rhs.client_lang)
		return false;
	else
	{
		if (client_lang == Tango::CPP)
		{
			if (client_pid != rhs.client_pid)
				return false;

			char *tmp = client_ip;
			const char *rhs_tmp = rhs.client_ip;

			if (strlen(tmp) != strlen(rhs_tmp))
                return false;

			if (strcmp(tmp,rhs_tmp) != 0)
                return false;
		}
		else
		{
			if (java_ident[0] != rhs.java_ident[0])
				return false;
			if (java_ident[1] != rhs.java_ident[1])
				return false;
		}
	}

	return true;
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::operator!=()
//
// description : 	Operator of the client_addr class
//
//--------------------------------------------------------------------------

bool client_addr::operator!=(const client_addr &rhs)
{
	if (client_ident != rhs.client_ident)
		return true;

	if (client_lang != rhs.client_lang)
		return true;
	else
	{
		if (client_lang == Tango::CPP)
		{
			if (client_pid != rhs.client_pid)
				return true;

			char *tmp = client_ip;
			const char *rhs_tmp = rhs.client_ip;

			if (strlen(tmp) != strlen(rhs_tmp))
				return true;

			if (strcmp(tmp,rhs_tmp) != 0)
				return true;
		}
		else
		{
			if (java_ident[0] != rhs.java_ident[0])
				return true;
			if (java_ident[1] != rhs.java_ident[1])
				return true;
		}
	}

	return false;
}

//+-------------------------------------------------------------------------
//
// method : 		client_addr::client_ip_2_client_name()
//
// description : 	Convert client host IP address to client host name
//
//--------------------------------------------------------------------------

int client_addr::client_ip_2_client_name(string &cl_host_name) const
{
	int ret = 0;
	string client_ip_str(client_ip);

	string::size_type pos;
	if ((pos = client_ip_str.find(':')) == string::npos)
		ret = -1;
	else
	{
		pos++;
		if ((pos = client_ip_str.find(':',pos)) == string::npos)
			ret = -1;
		else
		{
			pos++;
			string ip_str = client_ip_str.substr(pos);
			if ((pos = ip_str.find(':')) == string::npos)
				ret = -1;
			else
			{
				string full_ip_str = ip_str.substr(0,pos);

				if ((pos = full_ip_str.find('.')) == string::npos)
					ret = -1;
				else
				{
					string ip1_str = full_ip_str.substr(0,pos);

					string::size_type old_pos;
					pos++;
					old_pos = pos;
					if ((pos = full_ip_str.find('.',pos)) == string::npos)
						ret = -1;
					else
					{
						string ip2_str = full_ip_str.substr(old_pos,pos - old_pos);
						pos++;
						old_pos = pos;
						if ((pos = full_ip_str.find('.',pos)) == string::npos)
							ret = -1 ;
						else
						{
							string ip3_str = full_ip_str.substr(old_pos,pos - old_pos);
							pos++;
							string ip4_str = full_ip_str.substr(pos);

							struct sockaddr_in si;
							si.sin_family = AF_INET;
							si.sin_port = 0;
#ifdef _TG_WINDOWS_
							int slen = sizeof(si);
							WSAStringToAddress((char *)full_ip_str.c_str(),AF_INET,NULL,(SOCKADDR *)&si,&slen);
#else
							inet_pton(AF_INET,full_ip_str.c_str(),&si.sin_addr);
#endif

							char host[512];

							int res = getnameinfo((const sockaddr *)&si,sizeof(si),host,512,0,0,0);

							if (res == 0)
							{
								cl_host_name = host;
								ret = 0;
							}
							else
								ret = -1;
						}
					}
				}
			}
		}
	}

	return ret;
}


//+-------------------------------------------------------------------------
//
// operator overloading : 	<<
//
// description : 	Friend function to ease printing instance of the
//					client_addr class
//
//--------------------------------------------------------------------------

ostream &operator<<(ostream &o_str,const client_addr &ca)
{
	if (ca.client_ident == false)
		o_str << "Client identification not available";
	else
	{
		if (ca.client_lang == Tango::CPP)
		{
			string cl_name;
			o_str << "CPP or Python client with PID " << ca.client_pid << " from host ";
			if (ca.client_ip_2_client_name(cl_name) == 0)
				o_str << cl_name;
			else
				o_str << ca.client_ip;
		}
		else
		{
			o_str << "JAVA client class " << ca.java_main_class << " from host ";
			string cl_name;
			if (ca.client_ip_2_client_name(cl_name) == 0)
				o_str << cl_name;
			else
				o_str << ca.client_ip;
		}
	}

	return o_str;
}


} // End of Tango namespace

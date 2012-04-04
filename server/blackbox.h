//=============================================================================
//
// file :               BlackBox.h
//
// description :        Include for the BlackBox object. This class implements
//                      the black box objects which keep tracks of all
//			operation invoke on a device or attribute retrieved.
//			This black box is managed as a circular buffer
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
//=============================================================================

#ifndef _BLACKBOX_H
#define _BLACKBOX_H

#include <tango.h>
#ifdef _TG_WINDOWS_
#include <winsock.h>
#endif
#include <time.h>
#include <omniORB4/omniInterceptors.h>

namespace Tango
{

#define		IP_ADDR_BUFFER_SIZE		80

CORBA::Boolean get_client_addr(omni::omniInterceptors::serverReceiveRequest_T::info_T &);

class client_addr: public omni_thread::value_t
{
public:
    client_addr() {client_ip[0]='\0';::memset(java_ident,0,sizeof(DevULong64)<<1);}
	client_addr(const char *addr):client_ident(false),client_pid(0) {strcpy(client_ip,addr);}
	~client_addr() {};

	client_addr(const client_addr &);
	client_addr & operator=(const client_addr &);
	bool operator==(const client_addr &);
	bool operator!=(const client_addr &);

	bool				client_ident;
	char				client_ip[IP_ADDR_BUFFER_SIZE];
	LockerLanguage		client_lang;
	TangoSys_Pid		client_pid;
	string				java_main_class;
	DevULong64			java_ident[2];

	int client_ip_2_client_name(string &) const;
	friend ostream &operator<<(ostream &o_str,const client_addr &ca);
};

//=============================================================================
//
//			The BlackBoxElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================

#define 	DEFAULT_ATTR_NB		10

enum BlackBoxElt_ReqType
{
	Req_Unknown,
	Req_Operation,
	Req_Attribute
};

enum BlackBoxElt_AttrType
{
	Attr_Unknown,
	Attr_Name,
	Attr_Description,
	Attr_State,
	Attr_Status,
	Attr_AdmName
};

enum BlackBoxElt_OpType
{
	Op_Unknown,
	Op_Command_inout,
	Op_BlackBox,
	Op_Ping,
	Op_Info,
	Op_Command_list,
	Op_Command,
	Op_Get_Attr_Config,
	Op_Set_Attr_Config,
	Op_Read_Attr,
	Op_Write_Attr,
	Op_Command_inout_2,
	Op_Command_list_2,
	Op_Command_2,
	Op_Get_Attr_Config_2,
	Op_Read_Attr_2,
	Op_Command_inout_history_2,
	Op_Read_Attr_history_2,
	Op_Read_Attr_3,
	Op_Write_Attr_3,
	Op_Read_Attr_history_3,
	Op_Info_3,
	Op_Get_Attr_Config_3,
	Op_Set_Attr_Config_3,
	Op_Read_Attr_history_4,
	Op_Command_inout_history_4,
	Op_Command_inout_4,
	Op_Write_Attr_4,
	Op_Read_Attr_4,
	Op_Set_Attr_Config_4,
	Op_Write_Read_Attributes_4
};

class BlackBoxElt
{
public:
	BlackBoxElt();
	~BlackBoxElt();

	BlackBoxElt_ReqType		req_type;
	BlackBoxElt_AttrType	attr_type;
	BlackBoxElt_OpType		op_type;
	string					cmd_name;
	vector<string>			attr_names;
	struct timeval			when;
	char					host_ip_str[IP_ADDR_BUFFER_SIZE];
	DevSource				source;

	bool					client_ident;
	LockerLanguage			client_lang;
	TangoSys_Pid			client_pid;
	string					java_main_class;
};

inline bool operator<(const BlackBoxElt &,const BlackBoxElt &)
{
	return true;
}

inline bool operator==(const BlackBoxElt &,const BlackBoxElt &)
{
	return true;
}

//=============================================================================
//
//			The BlackBox class
//
// description :	Class to implement the black box itself. This is mainly
//			a vector of BlackBoxElt managed as a circular vector
//
//=============================================================================

class BlackBox
{
public:
	BlackBox();
	BlackBox(long);

	void insert_corba_attr(BlackBoxElt_AttrType);
	void insert_cmd(const char *,long vers=1,DevSource=Tango::DEV);
	void insert_attr(const Tango::DevVarStringArray &,long vers=1,DevSource=Tango::DEV);
	void insert_attr(const Tango::DevVarStringArray &,const ClntIdent &,long vers=1,DevSource=Tango::DEV);
	void insert_attr(const Tango::AttributeValueList &,long vers=1);
	void insert_attr(const Tango::AttributeValueList_4 &,const ClntIdent &,long vers);
	void insert_wr_attr(const Tango::AttributeValueList_4 &,const ClntIdent &,long vers);
	void insert_op(BlackBoxElt_OpType);
	void insert_op(BlackBoxElt_OpType,const ClntIdent &);

	void insert_cmd_nl(const char *,long,DevSource);
	void insert_cmd_cl_ident(const char *,const ClntIdent &,long vers=1,DevSource=Tango::DEV);
	void add_cl_ident(const ClntIdent &,client_addr *);
	void update_client_host(client_addr *);

	Tango::DevVarStringArray *read(long);

private:

	void inc_indexes();
	void get_client_host();
	void build_info_as_str(long);
	void date_ux_to_str(struct timeval &,char *);
	void add_source(long);
	void insert_op_nl(BlackBoxElt_OpType);
	void insert_attr_nl(const Tango::AttributeValueList &,long);
	void insert_attr_nl_4(const Tango::AttributeValueList_4 &);
	void insert_attr_wr_nl(const Tango::AttributeValueList_4 &,long);

	vector<BlackBoxElt>	box;
	long				insert_elt;
	long				nb_elt;
	long				max_elt;

	omni_mutex			sync;

	string				elt_str;
};

} // End of Tango namespace

#endif /* _BLACKBOX_ */

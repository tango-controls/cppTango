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
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:39  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:13  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:28  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _BLACKBOX_H
#define _BLACKBOX_H

#include <OB/OCI_IIOP.h>

#include <tango.h>
#ifdef WIN32
#include <winsock.h>
#endif
#include <time.h>

namespace Tango
{

//=============================================================================
//
//			The BlackBoxElt class
//
// description :	Class to store all the necessary information which will
//			be stored and returned to client on request
//
//=============================================================================


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
	Attr_Status
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
	Op_Write_Attr
};

class BlackBoxElt
{
public:
	BlackBoxElt();

	BlackBoxElt_ReqType		req_type;
	BlackBoxElt_AttrType		attr_type;
	BlackBoxElt_OpType		op_type;
	string				host;
	string				cmd_name;
	struct timeval			when;
	OCI::IIOP::InetAddr_slice	*host_ip;	
};

inline bool operator<(const BlackBoxElt &l,const BlackBoxElt &r)
{
	return true;
}

inline bool operator==(const BlackBoxElt &l,const BlackBoxElt &r)
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

	void insert_attr(BlackBoxElt_AttrType);
	void insert_cmd(const char *);
	void insert_op(BlackBoxElt_OpType);
	
	void insert_attr(BlackBoxElt_AttrType,const char *);
	void insert_cmd(const char *,const char *);
	void insert_op(BlackBoxElt_OpType,const char *);
	
	Tango::DevVarStringArray *read(long);
	
private:

	void inc_indexes();
	void get_client_host();
	void build_info_as_str(long);
	void date_ux_to_str(struct timeval &,char *);
	
	vector<BlackBoxElt>	box;
	long			insert_elt;
	long			nb_elt;
	long			max_elt;
	
	string			elt_str;
	
	CORBA::ORB_ptr		orb;
};

} // End of Tango namespace

#endif /* _BLACKBOX_ */

//=============================================================================
//
// file :               Tango_const.h
//
// description :        Include for Tango system constant definition  
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:42  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:16  taurel
// Just update revision number
//
// Revision 1.1.1.1  2000/02/04 10:58:27  taurel
// Imported sources
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _TANGO_CONST_H
#define _TANGO_CONST_H

namespace Tango
{



#define		DbErr_DeviceNotDefined  	1

//
// Some general interest define
//

#define		DevVersion			1
#define		DefaultMaxSeq			20
#define		DefaultBlackBoxDepth		25

#define		InitialOutput			"Initial Output"
#define		DSDeviceDomain  		"dserver"
#define		DefaultDocUrl			"http://www.ulla.com"
#define		EnvVariable			"TANGO_HOST"
#define		DbObjName			"database"
#define		DescNotSet			"Uninitialised"
#define		ResNotDefined			"0"
#define         MessBoxTitle			"Tango Device Server"

#define		DefaultWritAttrProp		false
#define		AllAttr 			"All attributes"

//
// Tango name length
//

#define		MaxServerNameLength		255

//
// Many, many typedef
//

typedef	const char *	ConstDevString;	// Pseudo Tango type to ease POGO job

class DeviceImpl;

typedef bool (DeviceImpl::*StateMethPtr)(const CORBA::Any &);

typedef void (DeviceImpl::*CmdMethPtr)();

typedef void (DeviceImpl::*CmdMethPtr_Bo)(DevBoolean);
typedef void (DeviceImpl::*CmdMethPtr_Sh)(DevShort);
typedef void (DeviceImpl::*CmdMethPtr_Lg)(DevLong);
typedef void (DeviceImpl::*CmdMethPtr_Fl)(DevFloat);
typedef void (DeviceImpl::*CmdMethPtr_Db)(DevDouble);
typedef void (DeviceImpl::*CmdMethPtr_US)(DevUShort);
typedef void (DeviceImpl::*CmdMethPtr_UL)(DevULong);
typedef void (DeviceImpl::*CmdMethPtr_Str)(DevString);
typedef void (DeviceImpl::*CmdMethPtr_ChA)(const DevVarCharArray *);
typedef void (DeviceImpl::*CmdMethPtr_ShA)(const DevVarShortArray *);
typedef void (DeviceImpl::*CmdMethPtr_LgA)(const DevVarLongArray *);
typedef void (DeviceImpl::*CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef void (DeviceImpl::*CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef void (DeviceImpl::*CmdMethPtr_USA)(const DevVarUShortArray *);
typedef void (DeviceImpl::*CmdMethPtr_ULA)(const DevVarULongArray *);
typedef void (DeviceImpl::*CmdMethPtr_StrA)(const DevVarStringArray *);
typedef void (DeviceImpl::*CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef void (DeviceImpl::*CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef void (DeviceImpl::*CmdMethPtr_Sta)(DevState);

typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr)();
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr)();
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr)();
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr)();
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr)();
typedef DevUShort (DeviceImpl::*US_CmdMethPtr)();
typedef DevULong (DeviceImpl::*UL_CmdMethPtr)();
typedef DevString (DeviceImpl::*Str_CmdMethPtr)();
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr)();
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr)();
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr)();
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr)();
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr)();
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr)();
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr)();
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr)();
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr)();
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr)();
typedef DevState (DeviceImpl::*Sta_CmdMethPtr)();

typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Bo)(DevBoolean);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Sh)(DevShort);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Lg)(DevLong);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Fl)(DevFloat);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Db)(DevDouble);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_US)(DevUShort);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_UL)(DevULong);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Str)(DevString);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevBoolean (DeviceImpl::*Bo_CmdMethPtr_Sta)(DevState);

typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Bo)(DevBoolean);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Sh)(DevShort);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Lg)(DevLong);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Fl)(DevFloat);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Db)(DevDouble);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_US)(DevUShort);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_UL)(DevULong);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Str)(DevString);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevShort (DeviceImpl::*Sh_CmdMethPtr_Sta)(DevState);

typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Bo)(DevBoolean);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Sh)(DevShort);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Lg)(DevLong);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Fl)(DevFloat);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Db)(DevDouble);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_US)(DevUShort);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_UL)(DevULong);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Str)(DevString);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevLong (DeviceImpl::*Lg_CmdMethPtr_Sta)(DevState);

typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Bo)(DevBoolean);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Sh)(DevShort);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Lg)(DevLong);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Fl)(DevFloat);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Db)(DevDouble);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_US)(DevUShort);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_UL)(DevULong);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Str)(DevString);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevFloat (DeviceImpl::*Fl_CmdMethPtr_Sta)(DevState);

typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Bo)(DevBoolean);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Sh)(DevShort);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Lg)(DevLong);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Fl)(DevFloat);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Db)(DevDouble);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_US)(DevUShort);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_UL)(DevULong);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Str)(DevString);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevDouble (DeviceImpl::*Db_CmdMethPtr_Sta)(DevState);

typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Bo)(DevBoolean);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Sh)(DevShort);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Lg)(DevLong);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Fl)(DevFloat);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Db)(DevDouble);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_US)(DevUShort);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_UL)(DevULong);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Str)(DevString);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevUShort (DeviceImpl::*US_CmdMethPtr_Sta)(DevState);

typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Bo)(DevBoolean);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Sh)(DevShort);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Lg)(DevLong);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Fl)(DevFloat);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Db)(DevDouble);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_US)(DevUShort);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_UL)(DevULong);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Str)(DevString);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevULong (DeviceImpl::*UL_CmdMethPtr_Sta)(DevState);

typedef DevString (DeviceImpl::*Str_CmdMethPtr_Bo)(DevBoolean);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Sh)(DevShort);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Lg)(DevLong);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Fl)(DevFloat);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Db)(DevDouble);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_US)(DevUShort);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_UL)(DevULong);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Str)(DevString);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevString (DeviceImpl::*Str_CmdMethPtr_Sta)(DevState);

typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Sh)(DevShort);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Lg)(DevLong);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Db)(DevDouble);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_US)(DevUShort);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_UL)(DevULong);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Str)(DevString);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarCharArray *(DeviceImpl::*ChA_CmdMethPtr_Sta)(DevState);

typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Sh)(DevShort);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Lg)(DevLong);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Db)(DevDouble);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_US)(DevUShort);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_UL)(DevULong);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Str)(DevString);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarShortArray *(DeviceImpl::*ShA_CmdMethPtr_Sta)(DevState);

typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Sh)(DevShort);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Lg)(DevLong);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Db)(DevDouble);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_US)(DevUShort);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_UL)(DevULong);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Str)(DevString);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarLongArray *(DeviceImpl::*LgA_CmdMethPtr_Sta)(DevState);

typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Sh)(DevShort);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Lg)(DevLong);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Db)(DevDouble);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_US)(DevUShort);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_UL)(DevULong);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Str)(DevString);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarFloatArray *(DeviceImpl::*FlA_CmdMethPtr_Sta)(DevState);

typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Sh)(DevShort);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Lg)(DevLong);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Db)(DevDouble);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_US)(DevUShort);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_UL)(DevULong);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Str)(DevString);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarDoubleArray *(DeviceImpl::*DbA_CmdMethPtr_Sta)(DevState);

typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Sh)(DevShort);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Lg)(DevLong);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Db)(DevDouble);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_US)(DevUShort);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_UL)(DevULong);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Str)(DevString);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarUShortArray *(DeviceImpl::*USA_CmdMethPtr_Sta)(DevState);

typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Sh)(DevShort);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Lg)(DevLong);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Db)(DevDouble);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_US)(DevUShort);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_UL)(DevULong);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Str)(DevString);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarULongArray *(DeviceImpl::*ULA_CmdMethPtr_Sta)(DevState);

typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Sh)(DevShort);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Lg)(DevLong);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Db)(DevDouble);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_US)(DevUShort);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_UL)(DevULong);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Str)(DevString);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarStringArray *(DeviceImpl::*StrA_CmdMethPtr_Sta)(DevState);

typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Sh)(DevShort);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Lg)(DevLong);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Db)(DevDouble);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_US)(DevUShort);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_UL)(DevULong);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Str)(DevString);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarLongStringArray *(DeviceImpl::*LSA_CmdMethPtr_Sta)(DevState);

typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Bo)(DevBoolean);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Sh)(DevShort);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Lg)(DevLong);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Fl)(DevFloat);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Db)(DevDouble);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_US)(DevUShort);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_UL)(DevULong);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Str)(DevString);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevVarDoubleStringArray *(DeviceImpl::*DSA_CmdMethPtr_Sta)(DevState);

typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Bo)(DevBoolean);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Sh)(DevShort);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Lg)(DevLong);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Fl)(DevFloat);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Db)(DevDouble);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_US)(DevUShort);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_UL)(DevULong);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Str)(DevString);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_ChA)(const DevVarCharArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_ShA)(const DevVarShortArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_LgA)(const DevVarLongArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_FlA)(const DevVarFloatArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_DbA)(const DevVarDoubleArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_USA)(const DevVarUShortArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_ULA)(const DevVarULongArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_StrA)(const DevVarStringArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_LSA)(const DevVarLongStringArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_DSA)(const DevVarDoubleStringArray *);
typedef DevState *(DeviceImpl::*Sta_CmdMethPtr_Sta)(DevState);


//
// Some enum
//

enum CmdArgType {
	DEV_VOID,
	DEV_BOOLEAN,
	DEV_SHORT,
	DEV_LONG,
	DEV_FLOAT,
	DEV_DOUBLE,
	DEV_USHORT,
	DEV_ULONG,
	DEV_STRING,
	DEVVAR_CHARARRAY,
	DEVVAR_SHORTARRAY,
	DEVVAR_LONGARRAY,
	DEVVAR_FLOATARRAY,
	DEVVAR_DOUBLEARRAY,
	DEVVAR_USHORTARRAY,
	DEVVAR_ULONGARRAY,
	DEVVAR_STRINGARRAY,
	DEVVAR_LONGSTRINGARRAY,
	DEVVAR_DOUBLESTRINGARRAY,
	DEV_STATE,
	CONST_DEV_STRING
};

enum MessBoxType {
        STOP,
        INFO
};

//
// The command argument name
//

const char * const CmdArgTypeName[] = {
	"DevVoid",
	"DevBoolean",
	"DevShort",
	"DevLong",
	"DevFloat",
	"DevDouble",
	"DevUShort",
	"DevULong",
	"DevString",
	"DevVarCharArray",
	"DevVarShortArray",
	"DevVarLongArray",
	"DevVarFloatArray",
	"DevVarDoubleArray",
	"DevVarUShortArray",
	"DevVarULongArray",
	"DevVarStringArray",
	"DevVarLongStringArray",
	"DevVarDoubleStringArray",
	"DevState",
	"ConstDevString"
};

//
// The state name
//

const char * const DevStateName[] = {
	"ON",
	"OFF",
	"CLOSE",
	"OPEN",
	"INSERT",
	"EXTRACT",
	"MOVING",
	"STANDBY",
	"FAULT",
	"INIT",
	"RUNNING",
	"ALARM",
	"DISABLE",
	"UNKNOWN"
};

//
// The optional attribute properties
//

#define		AlrmValueNotSpec	  "Not specified"
#define		AssocWritNotSpec	  "None"

typedef struct _OptAttrProp
{
	const char *name;
	const char *default_value;
}OptAttrProp;

/*
 * List of strings used by the API as the DevError reason field.
 * This list is given here only for API writers to re-use (if possible)
 * strings already used.
 *
 *
 * API_MemoryAllocation		 
 * API_CommandNotFound 		 
 * API_IncompatibleCmdArgumentType	 
 * API_BlackBoxArgument		 
 * API_BlackBoxEmpty			 
 * API_CannotOpenFile  		 
 * API_SignalOutOfRange		 
 * API_CantInstallSignal		 
 * API_CommandNotAllowed		 
 * API_CantRetrieveClassList		 
 * API_ClassNotFound			 
 * API_InitMethodNotFound		 
 * API_JavaRuntimeSecurityException	 
 * API_InitThrowsException		 
 * API_InitNotPublic			 
 * API_DeviceNotFound  		 
 * API_CmdArgumentTypeNotSupported	 
 * API_MethodNotFound  		 
 * API_OverloadingNotSupported 	 
 * API_MethodArgument  		 
 * API_AttrWrongDefined		 
 * API_AttrOptProp			 
 * API_AttrNotFound			 
 * API_AttrValueNotSet 		 
 * API_AttrNoAlarm			 
 * API_AttrNotWritable 		 
 * API_IncompatibleAttrDataType	 
 * API_WAttrOutsideLimit		 
 * API_NtDebugWindowError		 
 * API_CantCreateClassPoa		 
 * API_CantGetDevObjectId		 
 * API_CantActivatePOAManager
 * API_DatabaseAccess
 * API_CorbaSysException  
 */

} // End of Tango namespace

		
#endif /* TANGO_CONST_H */

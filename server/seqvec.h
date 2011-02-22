//=============================================================================
//
// file :               SeqVec.h
//
// description :        Include for the utilities function to ease CORBA
//			sequences from standard C++ vector or in the 
//			oposite way.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
// Revision 3.3  2007/03/06 08:19:03  taurel
// - Added 64 bits data types for 64 bits computer...
//
// Revision 3.2  2006/05/18 08:51:56  taurel
// - Miscellaneous changes due to Python device server ported to Windows
// - Fix some bugs discovered by Windows VC8 using the test suite
// - Update Windows resource file include path
// - Fix some Windows VC8 warnings
//
// Revision 3.1  2005/01/13 08:30:00  taurel
// - Merge trunk with Release_5_0 from brach Release_5_branch
//
// Revision 3.0.6.1  2004/09/15 06:47:17  taurel
// - Added four new types for attributes (boolean, float, unsigned short and unsigned char)
// - It is also possible to read state and status as attributes
// - Fix bug in Database::get_class_property() method (missing ends insertion)
// - Fix bug in admin device DevRestart command (device name case problem)
//
// Revision 3.0  2003/03/25 16:44:11  taurel
// Many changes for Tango release 3.0 including
// - Added full logging features
// - Added asynchronous calls
// - Host name of clients now stored in black-box
// - Three serialization model in DS
// - Fix miscellaneous bugs
// - Ported to gcc 3.2
// - Added ApiUtil::cleanup() and destructor methods
// - Some internal cleanups
// - Change the way how TangoMonitor class is implemented. It's a recursive
//   mutex
//
// Revision 2.9  2003/03/11 17:55:57  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.8  2002/12/16 12:07:33  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:16:23  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:07  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:20  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:55  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:25  taurel
// Miscellaneous small changes/bug fixes for Tango CPP release 2.1.0
//     - classes reference documentation now generated using doxygen instead of doc++
//     - A little file added to the library which summarizes version number.
//       The RCS/CVS "ident" command will now tells you that release library x.y.z is composed
//       by C++ client classes set release a.b and C++ server classes set release c.d
//     - Fix incorrect field setting for DevFailed exception re-thrown from a CORBA exception
//     - It's now not possible to poll the Init command
//     - It's now possible to define a default class doc. per control system
//       instance (using property)
//     - The test done to check if attribute value has been set before it is
//       returned to caller is done only if the attribute quality is set to VALID
//     - The JTCInitialize object is now stored in the Util
//     - Windows specific : The tango.h file now also include winsock.h
//
// Revision 2.2  2002/04/30 10:50:42  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:04  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:11  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:14  taurel
// See tango WEB pages for list of changes
//
// Revision 1.1  2001/07/04 12:27:11  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================

#ifndef _SECVEC_H
#define _SECVEC_H


#include <tango.h>


namespace Tango
{


//
// These functions are not defined within the tango namespace because the VC++
// is not able to understand that the operator overloading functions is defined
// within a namespace x from the operand namespace (c++ or aCC is able to 
// do it)
//

//=============================================================================
//
//			Operator for the unsigned char array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarCharArray) and C++ vectors
//
//=============================================================================

//#define NOSPACEINDOC_SEQVEC

/** @defgroup Ope Operator overloading functions
 * Overloading of the << operator between C++ vector and Tango types */
//@{
/**
 * 
 * Init a DevVarCharArray from a C++ vector of char(s).
 *
 * @param lval The DevVarCharArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarCharArray &lval,const vector<unsigned char> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of char from a DevVarCharArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarCharArray
 */
inline void operator<<(vector<unsigned char> &lval,const DevVarCharArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Short array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarShortArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarShortArray from a C++ vector of short(s).
 *
 * @param lval The DevVarShortArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarShortArray &lval,const vector<short> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of short from a DevVarShortArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarShortArray
 */
inline void operator<<(vector<short> &lval,const DevVarShortArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarLongArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarLongArray from a C++ vector of DevLong(s).
 *
 * @param lval The DevVarLongArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarLongArray &lval,const vector<DevLong> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of DevLong from a DevVarLongArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarLongArray
 */
inline void operator<<(vector<DevLong> &lval,const DevVarLongArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Long Long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarLong64Array) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarLongArray from a C++ vector of DevLong64(s).
 *
 * @param lval The DevVarLong64Array to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarLong64Array &lval,const vector<DevLong64> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarLong64Array.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarLong64Array
 */
inline void operator<<(vector<DevLong64> &lval,const DevVarLong64Array &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Float array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarLongArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarFloatArray from a C++ vector of float(s).
 *
 * @param lval The DevVarFloatArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarFloatArray &lval,const vector<float> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarFloatArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarFloatArray
 */
 
inline void operator<<(vector<float> &lval,const DevVarFloatArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Double array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarDoubleArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarDoubleArray from a C++ vector of double(s).
 *
 * @param lval The DevVarDoubleArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarDoubleArray &lval,const vector<double> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarDoubleArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarDoubleArray
 */
 
inline void operator<<(vector<double> &lval,const DevVarDoubleArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Boolean array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarBooleanArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarBooleanArray from a C++ vector of bool(s).
 *
 * @param lval The DevVarBooleanArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarBooleanArray &lval,const vector<bool> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarBooleanArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarBooleanArray
 */
 
inline void operator<<(vector<bool> &lval,const DevVarBooleanArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Unsigned short array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarUShortArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarUShortArray from a C++ vector of unsigned short(s).
 *
 * @param lval The DevVarUShortArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarUShortArray &lval,const vector<unsigned short> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarUShortArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarUShortArray
 */
 
inline void operator<<(vector<unsigned short> &lval,const DevVarUShortArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the Unsigned long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarUShortArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarULongArray from a C++ vector of DevULong(s).
 *
 * @param lval The DevVarULongArray to be initialised
 * @param rval The C++ vector
 */
 
inline void operator<<(DevVarULongArray &lval,const vector<DevULong> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarULongArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarULongArray
 */
inline void operator<<(vector<DevULong> &lval,const DevVarULongArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}


//=============================================================================
//
//			Operator for the Unsigned long long array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarULong64Array) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarULong64Array from a C++ vector of DevULong64(s).
 *
 * @param lval The DevVarULong64Array to be initialised
 * @param rval The C++ vector
 */
 
inline void operator<<(DevVarULong64Array &lval,const vector<DevULong64> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarULong64Array.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarULong64Array
 */
inline void operator<<(vector<DevULong64> &lval,const DevVarULong64Array &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//=============================================================================
//
//			Operator for the string array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarStringArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarStringArray from a C++ vector of string(s).
 *
 * @param lval The DevVarStringArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarStringArray &lval,const vector<string> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = CORBA::string_dup(rval[i].c_str());
}

/**
 * Init a C++ vector of long from a DevVarStringArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarStringArray
 */
inline void operator<<(vector<string> &lval,const DevVarStringArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	string str;
	for (long i = 0;i < nb_elt;i++)
	{
		str = rval[i];
		lval.push_back(str);
	}
}
		
//=============================================================================
//
//			Operator for the DevState array types
//
// description :	These two operators allow simple insertion between
//			Tango sequence (DevVarStateArray) and C++ vectors
//
//=============================================================================

/**
 * Init a DevVarStateArray from a C++ vector of state(s).
 *
 * @param lval The DevVarStateArray to be initialised
 * @param rval The C++ vector
 */
inline void operator<<(DevVarStateArray &lval,const vector<DevState> &rval)
{
	size_t nb_elt = rval.size();
	lval.length((CORBA::ULong)nb_elt);
	for (unsigned long i = 0;i < nb_elt;i++)
		lval[i] = rval[i];
}

/**
 * Init a C++ vector of long from a DevVarStateArray.
 *
 * @param lval The C++ vector to be initialised
 * @param rval The DevVarStateArray
 */
inline void operator<<(vector<DevState> &lval,const DevVarStateArray &rval)
{
	long nb_elt = rval.length();
	if (lval.size() != 0)
		lval.clear();
	for (long i = 0;i < nb_elt;i++)
		lval.push_back(rval[i]);
}

//@}

//=============================================================================
//
//			Print operator for sequence types
//
// description :	These operators allow simple printing of sequences
//
//=============================================================================
//#ifndef TANGO_HAS_LOG4TANGO
/** @defgroup Eas Easy printing operator overloading functions
 * Overloading of the << operator between C++ ostream and some Tango types */
//@{
/**
 * Print a DevVarCharArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarCharArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarCharArray &rval);
/**
 * Print a DevVarShortArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarShortArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarShortArray &rval);
/**
 * Print a DevVarLongArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarLongArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarLongArray &rval);
/**
 * Print a DevVarLong64Array.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarLong64Array sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarLong64Array &rval);
/**
 * Print a DevVarFloatArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarFloatArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarFloatArray &rval);
/**
 * Print a DevVarDoubleArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarDoubleArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarDoubleArray &rval);
/**
 * Print a DevVarBooleanArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarBooleanArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarBooleanArray &rval);
/**
 * Print a DevVarUShortArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarUShortArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarUShortArray &rval);
/**
 * Print a DevVarULongArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarULongArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarULongArray &rval);
/**
 * Print a DevVarULong64Array.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarULong64Array sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarULong64Array &rval);
/**
 * Print a DevVarStringArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarStringArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarStringArray &rval);
/**
 * Print a DevVarStateArray.
 *
 * One line is printed for each sequence element.
 *
 * @param lval The C++ stream used for printing
 * @param rval The DevVarStateArray sequence to be printed
 */
ostream &operator<<(ostream &lval,const DevVarStateArray &rval);
//@}

//#endif // TANGO_HAS_LOG4TANGO

} // End of Tango namespace


#endif /* _SECVEC_ */

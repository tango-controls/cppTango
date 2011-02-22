//=============================================================================
//
// file :               SeqVec.cpp
//
// description :        Dource file for CORBA sequence printing functions.
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// $Revision$
//
// $Log$
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
//
// copyleft :           European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
//=============================================================================


#include <seqvec.h>

namespace Tango
{

//
// These functions are not defined within the tango namespace because the VC++
// is not able to understand that the operator overloading functions is defined
// within a namespace x from th eoerand namespace (c++ or aCC is able to
// do it)
//

//=============================================================================
//
//			The sequence print facilities functions
//
// description :	These methods allow an easy way to print all seuence
//			element using the following syntax
//				cout << seq << endl;
//
//=============================================================================


ostream &operator<<(ostream &o,const DevVarCharArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarLongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarFloatArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarDoubleArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarUShortArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarULongArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i];
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

ostream &operator<<(ostream &o,const DevVarStringArray &v)
{
	long nb_elt = v.length();
	for (long i = 0;i < nb_elt;i++)
	{
		o << "Element number [" << i << "] = " << v[i].in();
		if (i < (nb_elt - 1))
			o << endl;
	}
	return o;
}

} // End of Tango namespace

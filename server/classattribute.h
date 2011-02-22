//=============================================================================	
//
// file :		ClassAttribute.h
//
// description :	Include file for the Tango attribute.
//			Three classes are declared in this file :
//
//			AttrProperty : A helper class. This class stored a couple
//				       property name, property value as strings
//		 	ClassAttribute : A class for each attribute with all
//					 its properties defined at the class
//					 level. The properties are still stored
//					 as string
//			MultiClassAttribute : A class to manage all the class
//					      level definition of attributes.
//					      There is one instance of this class
//					      for each tango device pattern
//
// project :		TANGO
//
// author(s) :		E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
// $Log$
// Revision 3.8  2010/08/25 11:41:27  taurel
// - Fix some bugs preventing dynamic attributes management (in some cases)
//
// Revision 3.7  2009/01/21 12:49:04  taurel
// - Change CopyRights for 2009
//
// Revision 3.6  2008/10/06 15:00:36  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.5  2008/10/03 06:51:36  taurel
// - Add some licensing info in each files
//
// Revision 3.4  2005/06/29 08:30:53  taurel
// - Last commit before release 5.2 ?
//
// Revision 3.3  2004/07/07 08:39:57  taurel
//
// - Fisrt commit after merge between Trunk and release 4 branch
// - Add EventData copy ctor, asiignement operator and dtor
// - Add Database and DeviceProxy::get_alias() method
// - Add AttributeProxy ctor from "device_alias/attribute_name"
// - Exception thrown when subscribing two times for exactly yhe same event
//
// Revision 3.2  2003/08/22 12:52:54  taurel
// - For device implementing release 3 of IDL (derivating from device_3impl), change
//   the way how attributes are read or written
// - Fix small bug in vector iterator usage in case of "erase()" method called in
//   a for loop
//
// Revision 3.1  2003/08/21 07:23:46  taurel
// - End of the implementation of the new way to transfer data for read and
//   write attributes (better use of exception)
// - Added Attribute::set_date() and Attribute::set_value_date_quality() methods
// - Added DeviceAttribute ctors from "const char *"
// - Enable writing of spectrum and image attributes
// - Many new DeviceAttribute ctors/inserters to enable easy image and spectrums
//   attribute writing
// - Attribute date automatically set in case of attribute quality factor set to INVALID
// - Change in the polling thread discarding element algo. to support case of polling
//   several cmd/atts at the same polling period with cmd/attr having a long response time
// - Take cmd/attr execution time into account in the "Data not updated since" polling
//   status string
// - Split "str().c_str()" code in two lines of code. It was the reason of some problem
//   on Windows device server
// - Add the possibility to set a cmd/attr polling as "externally triggered". Add method
//   to send trigger to the polling thread
//
// Revision 3.0.4.1  2003/09/30 11:49:56  taurel
// Add some changes foreseen for release 4.1 and already implemented on
// the trunck into this release 4.0 branch
//
// Revision 3.0  2003/03/25 16:41:48  taurel
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
// Revision 2.10  2003/03/11 17:55:47  nleclercq
// Switch from log4cpp to log4tango
//
// Revision 2.9  2003/02/17 14:57:39  taurel
// Added the new Tango logging stuff (Thanks Nicolas from Soleil)
//
// Revision 2.8  2002/12/16 12:06:21  taurel
// No change in code at all but only forgot th emost important line in
// list of updates in the previous release :
// - Change underlying ORB from ORBacus to omniORB
//
// Revision 2.7  2002/12/16 10:15:36  taurel
// - New method get_device_list() in Util class
// - Util::get_class_list takes DServer device into account
// - Util::get_device_by_name() takes DServer device into account
// - Util::get_device_list_by_class() takes DServer device into account
// - New parameter to the attribute::set_value() method to enable CORBA to free
// memory allocated for the attribute
//
// Revision 2.6  2002/10/17 07:43:05  taurel
// Fix bug in history stored by the polling thread :
// - We need one copy of the attribute data to build an history!!! It is true
// also for command which return data created by the DeviceImpl::create_xxx
// methods. Chnage in pollring.cpp/pollring.h/dserverpoll.cpp/pollobj.cpp
// and pollobj.h
//
// Revision 2.5  2002/10/15 11:27:18  taurel
// Fix bugs in device.cpp file :
// - Protect the state and status CORBA attribute with the device monitor
// Add the "TgLibVers" string as a #define in tango_config.h
//
// Revision 2.4  2002/08/12 15:06:53  taurel
// Several big fixes and changes
//   - Remove HP-UX specific code
//   - Fix bug in polling alogorithm which cause the thread to enter an infinite
//     loop (pollthread.cpp)
//   - For bug for Win32 device when trying to set attribute config
//     (attribute.cpp)
//
// Revision 2.3  2002/07/02 15:22:24  taurel
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
// Revision 2.2  2002/04/30 10:50:40  taurel
// Don't check alarm on attribute if attribute quality factor is INVALID
//
// Revision 2.1  2002/04/29 12:24:03  taurel
// Fix bug in attribute::set_value method and on the check against min and max value when writing attributes
//
// Revision 2.0  2002/04/09 14:45:09  taurel
// See Tango WEB pages for list of changes
//
// Revision 1.6  2001/10/08 09:03:11  taurel
// See tango WEB pages for list of changes
//
// Revision 1.5  2001/07/04 12:27:09  taurel
// New methods re_throw_exception(). Read_attributes supports AllAttr mnemonic A new add_attribute()method in DeviceImpl class New way to define attribute properties New pattern to prevent full re-compile For multi-classes DS, it is now possible to use the Util::get_device_by_name() method in device constructor Adding << operator ovebloading Fix devie CORBA ref. number when device constructor sends an excep.
//
// Revision 1.4  2001/05/04 09:28:12  taurel
// Fix bugs in DServer::restart() method and in Util::get_device_by_name() method
//
// Revision 1.3  2001/03/30 08:03:44  taurel
// Fix bugs in attributes. For linux, add signal_handler in its own thread, change the way to kill server. For all system, change DevRestart philosophy.
//
// Revision 1.2  2001/03/09 08:20:14  taurel
// Fix bug in the MultiClassAttribute::init_class_attribute() method. Also remove the DbErr_DeviceNotDefined define.
//
// Revision 1.1.1.1  2001/02/27 08:46:20  taurel
// Imported sources
//
// Revision 1.3  2000/04/13 10:40:40  taurel
// Added attribute support
//
//=============================================================================

#ifndef _CLASS_ATTRIBUTE_H
#define _CLASS_ATTRIBUTE_H

#include <tango.h>
//#include <attrdesc.h>
#include <functional>

namespace Tango
{

//=============================================================================
//
//			The AttrProperty class
//
//
// description :	This class is used to store a pair of attribute name,
//			attribute value. Both name and value are stored as
//			string
//
//=============================================================================

class AttrProperty
{
public:
	AttrProperty(const char *name,const char *value);
	AttrProperty(string &name,string &value);
	AttrProperty(const char *name,long value);
	AttrProperty(const char *name,string &value);
	~AttrProperty() {};

	string &get_value() {return attr_value;}
	long get_lg_value() {return attr_lg;}
	string &get_name() {return attr_name;}
	
	void convert();

#ifndef TANGO_HAS_LOG4TANGO
	friend ostream &operator<<(ostream &,const AttrProperty &);
#endif

private:	
	string		attr_name;
	string		attr_value;
	long		attr_lg;
};


//=============================================================================
//
//			The MultiClassAttribute class
//
//
// description :	This class is a holder for all the ClassAttribute
//			instance. There is one instance of this class for each
//			Tango device pattern implementation. This instance is
//			stored in the DeviceClass object of the pattern
//
//=============================================================================


class MultiClassAttribute
{
public:
	MultiClassAttribute();
	~MultiClassAttribute();

	void init_class_attribute(string &class_name,long base = 0);
	
	vector<Tango::Attr *> &get_attr_list() {return attr_list;}
	Attr &get_attr(string &attr_name);
	void remove_attr(string &,const string &);

protected:	
	vector<Tango::Attr *>			attr_list;	
};


//=============================================================================
//
//			A binary function object
//
//
// description :	This binary function object is used by the find_if
//			std C++ find_if algorithm. It checks if the 
//			ClassAttribute object passed as argument (A1) stored
//			all the properties for the atribute name passed as
//			second argument (A2).
//			This function object is a predicate and therefore
//			returns a boolean (R)
//
//=============================================================================

template <class A1, class A2, class R>
struct WantedClassAttr : public  binary_function<A1,A2,R>
{
	R operator() (A1 att,A2 name_str) const
	{
		return att->get_name() == name_str;
	}
};

} // End of Tango namespace

#endif // _CLASS_ATTRIBUTE_H

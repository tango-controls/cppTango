//=============================================================================
//
// file :               doc.h
//
// description :        Just a file some some documentation to be used by
//                      doxygen
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2012
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
// $Revision: 1 $
//
//=============================================================================

#ifndef _DOC_DOXYGEN_H
#define _DOC_DOXYGEN_H

/**
\file   doc.h
\brief Just a file to write some Doxygen specific documentation
*/


/**@typedef DbData
 * A vector of DbDatum class
 */
typedef vector<DbDatum> DbData;
/**@typedef DbDevInfos
 * A vector of DbDevInfo structure
 */
typedef vector<DbDevInfo> DbDevInfos;
/**@typedef DbDevExportInfos
 * A vector of DbDevExport structure
 */
typedef vector<DbDevExportInfo> DbDevExportInfos;
/**@typedef DbDevImportInfos
 * A vector of DbDevImport structure
 */
typedef vector<DbDevImportInfo> DbDevImportInfos;

/**@typedef CommandInfoList
 * A vector of CommandInfo structure
 */
typedef vector<CommandInfo> CommandInfoList;
/**@typedef AttributeInfoList
 * A vector of AttributeInfo structure
 */
typedef vector<AttributeInfo> AttributeInfoList;
/**@typedef AttributeInfoListEx
 * A vector of AttributeInfoEx structure
 */
typedef vector<AttributeInfoEx> AttributeInfoListEx;

/*******************************************************
 *                                                     *
 *               The main page                         *
 *                                                     *
 *******************************************************/


/*! \mainpage
 *
 * \section intro_sec Introduction
 *
 * This is the reference documentation for all classes provided by the Tango C++ API.
 * These classes are divied in two groups (modules) which are
 * \li \ref Client
 * \li \ref Server
 *
 * Client classes are mostly used in application dealing with Tango devices.
 * Server classes are mostly used in Tango class or device server process main function.
 *
 * In order to develop Tango's related software, it's a good idea to have  a look at the
 * so-called "Tango book" (maily chapter 4 but not only)
 * \li <a href=http://ftp.esrf.fr/pub/cs/tango/tango_80.pdf>In PDF</a>
 * \li <a href=http://www.esrf.eu/computing/cs/tango/tango_doc/kernel_doc/ds_prog/index.html target=new>In HTML</a>
 *
 * A look at the so-called "Related Pages" could also help the developper.
 *
 * Many information are also available in the <a href=http://www.tango-controls.org target=new>Tango Web site</a>
 *
 */

/*******************************************************
 *                                                     *
 *               The Exception related page            *
 *                                                     *
 *******************************************************/

/** @page except Exception
 *
 *
 * @section Ex Tango API exceptions
 * All the exception thrown by this API are Tango::DevFailed exception. This exception is a variable length
 * array of Tango::DevError type. The Tango::DevError type is a four fields structure. These fields are :
 * @li A string describing the error type. This string replaces an error code and allows a more easy management
 * of include files. This field is called @b reason
 * @li A string describing in plain text the reason of the error. This field is called @b desc
 * @li A string giving the name of the method which thrown the exception. This field is named @b origin
 * @li The error severity. This is an enumeration with three values which are WARN, ERR or PANIC. Its
 * name is @b severity
 *
 * This is a variable length array in order to transmit to the client what is the primary error reason. The
 * sequence element 0 describes the primary error. An exception class hierarchy has been implemented within
 * the API to ease API programmers task. All the exception classes inherits from the Tango::DevFailed class.
 * Except for the NamedDevFaildeList exception class, they don’t add any new fields to the exception, they
 * just allow easy "catching". Exception classes thrown only by the API layer are :
 * @li ConnectionFailed
 * @li CommunicationFailed
 * @li WrongNameSyntax
 * @li NonDbDevice
 * @li WrongData
 * @li NonSupportedFeature
 * @li AsynCall
 * @li AsynReplyNotArrived
 * @li EventSystemFailed
 * @li NamedDevFailedList
 * @li DeviceUnlocked
 *
 * On top of these classes, exception thrown by the device (Tango::DevFailed exception) are directly passed
 * to the client.
 * @subsection ConFailed The ConnectionFailed exception
 * This exception is thrown when a problem occurs during the connection establishment between the application
 * and the device. The API is stateless. This means that DeviceProxy constructors filter most of the
 * exception except for cases described in the following table.
 * <TABLE>
 * <TR>
 *    <TH ALIGN="CENTER">Method name</TH>
 *    <TH ALIGN="CENTER">device type</TH>
 *    <TH ALIGN="CENTER">error type</TH>
 *    <TH ALIGN="CENTER">Level</TH>
 *    <TH ALIGN="CENTER">reason</TH>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=8>DeviceProxy constructor</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=4>with database</TD>
 *    <TD ALIGN="CENTER">TANGO_HOST not set</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_TangoHostNotSet</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=3>Device not defined in db or Alias not defined in db</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">DB_DeviceNotDefined</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">2</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotDefined</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>with database specified in dev name</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=2>database server not running</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CantConnectToDatabase</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>without database</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Server running but device not defined in server</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotExported</TD>
 * </TR>



 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=9>AttributeProxy constructor</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=7>with database</TD>
 *    <TD ALIGN="CENTER">TANGO_HOST not set</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_TangoHostNotSet</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=3>Device not defined in db</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">DB_DeviceNotDefined</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">2</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotDefined</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=3>Alias not defined in db</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">DB_SQLError</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">2</TD>
 *    <TD ALIGN="CENTER">API_AliasNotDefined</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>with database specified in dev name</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=2>database server not running</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CantConnectToDatabase</TD>
 * </TR>



 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=7>DeviceProxy or AttributeProxy method call (except command_inout, read_attribute)</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=2>without database</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Server not running</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_ServerNotRunning</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=5>with database</TD>
 *    <TD ALIGN="CENTER">Server ot running</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotExported</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Dead server</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CantConnectToDevice</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Dead database server when reconnection needed</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CantConnectToDatabase</TD>
 * </TR>


 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=11>DeviceProxy command_inout and read_attribute or AttributeProxy read and write</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=3>without database</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=3>Server not running</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotExported</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_ServerNotRunning</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">2</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=8>with database</TD>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Server not running</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotExported</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=3>Dead server</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CantConnectToDatabase</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">2</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed or API_AttributeFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=3>Dead database server when re-connection needed</TD>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_DeviceNotExported</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CantConnectToDatabase</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">2</TD>
 *    <TD ALIGN="CENTER">API_CommandFailed</TD>
 * </TR>
 * </TABLE>
 * The desc DevError structure field allows a user to get more precise information. These informations
 * are (according to the reason field) :
 * @li @b DB_DeviceNotDefined: The name of the device not defined in the database
 * @li @b API_CommandFailed: The device and command name
 * @li @b API_CantConnectToDevice: The device name
 * @li @b API_CorbaException: The name of the CORBA exception, its reason, its locality, its completed flag and
 * its minor code
 * @li @b API_CantConnectToDatabase: The database server host and its port number
 * @li @b API_DeviceNotExported: The device name
 *
 * @subsection CommFailed The CommunicationFailed exception
 * This exception is thrown when a communication problem is detected during the communication between
 * the client application and the device server. It is a two levels Tango::DevError structure. In case of time-out,
 * the DevError structures fields are:
 * <TABLE>
 * <TR>
 *    <TH ALIGN="CENTER">Level</TH>
 *    <TH ALIGN="CENTER">Reason</TH>
 *    <TH ALIGN="CENTER">Desc</TH>
 *    <TH ALIGN="CENTER">Severity</TH>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 *    <TD ALIGN="CENTER">CORBA exception fields translated into a string</TD>
 *    <TD ALIGN="CENTER">ERR</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_DeviceTimedOut</TD>
 *    <TD ALIGN="CENTER">String with time-out value and device name</TD>
 *    <TD ALIGN="CENTER">ERR</TD>
 * </TR>
 * </TABLE>
 * For all other communication errors, the DevError structures fields are:
 * <TABLE>
 * <TR>
 *    <TH ALIGN="CENTER">Level</TH>
 *    <TH ALIGN="CENTER">Reason</TH>
 *    <TH ALIGN="CENTER">Desc</TH>
 *    <TH ALIGN="CENTER">Severity</TH>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">0</TD>
 *    <TD ALIGN="CENTER">API_CorbaException</TD>
 *    <TD ALIGN="CENTER">CORBA exception fields translated into a string</TD>
 *    <TD ALIGN="CENTER">ERR</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">1</TD>
 *    <TD ALIGN="CENTER">API_CommunicationFailed</TD>
 *    <TD ALIGN="CENTER">String with device, method, command/attribute name</TD>
 *    <TD ALIGN="CENTER">ERR</TD>
 * </TR>
 * </TABLE>
 *
 * @subsection WrongName The WrongNameSyntax exception
 * This exception has only one level of Tango::DevError structure. The possible value for the reason field are :
 * @li @b API_UnsupportedProtocol This error occurs when trying to build a DeviceProxy or an AttributeProxy
 * instance for a device with an unsupported protocol. Refer to the appendix on device naming syntax
 * to get the list of supported database modifier
 * @li @b API_UnsupportedDBaseModifier This error occurswhen trying to build a DeviceProxy or an AttributeProxy
 * instance for a device/attribute with a database modifier unsupported. Refer to the appendix on device
 * naming syntax to get the list of supported database modifier
 * @li @b API_WrongDeviceNameSyntax This error occurs for all the other error in device name syntax. It is
 * thrown by the DeviceProxy class constructor.
 * @li @b API_WrongAttributeNameSyntax This error occurs for all the other error in attribute name syntax. It is
 * thrown by the AttributeProxy class constructor.
 * @li @b API_WrongWildcardUsage This error occurs if there is a bad usage of the wildcard character
 *
 * @subsection NonDb The NonDbDevice exception
 * This exception has only one level of Tango::DevError structure. The reason field is set to API_NonDatabaseDevice.
 * This exception is thrown by the API when using the DeviceProxy or AttributeProxy class database access
 * for non-database device.
 *
 * @subsection WrongData The WrongData exception
 * This exception has only one level of Tango::DevError structure. The possible value for the reason field are :
 * @li @b API_EmptyDbDatum This error occurs when trying to extract data from an empty DbDatum object
 * @li @b API_IncompatibleArgumentType This error occurs when trying to extract data with a type different than
 * the type used to send the data
 * @li @b API_EmptyDeviceAttribute This error occurswhen trying to extract data froman empty DeviceAttribute
 * object
 * @li @b API_IncompatibleAttrArgumentType This error occurs when trying to extract attribute data with a type
 * different than the type used to send the data
 * @li @b API_EmptyDeviceData This error occurs when trying to extract data from an empty DeviceData object
 * @li @b API_IncompatibleCmdArgumentType This error occurs when trying to extract command data with a
 * type different than the type used to send the data
 *
 * @subsection NonSupported The NonSupportedFeature exception
 * This exception is thrown by the API layer when a request to a feature implemented in Tango device interface
 * release n is requested for a device implementing Tango device interface n-x. There is one possible value
 * for the reason field which is API_UnsupportedFeature.
 *
 * @subsection AsynCall The AsynCall exception
 * This exception is thrown by the API layer when a the asynchronous model id badly used. This exception
 * has only one level of Tango::DevError structure. The possible value for the reason field are :
 * @li @b API_BadAsynPollId This error occurs when using an asynchronous request identifier which is not valid
 * any more.
 * @li @b API_BadAsyn This error occurs when trying to fire callback when no callback has been previously registered
 * @li @b API_BadAsynReqType This error occurs when trying to get result of an asynchronous request with
 * an asynchronous request identifier returned by a non-coherent asynchronous request (For instance,
 * using the asynchronous request identifier returned by a command_inout_asynch() method with a
 * read_attribute_reply() attribute).
 *
 * @subsection AsynReply The AsynReplyNotArrived exception
 * This exception is thrown by the API layer when:
 * @li a request to get asynchronous reply is made and the reply is not yet arrived
 * @li a blocking wait with timeout for asynchronous reply is made and the timeout expired.
 *
 * There is one possible value for the reason field which is API_AsynReplyNotArrived.
 *
 * @subsection EventFailed The EventSystemFailed exception
 * This exception is thrown by the API layer when subscribing or unsubscribing from an event failed. This
 * exception has only one level of Tango::DevError structure. The possible value for the reason field are :
 * @li @b API_NotificationServiceFailed This error occurs when the subscribe_event() method failed trying to access
 * the CORBA notification service
 * @li @b API_EventNotFound This error occurswhen you are using an incorrect event_id in the unsubscribe_event()
 * method
 * @li @b API_InvalidArgs This error occurs when NULL pointers are passed to the subscribe or unsubscribe event
 * methods
 * @li @b API_MethodArgument This error occurs when trying to subscribe to an event which has already been
 * subscribed to
 * @li @b API_DSFailedRegisteringEvent This error means that the device server to which the device belongs to
 * failed when it tries to register the event. Most likely, it means that there is no event property defined
 * @li @b API_EventNotFound Occurs when using a wrong event identifier in the unsubscribe_event method
 *
 *
 * @subsection NamedDevFailed The NamedDevFailedList exception
 * This exception is only thrown by the DeviceProxy::write_attributes() method. In this case, it is necessary
 * to have a new class of exception to transfer the error stack for several attribute(s) which failed during the
 * writing. Therefore, this exception class contains for each attributes which failed :
 * @li The name of the attribute
 * @li Its index in the vector passed as argumen tof the write_attributes() method
 * @li The error stack
 *
 * The following piece of code is an example of how to use this class exception
 * @code
 * catch (Tango::NamedDevFailed &e)
 * {
 *    long nb_faulty = e.get_faulty_attr_nb();
 *    for (long i = 0;i < nb_faulty;i++)
 *    {
 *       cout << "Attribute " << e.err_list[i].name << " failed!" << endl;
 *       for (long j = 0;j < e.err_list[i].err_stack.length();j++)
 *       {
 *          cout << "Reason [" << j << "] = " << e.err_list[i].err_stack[j].reason;
 *          cout << "Desc [" << j << "] = " << e.err_list[i].err_stack[j].desc;
 *       }
 *    }
 * }
 * @endcode
 * This exception inherits from Tango::DevFailed. It is possible to catch it with a "catch DevFailed" catch
 * block. In this case, like any other DevFailed exception, there is only one error stack. This stack is initialised
 * with the name of all the attributes which failed in its "reason" field.
 *
 * @subsection DevUnlock The DeviceUnlocked exception
 * This exception is thrown by the API layer when a device locked by the process has been unlocked by an
 * admin client. This exception has two levels of Tango::DevError structure. There is only possible value for
 * the reason field which is
 * @li @b API_DeviceUnlocked The device has been unlocked by another client (administration client)
 *
 * The first level is the message reported by the Tango kernel from the server side. The second layer is added
 * by the client API layer with informations on which API call generates the exception and device name.
 */

/*******************************************************
 *                                                     *
 *               The Reconnection related page         *
 *                                                     *
 *******************************************************/


/** @page recon Reconnection and exception
 * @section Tango API reconnection
 * The Tango API automatically manages re-connection between client and server in case of communication
 * error during a network access between a client and a server. The transparency reconnection mode allows
 * a user to be (or not be) informed that automatic reconnection took place. If the transparency reconnection
 * mode is not set, when a communication error occurs, an exception is returned to the caller and the connection
 * is internally marked as bad. On the next try to contact the device, the API will try to re-build the
 * network connection. If the transparency reconnection mode is set (the default case), the API will try to re-build the network
 * connection has soon as the communication error occurs and the caller is not informed. Several cases are
 * possible. They are summarized in the following table:
 * <TABLE>
 * <TR>
 *    <TH ALIGN="CENTER">Case</TH>
 *    <TH ALIGN="CENTER">Server state</TH>
 *    <TH ALIGN="CENTER">Call nb</TH>
 *    <TH ALIGN="CENTER">Exception (transparency false)</TH>
 *    <TH ALIGN="CENTER">Exception (transparency true)</TH>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=4>Server killed and re-started</TD>
 *    <TD ALIGN="CENTER">Server killed before call n</TD>
 *    <TD ALIGN="CENTER">n</TD>
 *    <TD ALIGN="CENTER">CommunicationFailed</TD>
 *    <TD ALIGN="CENTER">ConnectionFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Down</TD>
 *    <TD ALIGN="CENTER">n + 1</TD>
 *    <TD ALIGN="CENTER">ConnectionFailed (2 levels)</TD>
 *    <TD ALIGN="CENTER">idem</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Down</TD>
 *    <TD ALIGN="CENTER">n + 2</TD>
 *    <TD ALIGN="CENTER">idem</TD>
 *    <TD ALIGN="CENTER">idem</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Running</TD>
 *    <TD ALIGN="CENTER">n + x</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 * </TR>

 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=4>Server died and re-started</TD>
 *    <TD ALIGN="CENTER">Server died before call n</TD>
 *    <TD ALIGN="CENTER">n</TD>
 *    <TD ALIGN="CENTER">CommunicationFailed</TD>
 *    <TD ALIGN="CENTER">ConnectionFailed</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Died</TD>
 *    <TD ALIGN="CENTER">n + 1</TD>
 *    <TD ALIGN="CENTER">ConnectionFailed (3 levels)</TD>
 *    <TD ALIGN="CENTER">idem</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Died</TD>
 *    <TD ALIGN="CENTER">n + 2</TD>
 *    <TD ALIGN="CENTER">idem</TD>
 *    <TD ALIGN="CENTER">idem</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Running</TD>
 *    <TD ALIGN="CENTER">n + x</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 * </TR>

 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Server killed and re-started</TD>
 *    <TD ALIGN="CENTER">Server killed and re-started before call n</TD>
 *    <TD ALIGN="CENTER">n</TD>
 *    <TD ALIGN="CENTER">CommunicationFailed</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Running</TD>
 *    <TD ALIGN="CENTER">n + x</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 * </TR>

 * <TR>
 *    <TD ALIGN="CENTER" ROWSPAN=2>Server died and re-started</TD>
 *    <TD ALIGN="CENTER">Server died and re-started before call n</TD>
 *    <TD ALIGN="CENTER">n</TD>
 *    <TD ALIGN="CENTER">CommunicationFailed</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 * </TR>
 * <TR>
 *    <TD ALIGN="CENTER">Running</TD>
 *    <TD ALIGN="CENTER">n + x</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 *    <TD ALIGN="CENTER">No exception</TD>
 * </TR>
 * </TABLE>

 * Please note that the timeout case is managed differently because it will not enter the re-connection
 * system. The transparency reconnection mode is set by default to true starting with Tango version 5.5.
 */


#endif /* DOC_DOXYGEN */

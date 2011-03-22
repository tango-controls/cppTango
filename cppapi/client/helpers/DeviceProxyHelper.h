//+=============================================================================
//
// project :      TANGO Utility 
// file :         DeviceProxyHelper.h: 
// $Revision$
//
//
//   description :  This utility class helps to use command_inout,read_attributes and write_attribute(s)
//                on a DeviceProxy , from a Tango C++ Client.
//                Its role is :
//                  - to provide syntactic sugar to avoid the heavy manipulation of DeviceData objects
//                            used to exchange data between a C++ client and a server 
//                  - to handle exception in an uniform way using the TangoExceptionHelper.h file
//
// Usage examples : 
// 
//    Step 1)  create a DeviceProxyHelper object (in your client init_device for instance)
//    
//    tangoCA = new DeviceProxyHelper("lucia/sls/tangoca",this);
//      ==>  A DeviceProxy is internally created by DeviceProxyHelper ( note : the this pointer is used for logging purpose)
//
//    Step 2)  USE your remote Device !!
//
//     FOR COMMANDS (Note that  DEVVAR<X>STRINGARRAY argin or argout types are not supported)

//          There are 4 methods:
//            -command (no argin,no argout) 
//            -command_in (only argin) 
//            -command_out (only argout) 
//            -command_inout (argin and argout) 

//  Note :  the methods are Templates so you don't care about type (while they are supported by DeviceData!!!)
//         
//  COMMAND EXAMPLES :
//                      a) execute Command Reset on the deviceProxy : No argin nor argout required
//                      tangoCA->command("Reset");   
//
//                      b) execute Command GotoPosition on the deviceProxy with argin= 12.3
//                      tangoCA->command_in("GotoPosition", 12.3);   
//
//                      c) execute Command ReadPosition on the deviceProxy : argout will be put in read_value
//                      tangoCA->command_out("ReadPosition", read_value);  
// 
//                      d) execute Command ReadDoublePv on the deviceProxy : verticalGapPV.c_str() is the argin and *attr_VerticalGap_read is the argout.
//                      tangoCA->command_inout("ReadDoublePV",verticalGapPV.c_str(),
//                                            *attr_VerticalGap_read)
//                          ==> Automatically  the CommandInOutHelper has done all the DeviceData encapsulation  
//                             and transfers the result of the command_inout in your argout
// 
// 
//        Handling of composite type like DEVVAR<X>STRINGARRAY in argin and/or argout (with <X> = LONG or DOUBLE):
//           The <X> part and the STRING part must be seperated by user in two vectors, std::vector<X> and std::vector<std::string>.
//           The following must be called:
//
//        - command with argin only:
//               void command_in (
//                      const std::string& cmd_name, 
//                      const std::vector<_IN>& _nv_in,           // numerical part of the input DEVVAR<X>STRINGARRAY
//                      const std::vector<std::string>& _sv_in    // string part of the input DEVVAR<X>STRINGARRAY
//                    )
//
//        - command with argout only:
//              void command_out (
//                       const std::string& cmd_name, 
//                       std::vector<_OUT>& _nv_out,              // numerical part of the ouput DEVVAR<X>STRINGARRAY
//                       std::vector<std::string>& _sv_out        // string part of the ouput DEVVAR<X>STRINGARRAY
//                     )
//
//        - command with argin and argout: 
//              void command_inout (
//                        const std::string& cmd_name, 
//                        const std::vector<_IN>& _nv_in,         // numerical part of the input DEVVAR<X>STRINGARRAY
//                        const std::vector<std::string>& _sv_in, // string part of the input DEVVAR<X>STRINGARRAY
//                        std::vector<_OUT>& _nv_out,             // numerical part of the ouput DEVVAR<X>STRINGARRAY
//                        std::vector<std::string>& _sv_out       // string part of the ouput DEVVAR<X>STRINGARRAY
//                       )  
//          
//
//  ATTRIBUTES EXAMPLES :
//                      a) execute  read_attribute  on the deviceProxy and store value in omega_on_axis variable
//                             _omegaProxy_helper		->read_attribute("AxisCurrentPosition" , omega_on_axis);
//
//                      b) execute  write_attribute  on the deviceProxy and with argin = 12.3
//				                    _omegaProxy_helper		->	write_attribute("AxisCurrentPosition", 12.3);

//

//
// $Author$
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
//						Synchrotron SOLEIL
//                		L'Orme des Merisiers
//                		Saint-Aubin - BP 48 - France
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
// Revision 1.8  2010/09/21 12:08:44  taurel
// - Added GPL header
//
// Revision 1.7  2008/03/31 07:22:46  jensmeyer
// Added helper classes to Tango library.
//
//
// Revision 1.6  2008/02/13 13:20:30  langlois
// - merge from linux version
//
// Revision 1.1  2006/03/16 14:03:18  dupuy
// soleil distrib 2
//
// Revision 1.2  2006/03/14 14:58:42  hardion
// * Update DeviceProxyHelper with Florent modification ( read the write part of an attribute)
//
// Revision 1.3  2005/04/27 10:22:28  buteau
// - Under linux , for all methods of DeviceProxyHelper (i.e read_attribute, command_in, command_out , etc .. ) , the filename and the line number of client Device appears in Exceptions (instead of line number and filenmae in DeviceProxyHelper.h)
// - under WIN32, VC++6 : same feature but only on read_attribute, write_attribute, command methods of DeviceProxyHelper
// - minor fixes following  E. Taurel remarks
//
// Revision 1.2  2004/08/06 09:20:45  langlois
// - Added a getter for the DeviceProxy
//
// Revision 1.1.1.1  2004/07/13 15:39:53  root
// inital import
//
// Revision 1.0  10/07/2003 langlois
// - Changed the only one command_inout overloaded 4 times by four differents methods:
//  -command (no argin,no argout) 
//  -command_inout (argin and argout) 
//  -command_in (only argin) 
//  -command_out (only argout)
//
// Revision 0.0  30/06/2003 langlois
// - First version: 1 template methods command_inout overloaded 4 times 
// 
//

#ifndef _DEVICE_PROXY_HELPER_H_
#define _DEVICE_PROXY_HELPER_H_

//=============================================================================
// DEPENDENCIES
//=============================================================================
#include <TangoExceptionsHelper.h>

//=============================================================================
// MACRO
//=============================================================================
#define FULL_CMD_NAME(C) device_proxy_->dev_name() + "::" + C
#define FULL_ATTR_NAME(A) device_proxy_->dev_name() + "/" + A
//
//	Definition of macros to benefit from expansion of __FILE__ and __LINE__ keywords in Device caller source file
//
#define read_attribute(ATTR_NAME, VALUE)	 internal_read_attribute	(ATTR_NAME, VALUE, __FILE__, __LINE__)
#define read_attribute_w(ATTR_NAME, VALUE)	 internal_read_attribute_w  (ATTR_NAME, VALUE, __FILE__, __LINE__)
#define write_attribute(ATTR_NAME, VALUE)	 internal_write_attribute	(ATTR_NAME, VALUE, __FILE__, __LINE__)
#define command(CMD_NAME)					 internal_command			(CMD_NAME,  __FILE__, __LINE__)

// For VC++ 6 VA_ARG macro is not supported so ==>, we cannot override command_out functions
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_out internal_command_out
#else	// For compilers that support variable number of arguments
#define command_out(CMD_NAME, OUT, ...) internal_command_out (CMD_NAME, OUT, ## __VA_ARGS__, __FILE__, __LINE__ )
#endif

// For VC++ 6 VA_ARG macro is not supported so ==>, we cannot override command_in functions
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_in internal_command_in
#else	// For compilers that support variable number of arguments
#define command_in(CMD_NAME, IN, ...) internal_command_in (CMD_NAME, IN, ## __VA_ARGS__, __FILE__, __LINE__ )
#endif

// For VC++ 6 VA_ARG macro is not supported so ==>, we cannot override command_inout functions
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#else	// For compilers that support variable number of arguments
#define command_inout(CMD_NAME,  ...) internal_command_inout (CMD_NAME,  ## __VA_ARGS__, __FILE__, __LINE__ )
#endif

namespace Tango 
{
	
	//=============================================================================
	// CLASS: HelperBase
	//=============================================================================
	class HelperBase : public Tango::LogAdapter
	{
	public:
		//---------------------------------------------------------------------------
		//  HelperBase::get_device_proxy
		//  returns the underlying device
		//---------------------------------------------------------------------------
		inline Tango::DeviceProxy* get_device_proxy (void)
		{
			return device_proxy_;
		}
		//---------------------------------------------------------------------------
		//  HelperBase::operator->
		//  returns the underlying device
		//---------------------------------------------------------------------------
		inline Tango::DeviceProxy* operator-> (void)
		{
			return device_proxy_;
		}
		
	protected:
		//---------------------------------------------------------------------------
		//  HelperBase::HelperBase  (ctor)
		//  device_name : The name of the target device.
		//  client_device : Reference to the client device (for logging purpose).
		//---------------------------------------------------------------------------
		HelperBase (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
			throw (Tango::DevFailed)
			: Tango::LogAdapter(client_device), device_proxy_(0)
		{
			_DEV_TRY_REACTION
				(
				//- try
				device_proxy_ = new Tango::DeviceProxy(const_cast<std::string&>(device_name)),
				//- what do we tried
				std::string("new Tango::DeviceProxy on ") + device_name,
				//- origin
				"HelperBase::HelperBase",
				//- reaction
				if (device_proxy_) {delete device_proxy_; device_proxy_ = 0;}
				);
		}
		//---------------------------------------------------------------------------
		// HelperBase::~HelperBase (dtor)
		//---------------------------------------------------------------------------
		virtual ~HelperBase ()
		{
			if (device_proxy_)
			{
				delete device_proxy_;
				device_proxy_ = 0;
			}
		}
		//---------------------------------------------------------------------------
		//- the underlying device
		//---------------------------------------------------------------------------
		Tango::DeviceProxy* device_proxy_;
	};
	
	//=============================================================================
	// CLASS: CommandInOutHelper
	//=============================================================================
	class CommandInOutHelper : public virtual HelperBase
	{
	public:
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::CommandInOutHelper  (ctor)
		//  device_name : The name of the target device.
		//  client_device : Reference to the client device (for logging purpose).
		//---------------------------------------------------------------------------
		CommandInOutHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
			: HelperBase(device_name, client_device)
		{
			dd_out_.exceptions(Tango::DeviceData::isempty_flag | 
				Tango::DeviceData::wrongtype_flag);
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::~CommandInOutHelper  (dtor)
		//---------------------------------------------------------------------------
		virtual ~CommandInOutHelper ()
		{
			//- noop dtor
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command
		//  exec a DEV_VOID/DEV_VOID TANGO command on the underlying device
		//  cmd_name : The name of the TANGO command
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif	
		void internal_command (const std::string& cmd_name,
			std::string file, 
			int line)
			throw (Tango::DevFailed)
		{
			if (device_proxy_)
			{
				_DEV_TRY_FILE_LINE
					(
					//- try
					
					(device_proxy_->command_inout)(const_cast<std::string&>(cmd_name)),
					
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command",
					file,
					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_inout
		//  exec a ARG_OUT/ARG_IN TANGO command on the underlying device
		//  cmd_name : the name of the TANGO command
		//  argin : input argument
		//  argout : output argument
		
		
		//---------------------------------------------------------------------------
		//  template arg _IN must be supported by DeviceData::operator<<
		//  template arg _OUT must be supported by DeviceData::operator>>
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		
		template <class _IN, class _OUT>
			void internal_command_inout (const std::string& cmd_name, const _IN& argin, _OUT& argout, std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
			if (device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in << const_cast<_IN&>(argin);
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_inout",
					file,
					line
					);
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_ >> argout,
					//- what do we tried
					std::string("DeviceData::operator>> on data returned by ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_inout",
					file,
					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif		
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_inout
		//  exec a DEVVAR<X>STRINGARRAY/DEVVAR<X>STRINGARRAY command on the underlying device
		//  cmd_name : the name of the TANGO command
		//  _nv_in : numerical part of the input DEVVAR<X>STRINGARRAY
		//  _sv_in : string part of the input DEVVAR<X>STRINGARRAY
		//  _nv_out : numerical part of the ouput DEVVAR<X>STRINGARRAY
		//  _sv_out : string part of the ouput DEVVAR<X>STRINGARRAY
		//---------------------------------------------------------------------------
		//  template arg _IN must be supported by DeviceData::.insert
		//  template arg _OUT must be supported by DeviceData::.insert
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _IN, class _OUT>
			void internal_command_inout (const std::string& cmd_name, 
			const std::vector<_IN>& _nv_in, 
			const std::vector<std::string>& _sv_in,
			std::vector<_OUT>& _nv_out,
			std::vector<std::string>& _sv_out, 
			std::string file= __FILE__,
			int line= __LINE__)
			throw (Tango::DevFailed)
		{
			
			if (device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in.insert(const_cast<std::vector<_IN>&>(_nv_in), 
					const_cast<std::vector<std::string>&>(_sv_in));
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_inout",
					file,
					line
					);
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_.extract(_nv_out, _sv_out),
					//- what do we tried
					std::string("DeviceData::extract on data returned by ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_inout",
					file,
					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_inout
		//  Overloaded commands to  avoid usage of DevVarXX ARRAY for argout 
		//---------------------------------------------------------------------------
		template <class _IN>
			void internal_command_inout (const std::string& cmd_name, const _IN& argin, DevVarDoubleStringArray* argout,std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
			
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_inout
		//  Overloaded commands to  avoid usage of DevVarXX ARRAY for argout 
		//---------------------------------------------------------------------------
		template <class _IN>
			void internal_command_inout (const std::string& cmd_name, const _IN& argin, DevVarLongStringArray* argout,std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_inout
		//  Overloaded commands to  avoid usage of DevVarXX ARRAY for argout 
		//---------------------------------------------------------------------------
		template <class _IN>
			void internal_command_inout (const std::string& cmd_name, const _IN& argin, DevVarDoubleStringArray& argout,std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
		}
		
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_inout
		//  Overloaded commands to  avoid usage of DevVarXX ARRAY for argout 
		//---------------------------------------------------------------------------
		template <class _IN>
			void internal_command_inout (const std::string& cmd_name, const _IN& argin, DevVarLongStringArray& argout,std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_inout:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
		}
		
		
		
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_in
		//  exec a DEV_VOID/ARG_IN TANGO command on the underlying device
		//  cmd_name : the name of the TANGO command
		//  argin : input argument
		// dummy : used to have same number of parameter for the 2 command_in methods
		//---------------------------------------------------------------------------
		//  template arg _IN must be supported by DeviceData::operator<<
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _IN>
			void internal_command_in (const std::string& cmd_name, const _IN& argin,  std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
			if (device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in << const_cast<_IN&>(argin);
				_DEV_TRY_FILE_LINE
					(
					//- try
					(device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_in",
					file,
					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_in
		//  exec a DEV_VOID/DEVVAR<X>STRINGARRAY command on the underlying device
		//  cmd_name : the name of the TANGO command
		//  _nv_in   : numerical part of the input DEVVAR<X>STRINGARRAY
		//  _sv_in   : string part of the input DEVVAR<X>STRINGARRAY
		//---------------------------------------------------------------------------
		//  template arg _IN must be supported by DeviceData::.insert
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _IN>
			void internal_command_in (const std::string& cmd_name, 
			const std::vector<_IN>& _nv_in, 
			const std::vector<std::string>& _sv_in, std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
			if (device_proxy_)
			{
				Tango::DeviceData dd_in;
				dd_in.insert(const_cast<std::vector<_IN>&>(_nv_in), 
					const_cast<std::vector<std::string>&>(_sv_in));
				_DEV_TRY_FILE_LINE
					(
					//- try
					(device_proxy_->command_inout)(const_cast<std::string&>(cmd_name), dd_in),
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_in",
					file,
					line
					);
			}
		}
		
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_out
		//  exec a ARG_OUT/DEV_VOID TANGO command on the underlying device
		//  cmd_name : the name of the TANGO command
		//  argout : output argument 
		//---------------------------------------------------------------------------
		//  template arg _OUT must be supported by DeviceData::operator>>
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _OUT>
			void internal_command_out (const std::string& cmd_name, _OUT& argout,  std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
			
			if (device_proxy_)
			{
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name)),
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_out",
					file,
					line
					);
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_ >> argout,
					//- what do we tried
					std::string("DeviceData::operator>> on data returned by ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_out",
					file,
					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_out
		//  Overloaded commands to  avoid usage of DevVarDoubleStringArray ARRAY
		//---------------------------------------------------------------------------
		template <class _OUT>  
			void internal_command_out(_OUT dummy, DevVarDoubleStringArray* argout,  std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
			
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_out
		//  Overloaded commands to  avoid usage of DevVarLongStringArray ARRAY
		//---------------------------------------------------------------------------
		template <class _OUT>    
			void internal_command_out (_OUT dummy, DevVarLongStringArray* argout,  std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_out
		//  Overloaded commands to  avoid usage of DevVarDoubleStringArray ARRAY
		//---------------------------------------------------------------------------
		template <class _OUT> 
			void internal_command_out(_OUT dummy, DevVarDoubleStringArray& argout,  std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarDoubleStringArray *****")
#endif
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_out
		//  Overloaded commands to  avoid usage of DevVarLongStringArray ARRAY
		//---------------------------------------------------------------------------
		template <class _OUT> 
			void internal_command_out (_OUT dummy, DevVarLongStringArray& argout,  std::string file= __FILE__, int line= __LINE__)
			throw (Tango::DevFailed)
		{
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#pragma message  (" TANGO WARNING ***** command_out:Use only STL vector instead of DevVarLongStringArray *****")
#endif
			
			TangoSys_OMemStream o; 
			o << " [" << file << "::" << line << "]" << std::ends; 
			Tango::Except::throw_exception(static_cast<const char*>("TANGO_WRONG_DATA_ERROR"), 
				static_cast<const char*>("Use only STL vector instead of DevVarXXStringArray"), 
				static_cast<const char*>(o.str().c_str())); 
		}
		//---------------------------------------------------------------------------
		//  CommandInOutHelper::command_in
		//  exec a DEV_VOID/DEVVAR<X>STRINGARRAY command on the underlying device
		//  cmd_name : the name of the TANGO command
		//  _nv_out : numerical part of the output DEVVAR<X>STRINGARRAY
		//  _sv_out : string part of the output DEVVAR<X>STRINGARRAY
		//---------------------------------------------------------------------------
		//  template arg _OUT must be supported by DeviceData::.extract
		//---------------------------------------------------------------------------
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#undef command_inout	// Must avoid macro expansion 
#endif
		template <class _OUT>
			void internal_command_out (const std::string& cmd_name, 
			std::vector<_OUT>& _nv_out, 
			std::vector<std::string>& _sv_out,
			std::string file= __FILE__, 
			int line= __LINE__)
			throw (Tango::DevFailed)
		{
			if (device_proxy_)
			{
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_ = (device_proxy_->command_inout)(const_cast<std::string&>(cmd_name)),
					//- what do we tried
					std::string("command_inout on ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_out",
					file,
					line
					);
				_DEV_TRY_FILE_LINE
					(
					//- try
					dd_out_.extract(_nv_out, _sv_out),
					//- what do we tried
					std::string("DeviceData::extract on data returned by ") + FULL_CMD_NAME(cmd_name),
					//- origin
					"CommandInOutHelper::command_out" ,
					file,
					line
					);
			}
		}
#if (defined(_MSC_VER) && _MSC_VER < 1300)
#define command_inout internal_command_inout
#endif
		
private:
	//- placed here as a workaround due to CORBA::any_var limitations
	Tango::DeviceData dd_out_;
};


//=============================================================================
// CLASS: AttributeHelper
//=============================================================================
class AttributeHelper : public virtual HelperBase
{
public:
	//---------------------------------------------------------------------------
	//  AttributeHelper::AttributeHelper  (ctor)
	//  device_name : name of the target device
	//  client_device : reference to the client device (for logging purpose)
	//---------------------------------------------------------------------------
	AttributeHelper (const std::string& device_name, Tango::DeviceImpl *client_device = 0)
		: HelperBase(device_name, client_device)
	{
		da_out_.exceptions(Tango::DeviceAttribute::isempty_flag | Tango::DeviceAttribute::wrongtype_flag);
	}
	//---------------------------------------------------------------------------
	//  AttributeHelper::~AttributeHelper (dtor)
	//---------------------------------------------------------------------------
	virtual ~AttributeHelper ()
	{
		//- noop dtor
	}

	//---------------------------------------------------------------------------
	//  AttributeHelper::get_device_attribute
	//---------------------------------------------------------------------------
	Tango::DeviceAttribute get_device_attribute () 
	{
		return da_out_;
	}

	//---------------------------------------------------------------------------
	//  AttributeHelper::write_attribute
	//  writes the specified attribute
	//  attr_name : name of the TANGO attribute to be written
	//  attr_value : the attribute value
	//---------------------------------------------------------------------------
	//  template arg _VAL must be supported by DeviceAttribute::operator<<
	//---------------------------------------------------------------------------
	template <class _VAL>
		void internal_write_attribute (const std::string& attr_name, const _VAL& attr_value, std::string file, int line) 
		throw (Tango::DevFailed)
	{
		if (device_proxy_)
		{
			DeviceAttribute da;
			da.set_name(const_cast<std::string&>(attr_name));
			da << const_cast<_VAL&>(attr_value);
			_DEV_TRY_FILE_LINE
				(
				//- try
				(device_proxy_->write_attribute)(da),
				//- what do we tried
				std::string("write_attribute on ") + FULL_ATTR_NAME(attr_name),
				//- origin
				"AttributeHelper::write_attribute",
				file,
				line
				);
		}
	}
	//---------------------------------------------------------------------------
	//  AttributeHelper::read_attribute
	//  reads the specified attribute
	//  attr_name : the name of the TANGO attribute to be read
	//  attr_value : the attribute value
	//---------------------------------------------------------------------------
	//  template arg _VAL must be supported by DeviceAttribute::operator>>
	//---------------------------------------------------------------------------
	template <class _VAL>
		void internal_read_attribute (const std::string& attr_name, _VAL& attr_value, std::string file, int line ) 
		throw (Tango::DevFailed)
	{
		
		if (device_proxy_)
		{
			_DEV_TRY_FILE_LINE
				(
				//- try
				da_out_ = (device_proxy_->read_attribute)(const_cast<std::string&>(attr_name)),
				//- what do we tried
				std::string("read_attribute on ") + FULL_ATTR_NAME(attr_name),
				//- origin
				"AttributeHelper::read_attribute", 
				file,
				line
				);
			_DEV_TRY_FILE_LINE
				(
				//- try
				da_out_ >> attr_value,
				//- what do we tried
				std::string("DeviceAttribute::operator>> on data returned by ") + FULL_ATTR_NAME(attr_name),
				//- origin
				"AttributeHelper::read_attribute", 
				file,
				line
				);
			
		}
	}

	

	//---------------------------------------------------------------------------
	//  AttributeHelper::read_attribute_w
	//  reads the specified attribute and get its write value
	//  attr_name : the name of the TANGO attribute to be read
	//  w_attr_value : the write attribute value
	//---------------------------------------------------------------------------
	//  template arg _VAL must be supported by DeviceAttribute::operator>>
	//---------------------------------------------------------------------------
	template <class _VAL>
		void internal_read_attribute_w (const std::string& attr_name, _VAL& w_attr_value, std::string file, int line ) 
		throw (Tango::DevFailed)
	{
		
		if (device_proxy_)
		{
			_DEV_TRY_FILE_LINE
				(
				//- try
				da_out_ = (device_proxy_->read_attribute)(const_cast<std::string&>(attr_name)),
				//- what do we tried
				std::string("read_attribute on ") + FULL_ATTR_NAME(attr_name),
				//- origin
				"AttributeHelper::read_attribute_w", 
				file,
				line
				);

			//===========================================================================
			//==
			//==	TEST BY FL
			//==
			//===========================================================================
			
			//- Switch on attribute type
			try
			{
				switch(da_out_.get_type())
				{
				case Tango::DEV_BOOLEAN:
					{
						w_attr_value = da_out_.BooleanSeq[1];
						break;
					}
				case Tango::DEV_SHORT:
					{
						w_attr_value = da_out_.ShortSeq[1];
						break;
					}
				case Tango::DEV_LONG:
					{
						w_attr_value = da_out_.LongSeq[1];
						break;
					}
				case Tango::DEV_FLOAT:
					{
						w_attr_value = da_out_.FloatSeq[1];
						break;
					}
				case Tango::DEV_DOUBLE:
					{
						w_attr_value = da_out_.DoubleSeq[1];
						break;
					}
				default:
					{
						Tango::Except::throw_exception(
							static_cast<const char*>("TANGO_NON_SUPPORTED_FEATURE_ERROR"),
							static_cast<const char*>("This type is not supported"),
							static_cast<const char*>("AttributeHelper::read_attribute_w"));
						break;
					}	
				}
				//INFO_STREAM << "w_attr_value = " << w_attr_value << ENDLOG;
			}

			_HANDLE_DEV_EXCEPTION("get_type()","AttributeHelper::read_attribute_w");


			//===========================================================================
			//==
			//==	END TEST BY FL
			//==
			//===========================================================================
			
		}
	}
	
private:
	//- placed here as a workaround due to CORBA::any_var limitations
	Tango::DeviceAttribute da_out_;
};

//=============================================================================
// CLASS: DeviceProxyHelper
//=============================================================================
class DeviceProxyHelper : public CommandInOutHelper, public AttributeHelper
{
public:
	//---------------------------------------------------------------------------
	//  DeviceProxyHelper::DeviceProxyHelper  (ctor)
	//  device_name : name of the target device
	//  client_device : reference to the client device (for logging purpose)
	//---------------------------------------------------------------------------
	DeviceProxyHelper (const std::string& device_name, DeviceImpl *client_device = 0)
		: HelperBase(device_name, client_device),
		CommandInOutHelper(device_name, client_device),
		AttributeHelper(device_name, client_device)
	{
		//- noop ctor
	}
	//---------------------------------------------------------------------------
	//  DeviceProxyHelper::~DeviceProxyHelper (dtor)
	//---------------------------------------------------------------------------
	virtual ~DeviceProxyHelper ()
	{
		//- noop dtor
	}
};

} //- namespace tango

#endif // _DEVICE_PROXY_HELPER_H_

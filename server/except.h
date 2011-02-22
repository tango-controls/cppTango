//=============================================================================
//
// file :               except.h
//
// description :        Include for exception related utilities   
//
// project :            TANGO
//
// author(s) :          A.Gotz + E.Taurel
//
// $Revision$
//
// $Log$
// Revision 1.3  2000/04/13 10:40:43  taurel
// Added attribute support
//
// Revision 1.2  2000/02/04 11:00:17  taurel
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

#ifndef _EXCEPT_H
#define _EXCEPT_H

#include <OB/CORBA.h>
#include <string>
#include <idl/tango.h>

#ifdef __hpux
namespace std {}
#endif

using namespace std;

namespace Tango
{

//=============================================================================
//
//			The Except class
//
// description :	This class is a container for all exceptions related
//			methods to be used in aa Tango device server/client.
//			Most of these methods are static.
//
//=============================================================================

/**
 * Container class for all exception related methods. Most of these methods are
 * static methods
 *
 * @author	$Author$
 * @version	$Revision$
 */
 
class Except
{
public:

/**@name Exception related method */
//@{	
/**
 * Print a TANGO exception.
 *
 * Print all the details of a TANGO exception.
 *
 * @param ex The exception object reference
 */	
	static void print_exception(const CORBA::Exception &ex);

//@}

/**@name Throw exception inline methods (static) */
//@{
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void throw_exception(const char *reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].desc = CORBA::string_dup(desc);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].desc = CORBA::string_dup(desc);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].reason = CORBA::string_dup(reason);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void throw_exception(const char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
			
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and desc parameters will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin, reason and desc parameters will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
		delete[] desc;
	
		throw Tango::DevFailed(errors);
	}
			
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(string &reason,
					   string &desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(string &reason,
					   string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(string &reason,
					   const char *desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(desc);
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 		
	static inline void throw_exception(string &reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc);
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(const char *reason,
					   string &desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(const char *reason,
					   string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin);
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(const char *reason,
					   const char *desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(desc);
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	
	static inline void throw_exception(string &reason,
					   string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(string &reason,
					   char *desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(desc);
		delete[] desc;
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 		
	static inline void throw_exception(string &reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason.c_str());
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc);
		delete desc;
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(char *reason,
					   string &desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 	
	static inline void throw_exception(char *reason,
					   string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(origin);
		delete[] origin;
		errors[0].desc = CORBA::string_dup(desc.c_str());
	
		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the reason and desc parameter will be freed by this method
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
 
	static inline void throw_exception(char *reason,
					   char *desc,
					   string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);
	
		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		delete[] reason;
		errors[0].origin = CORBA::string_dup(desc);
		delete[] desc;
		errors[0].desc = CORBA::string_dup(origin.c_str());
	
		throw Tango::DevFailed(errors);
	}
			
//@}

/**@name Other throw exception methods */
//@{	
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is 
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	static void throw_exception(const CORBA::SystemException &ex,const char *origin);

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is 
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field. The memory
 * allocated for this parameter will be freed by this method.
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	static void throw_exception(const CORBA::SystemException &ex,char *origin);

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is 
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field. The memory
 * allocated for this parameter will be freed by this method.
 * @exception DevFailed The thrown exception.
 * Click <a href="../../tango_basic/idl_html/Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */	
	static void throw_exception(const CORBA::SystemException &ex,string &origin);
		
//@}

private:
	static char *print_CORBA_SystemException(const CORBA::SystemException *);

};

} // End of Tango namespace
		     
#endif /* EXCEPT */

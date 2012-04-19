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

#ifndef _EXCEPT_H
#define _EXCEPT_H

#include <string>

#include <idl/tango.h>

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

#define NOSPACEINDOC_EXCEPT

/**
 * Container class for all exception related methods. Most of these methods are
 * static methods
 *
 * $Author$
 * $Revision$
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

/**@name Error stack related method */
//@{
/**
 * Print a TANGO error stack.
 *
 * Print all the details of a TANGO error stack.
 *
 * @param ex The error stack reference
 */
	static void print_error_stack(const Tango::DevErrorList &ex);

//@}

/**@name Throw exception inline methods (static) */
//@{
/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
					   const string &desc,
					   const string &origin,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
					   const string &desc,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
					   const char *desc,
					   const string &origin,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const char *reason,
					   const string &desc,
					   const string &origin,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const char *reason,
					   const string &desc,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const char *reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(1);

		errors.length(1);
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup(reason);
		errors[0].origin = CORBA::string_dup(origin.c_str());
		errors[0].desc = CORBA::string_dup(desc);

		throw Tango::DevFailed(errors);
	}

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A default value "Tango::ERR" is defined for the DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
					   const string &desc,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
					   char *desc,
					   const string &origin,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(const string &reason,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(char *reason,
					   const string &desc,
					   const string &origin,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(char *reason,
					   const string &desc,
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void throw_exception(char *reason,
					   char *desc,
					   const string &origin,
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

/**@name Re-throw exception inline methods (static) */
//@{
/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].reason = CORBA::string_dup(reason);

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
 					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc);

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   const char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin, reason and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete[] desc;

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin.c_str());
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const char *desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc);

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin.c_str());
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const string &desc,
					   const char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const char *reason,
					   const char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   const string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		delete[] desc;
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the origin and desc parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   const string &reason,
					   char *desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason.c_str());
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc);
		delete desc;

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   const string &desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin.c_str());
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and origin parameters will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   const string &desc,
					   char *origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(origin);
		delete[] origin;
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());

		throw ex;
	}

/**
 * Re-throw a TANGO DevFailed exception with one more error.
 *
 * The exception is re-thrown with one more DevError
 * object. A default value "Tango::ERR" is defined for the new DevError
 * severity field.
 * The memory used by the reason and desc parameter will be freed by this method
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex The DevFailed exception
 * @param reason The exception DevError object reason field
 * @param desc The exception DevError object desc field
 * @param origin The exception DevError object origin field
 * @param sever The exception DevError object severity field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */

	static inline void re_throw_exception(Tango::DevFailed &ex,
					   char *reason,
					   char *desc,
					   const string &origin,
		            		   Tango::ErrSeverity sever = Tango::ERR)
	{
		long nb_err = ex.errors.length();
		ex.errors.length(nb_err + 1);

		ex.errors[nb_err].severity = sever;
		ex.errors[nb_err].reason = CORBA::string_dup(reason);
		delete[] reason;
		ex.errors[nb_err].origin = CORBA::string_dup(desc);
		delete[] desc;
		ex.errors[nb_err].desc = CORBA::string_dup(origin.c_str());

		throw ex;
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
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static void throw_exception(const CORBA::SystemException &ex,const char *origin);

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field. The memory
 * allocated for this parameter will be freed by this method.
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static void throw_exception(const CORBA::SystemException &ex,char *origin);

/**
 * Generate and throw a TANGO DevFailed exception.
 *
 * The exception is created with a single DevError
 * object. A value "Tango::ERR" is defined for the DevError
 * severity field.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex A CORBA System Exception. The reason and desc fields of the
 * DevError object will be set according to the data in this exception.
 * The desc field is always set to API_CorbaSysException and the reason flag is
 * different depending on the exact type of the CORBA system exception.
 * @param origin The exception DevError object origin field. The memory
 * allocated for this parameter will be freed by this method.
 * @exception DevFailed The thrown exception.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 */
	static void throw_exception(const CORBA::SystemException &ex,const string &origin);


/**
 * Compare two Tango DevFailed exceptions for equality
 *
 * The two DevFailed exceptions are verified by comparing the
 * reason, origin, description and severity of all exceptions in the error stack.
 * The strings reason, origin and description are compared literally.
 * Click <a href="../../../tango_idl/idl_html/_Tango.html#DevFailed">here</a> to read
 * <b>DevFailed</b> exception specification
 *
 * @param ex1 The first DevFailed exception
 * @param ex2 The second DevFailed exception
 * @return A boolean set to true if the two exceptions are equal
 */
	static bool compare_exception(Tango::DevFailed &ex1, Tango::DevFailed &ex2);


//@}

	static char *print_CORBA_SystemException(const CORBA::SystemException *);
	static omni_mutex the_mutex;

protected:
	static char mess[256];
};

} // End of Tango namespace

#endif /* EXCEPT */

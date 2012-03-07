/*
 * tango_rolling_file_appender.h
 *
 * by NL - SOLEIL - 09/2002.
 *
 * Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
 *						European Synchrotron Radiation Facility
 *                      BP 220, Grenoble 38043
 *                      FRANCE
 *
 * This file is part of Tango.
 *
 * Tango is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tango is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Tango.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision$
 *
 *
 */

#ifndef _LOG4TANGO_TANGO_ROLLING_FILE_APPENDER_H_
#define _LOG4TANGO_TANGO_ROLLING_FILE_APPENDER_H_

#ifdef TANGO_HAS_LOG4TANGO

namespace Tango
{
  class TangoRollingFileAppender : public log4tango::RollingFileAppender
	{
		public:
			/**
			 *
			 **/
			TangoRollingFileAppender (const std::string& name,
                                const std::string& fileName,
                                size_t maxFileSize);
			/**
			 *
			 **/
			virtual ~TangoRollingFileAppender ();

      /**
			 *
			 **/
      virtual bool isValid (void) const;
	};

} // namespace tango

#endif // _LOG4TANGO_TANGO_ROLLING_FILE_APPENDER_H_

#endif // TANGO_HAS_LOG4TANGO

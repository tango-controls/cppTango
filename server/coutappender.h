/*
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
 * coutappender.h
 *
 * by NL - SOLEIL - 01/2003.
 *
 * $Revision$
 *
 */

#ifndef _COUT_APPENDER_H_
#define _COUT_APPENDER_H_

#if defined(TANGO_HAS_LOG4TANGO)

namespace Tango {

class CoutAppender : public log4tango::LayoutAppender
{
public:
  /**
   *
   **/
  CoutAppender (const std::string& name);

  /**
   *
   **/
  virtual ~CoutAppender ();

  /**
   *
   **/
  inline virtual bool reopen() {
    return true;
  }

  /**
   *
   **/
  inline virtual void close() {
    //no-op
  }

protected:
  /**
   *
   **/
  virtual int _append (const log4tango::LoggingEvent& event);
};

} // namespace tango

#endif // _COUT_APPENDER_H_

#endif // TANGO_HAS_LOG4TANGO

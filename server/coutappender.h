/*
 * Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
 * $Log$
 * Revision 3.3  2009/01/21 12:49:04  taurel
 * - Change CopyRights for 2009
 *
 * Revision 3.2  2008/10/06 15:00:36  taurel
 * - Changed the licensing info from GPL to LGPL
 *
 * Revision 3.1  2008/10/03 06:51:36  taurel
 * - Add some licensing info in each files
 *
 * Revision 3.0  2003/03/25 16:41:59  taurel
 * Many changes for Tango release 3.0 including
 * - Added full logging features
 * - Added asynchronous calls
 * - Host name of clients now stored in black-box
 * - Three serialization model in DS
 * - Fix miscellaneous bugs
 * - Ported to gcc 3.2
 * - Added ApiUtil::cleanup() and destructor methods
 * - Some internal cleanups
 * - Change the way how TangoMonitor class is implemented. It's a recursive
 *   mutex
 *
 * Revision 2.2  2003/03/11 17:55:49  nleclercq
 * Switch from log4cpp to log4tango
 *
 * Revision 2.1  2003/02/17 14:57:39  taurel
 * Added the new Tango logging stuff (Thanks Nicolas from Soleil)
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

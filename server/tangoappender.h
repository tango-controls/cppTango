/*
 * tangoappender.h
 *
 * by NL - SOLEIL - 09/2002.
 *
 * Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
 * $Log$
 * Revision 3.4  2010/09/09 13:46:45  taurel
 * - Add year 2010 in Copyright notice
 *
 * Revision 3.3  2009/01/21 12:49:03  taurel
 * - Change CopyRights for 2009
 *
 * Revision 3.2  2008/10/06 15:01:36  taurel
 * - Changed the licensing info from GPL to LGPL
 *
 * Revision 3.1  2008/10/03 06:53:09  taurel
 * - Add some licensing info in each files
 *
 * Revision 3.0  2003/03/25 16:47:53  taurel
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
 * Revision 2.3  2003/03/13 15:17:50  nleclercq
 * Minor modifications on logging implementation
 *
 * Revision 2.2  2003/03/11 17:55:58  nleclercq
 * Switch from log4cpp to log4tango
 *
 * Revision 2.1  2003/02/17 14:57:45  taurel
 * Added the new Tango logging stuff (Thanks Nicolas from Soleil)
 *
 *
 */

#ifndef _TANGO_APPENDER_H_
#define _TANGO_APPENDER_H_

#ifdef TANGO_HAS_LOG4TANGO

namespace Tango
{

class TangoAppender : public log4tango::Appender
{
public:
  /**
   *
   **/
  TangoAppender (const std::string& src_name,
                 const std::string& name,
                 const std::string& dev_name,
                 bool open_connection = true);
  /**
   *
   **/
  virtual ~TangoAppender ();

  /**
   *
   **/
  virtual bool requires_layout (void) const;
      
  /**
   *
   **/
  virtual void set_layout(log4tango::Layout* layout);

  /**
   *
   **/
  virtual void close (void);
                 
  /**
   *
   **/
  virtual bool reopen (void);

  /**
   *
   **/
  virtual bool is_valid (void) const;

protected:
  /**
   *
   **/
  virtual int _append (const log4tango::LoggingEvent& event); 

private:
  /**
   *
   **/
  const std::string _dev_name;

  /**
   *
   **/
  const std::string _src_name;

  /**
   *
   **/
  DeviceProxy *_dev_proxy;
};

} // namespace tango

#endif // _TANGO_APPENDER_H_


#endif // TANGO_HAS_LOG4TANGO

/*
 * tangoappender.h
 *
 * by NL - SOLEIL - 09/2002.
 *
 * Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
 *
 */

#ifndef _TANGO_APPENDER_H_
#define _TANGO_APPENDER_H_

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
  DeviceProxy   *_dev_proxy;
  DevULong      _req_ctr;
};

} // namespace tango

#endif // _TANGO_APPENDER_H_

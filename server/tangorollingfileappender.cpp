static const char *RcsId = "$Id$\n$Name$";

//+=============================================================================
//
// file :         tangorollingfileappender.cpp
//
// description :  Implementation of the DServer logging oriented commands
//
// project :      TANGO
//
// author(s) :    N.Leclercq - SOLEIL
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
//-=============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>

#ifdef TANGO_HAS_LOG4TANGO

#include <tangorollingfileappender.h>

namespace Tango
{
  TangoRollingFileAppender::TangoRollingFileAppender(const std::string& name,
                                                     const std::string& fileName,
                                                     size_t maxFileSize)
    : log4tango::RollingFileAppender(name, fileName, maxFileSize * 1024)
  {
    // no-op
  }

  TangoRollingFileAppender::~TangoRollingFileAppender()
  {
    // no-op
  }

  bool TangoRollingFileAppender::isValid (void) const
  {
    return (_fd < 0) ? false : true;
  }

} // namespace tango

#endif // TANGO_HAS_LOG4TANGO

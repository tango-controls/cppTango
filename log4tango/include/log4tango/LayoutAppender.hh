//
// LayoutAppender.hh
//
// Copyright (C) :  2000 - 2002
//					LifeLine Networks BV (www.lifeline.nl). All rights reserved.
//					Bastiaan Bakker. All rights reserved.   
//					
//					2004,2005,2006,2007,2008,2009,2010
//					Synchrotron SOLEIL
//                	L'Orme des Merisiers
//                	Saint-Aubin - BP 48 - France
//
// This file is part of log4tango.
//
// Log4ango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Log4tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with Log4Tango.  If not, see <http://www.gnu.org/licenses/>.

#ifndef _LOG4TANGO_LAYOUTAPPENDER_H
#define _LOG4TANGO_LAYOUTAPPENDER_H

#include <log4tango/Portability.hh>
#include <string>
#include <log4tango/Appender.hh>
#include <log4tango/PatternLayout.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// class : LayoutAppender (superclass for appenders that require a Layout)
//-----------------------------------------------------------------------------
class LOG4TANGO_EXPORT LayoutAppender : public Appender 
{
public:

  typedef Layout DefaultLayoutType;

  LayoutAppender(const std::string& name);

  virtual ~LayoutAppender();

  virtual bool requires_layout() const;

  virtual void set_layout (Layout* layout = 0);

protected:

  Layout& get_layout();

private:
  Layout* _layout;
};

} // namespace log4tango

#endif // _LOG4TANGO_LAYOUTAPPENDER_H


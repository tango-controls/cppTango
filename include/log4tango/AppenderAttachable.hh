//
// AppenderAttachable.hh
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

#ifndef _LOG4TANGO_APPENDER_ATTACHABLE_H
#define _LOG4TANGO_APPENDER_ATTACHABLE_H

#include <log4tango/Portability.hh>
#include <map>
#include <vector>
#include <log4tango/Appender.hh>
#include <log4tango/threading/Threading.hh>

namespace log4tango {

/**
 * A map of appenders
 **/
typedef std::map<std::string, Appender*> AppenderMap;

/**
 * A map of appenders iterator
 **/
typedef AppenderMap::iterator AppenderMapIterator;

/**
 * Define what is a list of appenders
 **/
typedef std::vector<Appender*> AppenderList;


//-----------------------------------------------------------------------------
// class : AppenderAttachable
//----------------------------------------------------------------------------- 
class LOG4TANGO_EXPORT AppenderAttachable 
{
  public:

    /**
     * Constructor.
     **/
    AppenderAttachable ();

    /**
     * Destructor.
     **/
    virtual ~AppenderAttachable ();
    
    /**
     * Adds an Appender. Does nothing if the appender is NULL 
     * or already attached.
     **/
    void add_appender (Appender* appender);

    /**
     * Get all previously added appenders as a vector.
     * The caller must call "release" on each Appender in 
     * the returned list when it is no longer needed (the 
     * Appender class is ref-counted).
     **/
    AppenderList get_all_appenders (void);

    /**
     * Get an appender by name. 
     * The caller must call "release" on the returned Appender 
     * when it is no longer needed (the Appender class is 
     * ref-counted). Returns 0 if there is no Appender named 
     * "name" currently attached.
     **/
    Appender* get_appender (const std::string& name);

    /**
     * Returns true if the specified appender is in list of 
     * attached appanders, false otherwise.
     **/
    bool is_attached (Appender* appender);

    /**
     * Removes all appenders for this Logger.
     **/
    void remove_all_appenders();

    /**
     * Remove the appender passed as parameter from the list of appenders.
     **/
    void remove_appender(Appender* appender);

    /**
     * Remove the appender with the name passed as parameter from the 
     * list of appenders.
     **/
    void remove_appender(const std::string& name);

  protected:

    /**
     * Appenders repository.
     **/
    AppenderMap _appenders;

    /**
     * A mutex to protect the repository against race conditions.
     **/
    threading::Mutex _appendersMutex;

    AppenderAttachable (const AppenderAttachable& other);
    AppenderAttachable& operator=(const AppenderAttachable& other);
};

} // namespace log4tango

#endif // _LOG4TANGO_APPENDER_ATTACHABLE_H

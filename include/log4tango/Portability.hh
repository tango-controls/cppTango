/*
 * Portability.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_PORTABILITY_H
#define _LOG4TANGO_PORTABILITY_H

#if defined (_MSC_VER) || defined(__BORLANDC__)
# include <log4tango/config-win32.h>
#else
# include <log4tango/config.h>
#endif

#include <log4tango/Export.hh>

#if defined(_MSC_VER)
# pragma warning( disable : 4786 ) // 255 char debug symbol limit
# pragma warning( disable : 4290 ) // throw specifier not implemented
# pragma warning( disable : 4251 ) // "class XXX should be exported"
#endif

#ifndef LOG4TANGO_HAVE_SSTREAM
#include <strstream>
namespace std {
  class LOG4TANGO_EXPORT ostringstream : public ostrstream {
    public:
      std::string str();
      void str (const char*);
  };
}
#endif // LOG4TANGO_HAVE_SSTREAM

#endif // _LOG4TANGO_PORTABILITY_H

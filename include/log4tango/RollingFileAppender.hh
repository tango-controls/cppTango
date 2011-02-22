/*
 * RollingFileAppender.hh
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#ifndef _LOG4TANGO_ROLLINGFILEAPPENDER_H
#define _LOG4TANGO_ROLLINGFILEAPPENDER_H

#include <log4tango/Portability.hh>
#include <log4tango/FileAppender.hh>

namespace log4tango {

//-----------------------------------------------------------------------------
// class RollingFileAppender (olls over the logfile)
//----------------------------------------------------------------------------- 
class LOG4TANGO_EXPORT RollingFileAppender : public FileAppender
{
 public:

    RollingFileAppender(const std::string& name, 
                        const std::string& file_name,
                        size_t max_fs = 10*1024*1024, 
                        unsigned int max_bi = 1,
                        bool append = true,
                        mode_t mode = 00644);

    virtual void set_max_backup_index(unsigned int maxBackups);

    virtual unsigned int get_max_backup_index() const;

    virtual void set_maximum_file_size (size_t max_fs);

    virtual size_t get_max_file_size() const;

    virtual void roll_over();

protected:

    virtual int _append (const LoggingEvent& event);

    unsigned int _max_backup_index;

    size_t _max_file_size;
};

} // namespace log4tango

#endif // _LOG4TANGO_ROLLINGFILEAPPENDER_H

/*
 * PatternLayout.cpp
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "PortabilityImpl.hh"

#include <log4tango/PatternLayout.hh>
#include <log4tango/Level.hh> 
#include <log4tango/NDC.hh>
#include <log4tango/TimeStamp.hh>

#ifdef LOG4TANGO_HAVE_SSTREAM
# include <sstream>
#else
# include <strstream>
#endif

#include <iomanip>
#include <ctime>
#include <cmath>

#ifdef LOG4TANGO_HAVE_INT64_T
# ifdef LOG4TANGO_HAVE_STDINT_H
#   include <stdint.h>
# endif // LOG4TANGO_HAVE_STDINT_H
# ifdef LOG4TANGO_MISSING_INT64_OSTREAM_OP
  // workaround suggested at:
  // http://support.microsoft.com/default.aspx?scid=kb;EN-US;q168440
  #include <stdio.h>
  std::ostream& operator<<(std::ostream& os, int64_t i) {
    char buf[20];
    ::sprintf(buf,"%I64d", i);
    return os << buf;
  }
# endif // LOG4TANGO_MISSING_INT64_OSTREAM_OP
#endif // LOG4TANGO_HAVE_INT64_T

namespace log4tango {

  struct StringLiteralComponent : public PatternLayout::PatternComponent 
  {
    StringLiteralComponent(const std::string& literal) 
      : _literal(literal) 
    {
      //no-op
    }
    virtual void append(std::ostringstream& out, const LoggingEvent& event) 
    {
      out << _literal;
    }
    private:
      std::string _literal;
  };

  struct LoggerNameComponent : public PatternLayout::PatternComponent 
  {
    LoggerNameComponent(std::string specifier) {
            if (specifier == "") {
                _precision = -1;
            } else {
#ifdef LOG4TANGO_HAVE_SSTREAM 
                std::istringstream s(specifier);
#else
                std::istrstream s(specifier.c_str());
#endif
                s >> _precision;
            }
        }

        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            if (_precision == -1) {
                out << event.logger_name;
            } else {
                std::string::size_type begin = std::string::npos;
                for(int i = 0; i < _precision; i++) {
                    begin = event.logger_name.rfind('.', begin - 2);
                    if (begin == std::string::npos) {
                        begin = 0;
                        break;
                    }
                    begin++;
                }
                out << event.logger_name.substr(begin);
            }
        }

        private:
        int _precision;
    };

    struct MessageComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            out << event.message;
        }
    };

#ifdef LOG4TANGO_HAS_NDC
    struct NDCComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            out << event.ndc;
        }
    };
#endif

    struct LevelComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            out << Level::get_name(event.level);
        }
    };

    struct ThreadNameComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            out << event.thread_id; // event.thread_name;
        }
    };

    struct ProcessorTimeComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            out << ::clock();
        }
    };

    struct TimeStampComponent : public PatternLayout::PatternComponent {
        static const char* const FORMAT_ISO8601;
        static const char* const FORMAT_ABSOLUTE;
        static const char* const FORMAT_DATE;

        TimeStampComponent(std::string timeFormat) {
            if ((timeFormat == "") || (timeFormat == "ISO8601")) {
                timeFormat = FORMAT_ISO8601;
            } else if (timeFormat == "ABSOLUTE") {
                timeFormat = FORMAT_ABSOLUTE;
            } else if (timeFormat == "DATE") {
                timeFormat = FORMAT_DATE;
            }
            std::string::size_type pos = timeFormat.find("%l");
            if (pos == std::string::npos) {
                _printMillis = false;
                _timeFormat1 = timeFormat; 
            } else {
                _printMillis = true;
                _timeFormat1 = timeFormat.substr(0, pos);
                _timeFormat2 = timeFormat.substr(pos + 2);
            }
        }

        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            struct tm *currentTime;
            time_t t = event.timestamp.get_seconds();
            currentTime = std::localtime(&t);
            char formatted[100];
            std::string timeFormat;
            if (_printMillis) {
                std::ostringstream formatStream;
                formatStream << _timeFormat1 
                             << std::setw(3) << std::setfill('0')
                             << event.timestamp.get_milliseconds()
                             << _timeFormat2;
                timeFormat = formatStream.str();
            } else {
                timeFormat = _timeFormat1;
            }
            std::strftime(formatted, sizeof(formatted), timeFormat.c_str(), currentTime);
            out << formatted;
        }

        private:
        std::string _timeFormat1;
        std::string _timeFormat2;
        bool _printMillis;
    };

    const char* const TimeStampComponent::FORMAT_ISO8601 = "%Y-%m-%d %H:%M:%S,%l";
    const char* const TimeStampComponent::FORMAT_ABSOLUTE = "%H:%M:%S,%l";
    const char* const TimeStampComponent::FORMAT_DATE = "%d %b %Y %H:%M:%S,%l";

    struct SecondsSinceEpochComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
            out << event.timestamp.get_seconds();
        }
    };

    struct MillisSinceEpochComponent : public PatternLayout::PatternComponent {
        virtual void append(std::ostringstream& out, const LoggingEvent& event) {
#ifdef LOG4TANGO_HAVE_INT64_T
            int64_t t = event.timestamp.get_seconds() -
                TimeStamp::get_start_time().get_seconds();
            t *= 1000;
            t += event.timestamp.get_milliseconds() -
                TimeStamp::get_start_time().get_milliseconds();
            
            out << t;
#else
            double t = event.timestamp.get_seconds() -
                TimeStamp::get_start_time().get_seconds();
            t *= 1000;
            t += event.timestamp.get_milliseconds() -
                TimeStamp::get_start_time().get_milliseconds();

            out << std::setiosflags(std::ios::fixed)
                << std::setprecision(0) << t;
#endif
        }
    };

  struct FormatModifierComponent : public PatternLayout::PatternComponent {
    FormatModifierComponent(PatternLayout::PatternComponent* component,
                            int minWidth, int maxWidth) :
        _component(component) , 
        _minWidth(minWidth < 0 ? -minWidth : minWidth),
        _maxWidth(maxWidth),
        _alignLeft(minWidth < 0) {
  }

  virtual ~FormatModifierComponent() {
    delete _component;
  }

  virtual void append(std::ostringstream& out, const LoggingEvent& event) {
    std::ostringstream s;
    _component->append(s, event);
    std::string msg = s.str();
    if (_maxWidth > 0) {
        msg.erase(_maxWidth);
    }
    int fillCount = _minWidth - msg.length();
    if (fillCount > 0) {
        if (_alignLeft) {
            out << msg << std::string(fillCount, ' ');
        } else {
            out << std::string(fillCount, ' ') << msg;
        }
    } else {
        out << msg;
    }
  }

  private:
    PatternLayout::PatternComponent* _component;
    int _minWidth;
    int _maxWidth;
    bool _alignLeft;
};

#ifdef LOG4TANGO_HAS_NDC
    const char* PatternLayout::BASIC_CONVERSION_PATTERN = "%R %p %c %x: %m%n";
#else
    const char* PatternLayout::BASIC_CONVERSION_PATTERN = "%R %p %c %m%n";
#endif

PatternLayout::PatternLayout() {
  set_conversion_pattern(BASIC_CONVERSION_PATTERN);
}

PatternLayout::~PatternLayout() {
  clear_conversion_pattern();
}

void PatternLayout::clear_conversion_pattern() {
  for(ComponentVector::const_iterator i = _components.begin();
    i != _components.end(); ++i) {
    delete (*i);
  }
  _components.clear();
  _conversionPattern = "";
}

int PatternLayout::set_conversion_pattern (const std::string& conversionPattern) 
{
#ifdef LOG4TANGO_HAVE_SSTREAM 
    std::istringstream conversionStream(conversionPattern);
#else
    std::istrstream conversionStream(conversionPattern.c_str());
#endif
    std::string literal;

    char ch;
    PatternLayout::PatternComponent* component = NULL;
    int minWidth = 0;
    int maxWidth = 0;
    clear_conversion_pattern();
    while (conversionStream.get(ch)) {
        if (ch == '%') {
            // readPrefix;
            {
                char ch2;
                conversionStream.get(ch2);
                if ((ch2 == '-') || ((ch2 >= '0') && (ch2 <= '9'))) {
                    conversionStream.putback(ch2);
                    conversionStream >> minWidth;
                    conversionStream.get(ch2);
                } 
                if (ch2 == '.') {
                    conversionStream >> maxWidth;
                } else {
                    conversionStream.putback(ch2);
                }                        
            }
            if (!conversionStream.get(ch)) {
              return -1;
            }
            std::string specPostfix = "";
            // read postfix
            {
                char ch2;
                if (conversionStream.get(ch2)) {
                    if (ch2 == '{') {
                        while(conversionStream.get(ch2) && (ch2 != '}'))
                            specPostfix += ch2;
                    } else {
                        conversionStream.putback(ch2);
                    }
                }
            }
            switch (ch) {
            case '%':
                literal += ch;
                break;
            case 'm':
                component = new MessageComponent();
                break;
            case 'n':
                {
                    std::ostringstream endline;
                    endline << std::endl;
                    literal += endline.str();
                }
                break;
            case 'c':
                component = new LoggerNameComponent(specPostfix);
                break;
            case 'd':
                component = new TimeStampComponent(specPostfix);
                break;
            case 'p':
                component = new LevelComponent();
                break;
            case 'r':
                component = new MillisSinceEpochComponent();
                break;
            case 'R':
                component = new SecondsSinceEpochComponent();
                break;
            case 'u':
                component = new ProcessorTimeComponent();
                break;
#ifdef LOG4TANGO_HAS_NDC
            case 'x':
                component = new NDCComponent();
                break;
#endif
            default:
                return -1;                 
            }
            if (component) {
                if (!literal.empty()) {
                    _components.push_back(new StringLiteralComponent(literal));
                    literal = "";
                }
                if ((minWidth != 0) || (maxWidth != 0)) {
                    component = new FormatModifierComponent(component, minWidth, maxWidth);
                    minWidth = maxWidth = 0;
                }
                _components.push_back(component);
                component = NULL;
            }
        } else {
            literal += ch;
        }
    }
    if (!literal.empty()) {
        _components.push_back(new StringLiteralComponent(literal));
    }

    _conversionPattern = conversionPattern;

    return 0;
}

std::string PatternLayout::get_conversion_pattern() const {
    return _conversionPattern;
}

std::string PatternLayout::format(const LoggingEvent& event) {
    std::ostringstream message;

    for(ComponentVector::const_iterator i = _components.begin();
        i != _components.end(); ++i) {
        (*i)->append(message, event);
    }

    return message.str();
}

} // namespace log4tango

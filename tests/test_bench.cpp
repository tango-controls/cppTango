#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>

#include <log4tango/Logger.hh>
#include <log4tango/OstreamAppender.hh>
#include <log4tango/FileAppender.hh>
#include <log4tango/Layout.hh>
#include <log4tango/PatternLayout.hh>
#include <log4tango/TimeStamp.hh>

#include "clock.hh"


// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    log4tango::Level::Value  pv;
    pv = argc > 1 ? std::atoi(argv[1]) : log4tango::Level::DEBUG;

    int count;
    count = argc > 2 ? std::atoi(argv[2]) : 100;

    size_t size;
    size =  argc > 3 ? std::atoi(argv[3]) : 128;

    std::cout << "  level: " << log4tango::Level::get_name(pv)
              << "  count: " << count << std::endl << " iterations"
	      << "  size: " << size  << std::endl << " bytes"
	      << std::endl;

    log4tango::Logger log("cat");
    log.set_level(pv - 100);

    log4tango::OstreamAppender *ostreamAppender = new log4tango::OstreamAppender("cerr", &std::cerr);
    log.add_appender(ostreamAppender);

    Clock  clock;
    char*  buffer = new char[size + 1];

    std::memset(buffer, 'X', size + 1);
    buffer[size] = '\0';

    const int num_tests = 7;

    const char* tests[num_tests] = {
    	"  layout::log.error(buffer) : ",
	"  layout::log.error_stream << std:string : ",
	"  layout::log.error(std:string) :  ",
	"  patternlayout::log.error(buffer) : ",
	"  patternlayout::log.error_stream << std:string : ",
	"  patternlayout::log.error(std:string) :  ",
	"  patternlayout::if cat.is_p_enabled(p) log.error_stream << std:string : ",
    };

    float results[num_tests];
    ::memset(results, 0, num_tests * sizeof(float));

    {
	clock.start();
	for (int i = 0; i < count; i++) log.error("%s", buffer);
	clock.stop();
	results[0] = ((float)clock.elapsed()) / count;
    }
    {
	std::string str(size, 'X');
	clock.start();
	for (int i = 0; i < count; i++) log.error_stream() << str;
	clock.stop();
	results[1] = ((float)clock.elapsed()) / count;
    }
    {
	std::string str(size, 'X');
	clock.start();
	for (int i = 0; i < count; i++) log.error(str);
	clock.stop();
	results[2] = ((float)clock.elapsed()) / count;
    }

    {
        log4tango::PatternLayout* patternLayout = new log4tango::PatternLayout();
        patternLayout->set_conversion_pattern("%R %p %c %m\n");
        ostreamAppender->set_layout(patternLayout);
    }
    {
	clock.start();
	for (int i = 0; i < count; i++) log.error("%s", buffer);
	clock.stop();
	results[3] = ((float)clock.elapsed()) / count;
    }
    {
	std::string str(size, 'X');
	clock.start();
	for (int i = 0; i < count; i++) log.error_stream() << str;
	clock.stop();
	results[4] = ((float)clock.elapsed()) / count;
    }
    {
	std::string str(size, 'X');
	clock.start();
	for (int i = 0; i < count; i++) 
	  if (log.is_level_enabled(pv)) 
	    log.error_stream() << str;
	clock.stop();
	results[6] = ((float)clock.elapsed()) / count;
    }
    {
	std::string str(size, 'X');
	clock.start();
	for (int i = 0; i < count; i++) log.error(str);
	clock.stop();
	results[5] = ((float)clock.elapsed()) / count;
    }

    for (int i = 0; i < num_tests; i++) std::cout << tests[i] << results[i] << " us" << std::endl;

    return 0;
}

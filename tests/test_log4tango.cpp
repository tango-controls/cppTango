#include <stdio.h>
#include <iostream>
#include <log4tango/Logger.hh>
#include <log4tango/Appender.hh>
#include <log4tango/OstreamAppender.hh>
#include <log4tango/Layout.hh>
#include <log4tango/Level.hh>
#ifdef LOG4TANGO_HAS_NDC
# include <log4tango/NDC.hh>
#endif

int test_get_appender (log4tango::Logger& logger,
                       log4tango::Appender* appender_1,
                       log4tango::Appender* appender_2,
                       log4tango::Appender* appender_3)
{
  log4tango::Appender *tmp;

  tmp = logger.get_appender("appender_1");
  if (tmp == appender_1) {
    std::cout << "OK: tmp == appender_1" << std::endl;
  }
  else {
    std::cout << "KO: tmp != appender_1" << std::endl;
    return -1;
  }
  tmp = logger.get_appender("appender_2");
  if (tmp == appender_2) {
    std::cout << "OK: tmp == appender_2" << std::endl;
  }
  else {
    std::cout << "KO: tmp != appender_2" << std::endl;
    return -1;
  }
  tmp = logger.get_appender("appender_3");
  if (tmp == appender_3) {
    std::cout << "OK: tmp == appender_3" << std::endl;
  }
  else {
    std::cout << "OK: tmp != appender_3" << std::endl;
    return -1;
  }
  return 0;
}

void test_level()
{
  log4tango::Logger cat_1("cat_1");
  cat_1.set_level(log4tango::Level::ERROR);
  cat_1.remove_all_appenders();
  
  log4tango::Appender* appender_1 = 
      new log4tango::OstreamAppender("appender_1", &std::cout);
  appender_1->set_layout(new log4tango::Layout());
  cat_1.add_appender(appender_1);

  std::cout << "level is OFF: no message should be printed" << std::endl;
  cat_1.set_level(log4tango::Level::OFF);
  cat_1.fatal("fatal log");
  cat_1.error("error log");
  cat_1.warn ("warn log");
  cat_1.info ("info log");
  cat_1.debug("debug log");

  std::cout << "level is FATAL: 1 messages should be printed" << std::endl;
  cat_1.set_level(log4tango::Level::FATAL);
  cat_1.fatal("fatal log");
  cat_1.error("error log");
  cat_1.warn ("warn log");
  cat_1.info ("info log");
  cat_1.debug("debug log");

  std::cout << "level is ERROR: 2 messages should be printed" << std::endl;
  cat_1.set_level(log4tango::Level::ERROR);
  cat_1.fatal("fatal log");
  cat_1.error("error log");
  cat_1.warn ("warn log");
  cat_1.info ("info log");
  cat_1.debug("debug log");

  std::cout << "level is WARN: 3 messages should be printed" << std::endl;
  cat_1.set_level(log4tango::Level::WARN);
  cat_1.fatal("fatal log");
  cat_1.error("error log");
  cat_1.warn ("warn log");
  cat_1.info ("info log");
  cat_1.debug("debug log");

  std::cout << "level is INFO: 4 messages should be printed" << std::endl;
  cat_1.set_level(log4tango::Level::INFO);
  cat_1.fatal("fatal log");
  cat_1.error("error log");
  cat_1.warn ("warn log");
  cat_1.info ("info log");
  cat_1.debug("debug log");

  std::cout << "level is DEBUG: 5 messages should be printed" << std::endl;
  cat_1.set_level(log4tango::Level::DEBUG);
  cat_1.fatal("fatal log");
  cat_1.error("error log");
  cat_1.warn ("warn log");
  cat_1.info ("info log");
  cat_1.debug("debug log");

  cat_1.remove_all_appenders();

  appender_1 = new log4tango::OstreamAppender("appender_1", &std::cout);
  appender_1->set_layout(new log4tango::Layout());
  cat_1.add_appender(appender_1);

  log4tango::Appender* appender_2 = 
      new log4tango::OstreamAppender("appender_2", &std::cout);
  appender_2->set_layout(new log4tango::Layout());
  cat_1.add_appender(appender_2);

  log4tango::Appender* appender_3 = 
      new log4tango::OstreamAppender("appender_3", &std::cout);
  appender_3->set_layout(new log4tango::Layout());
  cat_1.add_appender(appender_3);

  std::cout << std::endl;

  unsigned int i;
  log4tango::AppenderList al = cat_1.get_all_appenders();
  std::cout << "cat_1 has " << al.size() << " appenders" << std::endl;
  for (i = 0; i < al.size(); i++) {
    std::cout << "`-> appender#" << i << ": " <<  al[i]->get_name() << std::endl;
  }

  std::cout << std::endl;

  std::cout << "Following test should work" << std::endl;
  test_get_appender(cat_1, appender_1, appender_2, appender_3);

  cat_1.remove_all_appenders();

  std::cout << std::endl;
  std::cout << "Following test should fail" << std::endl;
  test_get_appender(cat_1, appender_1, appender_2, appender_3);

  appender_1 = new log4tango::OstreamAppender("appender_1", &std::cout);
  appender_2 = new log4tango::OstreamAppender("appender_2", &std::cout);
  cat_1.add_appender(appender_1);
  cat_1.add_appender(appender_2);

  std::cout << std::endl;

  al = cat_1.get_all_appenders();
  std::cout << "cat_1 has " << al.size() << " appenders" << std::endl;
  for (i = 0; i < al.size(); i++) {
    std::cout << "`-> appender#" << i << ": " <<  al[i]->get_name() << std::endl;
  }

  cat_1.remove_appender(appender_1);

  std::cout << std::endl;

  al = cat_1.get_all_appenders();
  std::cout << "cat_1 has " << al.size() << " appenders" << std::endl;
  for (i = 0; i < al.size(); i++) {
    std::cout << "`-> appender#" << i << ": " <<  al[i]->get_name() << std::endl;
  }

  cat_1.remove_appender(appender_2);

  std::cout << std::endl;

  al = cat_1.get_all_appenders();
  std::cout << "cat_1 has " << al.size() << " appenders" << std::endl;
  for (i = 0; i < al.size(); i++) {
    std::cout << "`-> appender#" << i << ": " <<  al[i]->get_name() << std::endl;
  }

  cat_1.remove_appender(appender_3);

} /* end test_level() */


int main(int argc, char** argv) {    

  test_level();

  log4tango::Logger cat_1("cat_1");
  log4tango::Appender* appender_1 = 
      new log4tango::OstreamAppender("appender_1", &std::cout);
  appender_1->set_layout(new log4tango::Layout());
  cat_1.add_appender(appender_1);
  cat_1.set_level(log4tango::Level::ERROR);

  log4tango::Logger cat_2("cat_2");
  log4tango::Appender* appender_2 = 
      new log4tango::OstreamAppender("appender_2", &std::cout);
  appender_2->set_layout(new log4tango::Layout());
  cat_2.add_appender(appender_2);
  cat_2.set_level(log4tango::Level::INFO);

  std::cout << std::endl;

  std::cout << "cat_1 level: " 
            << cat_1.get_level()
            << " - "
            << log4tango::Level::get_name(cat_1.get_level()) 
            << std::endl;

  std::cout << "cat_2 level: " 
            << cat_2.get_level()
            << " - "
            << log4tango::Level::get_name(cat_2.get_level()) 
            << std::endl;

  return 0;
}

SUBDIRS = threading

liblog4tangoincludedir = $(includedir)/tango/log4tango

liblog4tangoinclude_HEADERS =  \
	Appender.hh \
	AppenderAttachable.hh \
	LayoutAppender.hh \
	FileAppender.hh \
	RollingFileAppender.hh \
	OstreamAppender.hh \
	Layout.hh \
	PatternLayout.hh \
	XmlLayout.hh \
	Logger.hh \
  LogSeparator.hh \
	LoggerStream.hh \
  LogStream.hh \
  LogStreambuf.hh \
	LoggingEvent.hh \
	Level.hh \
	NDC.hh \
	TimeStamp.hh \
	Filter.hh \
	Export.hh \
	Portability.hh \
	config.h \
	config-win32.h

dist-hook:
	rm -f $(distdir)/config.h

distclean-local:
	rm config.h



TESTS = test_log4tango test_bench

check_PROGRAMS = $(TESTS)

#check_DATA =

#EXTRA_DIST =

#noinst_PROGRAMS =

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src

test_log4tango_SOURCES = test_log4tango.cpp
test_log4tango_LDADD = $(top_builddir)/src/liblog4tango.la

test_bench_SOURCES = clock.cpp clock.hh test_bench.cpp
test_bench_LDADD = $(top_builddir)/src/liblog4tango.la

distclean-local:
	$(RM) -f *.log

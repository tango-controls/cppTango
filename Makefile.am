SUBDIRS = config src include tests doc

DIST_SUBDIRS = config src include doc tests


EXTRA_DIST = log4tango.pc.in \
		log4tango.spec.in log4tango.spec log4tango.m4 \
		m4/AC_CREATE_PREFIX_CONFIG_H.m4 \
		m4/BB_ENABLE_DOXYGEN.m4 \
		m4/AC_CXX_HAVE_SSTREAM.m4 \
		m4/CREATE_GENERIC_CONFIG.m4 \
		m4/AC_CXX_NAMESPACES.m4 \
		m4/PETI_PEDANTIC_GCC.m4 \
		m4/AC_FUNC_SNPRINTF.m4 \
		m4/AC_C_INT64_T.m4
		
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = log4tango.pc		

.PHONY: check rpm docs doc-dist

distclean-local:
	$(RM) CONTENTS TAGS log4tango-config

CONTENTS::
	find $(top_srcdir) \( ! -type l -a -name '*.cpp' -o -name '*.hh' -o -name Makefile.am \) > $@

TAGS:: CONTENTS
	etags `cat $<`

rpm: dist
	rpm -ta $(PACKAGE)-$(VERSION).tar.gz
	mv -f /usr/src/redhat/SRPMS/$(PACKAGE)-$(VERSION)-*.rpm $(top_srcdir)
	mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-$(VERSION)-*.rpm $(top_srcdir)
	mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-devel-$(VERSION)-*.rpm \
		$(top_srcdir)
	mv -f /usr/src/redhat/RPMS/*/$(PACKAGE)-doc-$(VERSION)-*.rpm \
		$(top_srcdir)

debian:
	chmod a+x $(top_srcdir)/debian/rules
	dpkg-buildpackage -rfakeroot -sa -us -uc -tc

docs:
	$(MAKE) -C doc

doc-dist: docs
	tar -zcf $(PACKAGE)-docs-$(VERSION).tar.gz -C $(top_srcdir)/doc/html --exclude CVS .
 

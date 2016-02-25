
# We need the ORB to compile and the tango header files to compile
AM_CPPFLAGS = $(ORB_INCLUDE_PREFIX)

IDL_BASE_DIR = $(top_srcdir)/lib/idl

BUILT_SOURCES = tangoSK.cpp

tangoSK.cpp: Makefile
	$(IDL) -I$(IDL_BASE_DIR) -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba $(IDL_BASE_DIR)/tango.idl
	@SED@ -i '/typedef\ _CORBA_ConstrType_Variable_Var<DevPipeData>/a \/\/Added by Tango team\
virtual ~DevPipeData() {if (mut_ptr != NULL)mut_ptr->unlock();}\
DevPipeData() {mut_ptr=NULL;}\
void set_pipe_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\
void rel_pipe_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\
omni_mutex *mut_ptr;' tango.h
	@SED@ -i '/typedef\ _CORBA_ConstrType_Variable_Var<AttributeValue_4>/a \/\/Added by Tango team\
virtual ~AttributeValue_4() {if (mut_ptr != NULL)mut_ptr->unlock();}\
AttributeValue_4() {mut_ptr=NULL;}\
void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\
void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\
omni_mutex *get_attr_mutex() {return mut_ptr;}\
omni_mutex *mut_ptr;' tango.h
	@SED@ -i '/typedef\ _CORBA_ConstrType_Variable_Var<AttributeValue_5>/a \/\/Added by Tango team\
virtual ~AttributeValue_5() {if (mut_ptr != NULL)mut_ptr->unlock();}\
AttributeValue_5() {mut_ptr=NULL;}\
void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\
void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\
omni_mutex *get_attr_mutex() {return mut_ptr;}\
omni_mutex *mut_ptr;' tango.h

# We're making a libtool convenience library which is not to be installed,
# therefore the automake noinst variable
noinst_LTLIBRARIES = libidl.la


# These are the sources for the library.
nodist_libidl_la_SOURCES = tangoSK.cpp  \
                           tangoDynSK.cpp \
                           tango.h

idldir=${includedir}/tango/idl

install-data-local:
	mkdir -p $(DESTDIR)/$(idldir)
	cp -f tango.h $(DESTDIR)/$(idldir)/tango.h
	chmod u+w $(DESTDIR)/$(idldir)/tango.h

uninstall-local:
	rm $(DESTDIR)/$(idldir)/tango.h
	rmdir $(DESTDIR)/$(idldir)

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm *.cpp
	-rm *.h
	

#idl_HEADERS=tango.h


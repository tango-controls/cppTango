
IDL_BASE_DIR = $(top_srcdir)/lib/idl

BUILT_SOURCES = tangoSK.cpp

tangoSK.cpp: Makefile
	$(IDL) -I$(IDL_BASE_DIR) -bcxx -Wbh=.h -Wbs=SK.cpp -Wbd=DynSK.cpp -Wba $(IDL_BASE_DIR)/tango.idl
	sed -i '/typedef\ _CORBA_ConstrType_Variable_Var<DevPipeData>/a \/\/Added by Tango team\
virtual ~DevPipeData() {if (mut_ptr != NULL)mut_ptr->unlock();}\
DevPipeData() {mut_ptr=NULL;}\
void set_pipe_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\
void rel_pipe_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\
omni_mutex *mut_ptr;' tango.h
	sed -i '/typedef\ _CORBA_ConstrType_Variable_Var<AttributeValue_4>/a \/\/Added by Tango team\
virtual ~AttributeValue_4() {if (mut_ptr != NULL)mut_ptr->unlock();}\
AttributeValue_4() {mut_ptr=NULL;}\
void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\
void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\
omni_mutex *get_attr_mutex() {return mut_ptr;}\
omni_mutex *mut_ptr;' tango.h
	sed -i '/typedef\ _CORBA_ConstrType_Variable_Var<AttributeValue_5>/a \/\/Added by Tango team\
virtual ~AttributeValue_5() {if (mut_ptr != NULL)mut_ptr->unlock();}\
AttributeValue_5() {mut_ptr=NULL;}\
void set_attr_mutex(omni_mutex *ptr) {mut_ptr=ptr;}\
void rel_attr_mutex() {if (mut_ptr != NULL){mut_ptr->unlock();mut_ptr=NULL;}}\
omni_mutex *get_attr_mutex() {return mut_ptr;}\
omni_mutex *mut_ptr;' tango.h
	cp tangoSK.cpp $(top_srcdir)/lib/cpp/server
	cp tangoDynSK.cpp $(top_srcdir)/lib/cpp/server
	cp tango.h $(top_srcdir)/lib/cpp/server/idl

idldir=${includedir}/tango/idl
idl_HEADERS=tango.h


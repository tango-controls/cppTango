//
// apiexcept.h - include file for TANGO device api exceptions
//
// 

#ifndef _APIEXCEPT_H
#define _APIEXCEPT_H

#include <tango.h>
#include <except.h>

using namespace std;

namespace Tango {


class ApiExcept
{
public:
	static inline void re_throw_exception(CORBA::SystemException &cex,
					      string &reason,
					      string &desc,
					      const char *origin,
		            		      Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(2);
	
		errors.length(2);

		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);		
		errors[0].desc = CORBA::string_dup(tmp);
		CORBA::string_free(tmp);
		
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup("API_CorbaException");
		errors[0].origin = CORBA::string_dup(origin);

		errors[1].desc = CORBA::string_dup(desc.c_str());
		errors[1].severity = sever;
		errors[1].reason = CORBA::string_dup(reason.c_str());
		errors[1].origin = CORBA::string_dup(origin);
			
		throw Tango::DevFailed(errors);
	}
	
	static inline void re_throw_exception(CORBA::SystemException &cex,
					      char *reason,
					      char *desc,
					      const char *origin,
		            		      Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(2);
	
		errors.length(2);

		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);				
		errors[0].desc = CORBA::string_dup(tmp);
		CORBA::string_free(tmp)
		;
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup("API_CorbaException");
		errors[0].origin = CORBA::string_dup(origin);

		errors[1].desc = CORBA::string_dup(desc);
		errors[1].severity = sever;
		errors[1].reason = CORBA::string_dup(reason);
		errors[1].origin = CORBA::string_dup(origin);
		
		delete[] reason;
		delete[] desc;
			
		throw Tango::DevFailed(errors);
	}
	
	static inline void re_throw_exception(CORBA::SystemException &cex,
					      const char *reason,
					      char *desc,
					      const char *origin,
		            		      Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(2);
	
		errors.length(2);
		
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);				
		errors[0].desc = CORBA::string_dup(tmp);
		CORBA::string_free(tmp);
		
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup("API_CorbaException");
		errors[0].origin = CORBA::string_dup(origin);

		errors[1].desc = CORBA::string_dup(desc);
		errors[1].severity = sever;
		errors[1].reason = CORBA::string_dup(reason);
		errors[1].origin = CORBA::string_dup(origin);
		
		delete[] desc;
			
		throw Tango::DevFailed(errors);
	}
	
	static inline void re_throw_exception(CORBA::SystemException &cex,
					      const char *reason,
					      string &desc,
					      const char *origin,
		            		      Tango::ErrSeverity sever = Tango::ERR)
	{
		Tango::DevErrorList errors(2);
	
		errors.length(2);
		
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);		
		errors[0].desc = CORBA::string_dup(tmp);
		CORBA::string_free(tmp);
		
		errors[0].severity = sever;
		errors[0].reason = CORBA::string_dup("API_CorbaException");
		errors[0].origin = CORBA::string_dup(origin);

		errors[1].desc = CORBA::string_dup(desc.c_str());
		errors[1].severity = sever;
		errors[1].reason = CORBA::string_dup(reason);
		errors[1].origin = CORBA::string_dup(origin);
					
		throw Tango::DevFailed(errors);
	}
};

#define MAKE_EXCEPT(E,I) \
class E: public Tango::DevFailed \
{ \
public: \
	E(const DevErrorList& err):DevFailed(err) {} \
}; \
class I \
{ \
public: \
	static inline void throw_exception(const char *reason,string &desc,const char *origin, \
					   Tango::ErrSeverity sever = Tango::ERR) \
	{\
		Tango::DevErrorList errors(1);\
		errors.length(1);\
		errors[0].desc = CORBA::string_dup(desc.c_str()); \
		errors[0].severity = sever; \
		errors[0].reason = CORBA::string_dup(reason);\
		errors[0].origin = CORBA::string_dup(origin);\
		throw Tango::E(errors);\
	}\
\
	static inline void throw_exception(const char *reason,char *desc,const char *origin,\
					   Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(1);\
		errors.length(1);\
		errors[0].desc = CORBA::string_dup(desc);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup(reason);\
		errors[0].origin = CORBA::string_dup(origin);\
		delete[] desc;\
		throw Tango::E(errors);\
	}\
\
	static inline void throw_exception(const char *reason,const char *desc,const char *origin,\
					   Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(1);\
		errors.length(1);\
		errors[0].desc = CORBA::string_dup(desc);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup(reason);\
		errors[0].origin = CORBA::string_dup(origin);\
		throw Tango::E(errors);\
	}\
\
	static inline void re_throw_exception(CORBA::SystemException &cex,\
					      string &reason,string &desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(2);\
		errors.length(2);\
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
		errors[0].desc = CORBA::string_dup(tmp);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup("API_CorbaException");\
		errors[0].origin = CORBA::string_dup(origin);\
		errors[1].desc = CORBA::string_dup(desc.c_str());\
		errors[1].severity = sever;\
		errors[1].reason = CORBA::string_dup(reason.c_str());\
		errors[1].origin = CORBA::string_dup(origin);\
		throw Tango::E(errors);\
	}\
\
	static inline void re_throw_exception(CORBA::SystemException &cex,\
					      char *reason,char *desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(2);\
		errors.length(2);\
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
		errors[0].desc = CORBA::string_dup(tmp);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup("API_CorbaException");\
		errors[0].origin = CORBA::string_dup(origin);\
		errors[1].desc = CORBA::string_dup(desc);\
		errors[1].severity = sever;\
		errors[1].reason = CORBA::string_dup(reason);\
		errors[1].origin = CORBA::string_dup(origin);\
		delete[] desc;\
		delete[] reason;\
		throw Tango::E(errors);\
	}\
	static inline void re_throw_exception(CORBA::SystemException &cex,\
					      const char *reason,string &desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(2);\
		errors.length(2);\
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
		errors[0].desc = CORBA::string_dup(tmp);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup("API_CorbaException");\
		errors[0].origin = CORBA::string_dup(origin);\
		errors[1].desc = CORBA::string_dup(desc.c_str());\
		errors[1].severity = sever;\
		errors[1].reason = CORBA::string_dup(reason);\
		errors[1].origin = CORBA::string_dup(origin);\
		throw Tango::E(errors);\
	}\
	static inline void re_throw_exception(CORBA::SystemException &cex,\
					      const char *reason,string &desc,\
					      string &origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(2);\
		errors.length(2);\
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
		errors[0].desc = CORBA::string_dup(tmp);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup("API_CorbaException");\
		errors[0].origin = CORBA::string_dup(origin.c_str());\
		errors[1].desc = CORBA::string_dup(desc.c_str());\
		errors[1].severity = sever;\
		errors[1].reason = CORBA::string_dup(reason);\
		errors[1].origin = CORBA::string_dup(origin.c_str());\
		throw Tango::E(errors);\
	}\
\
	static inline void re_throw_exception(CORBA::SystemException &cex,\
					      const char *reason,char *desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		Tango::DevErrorList errors(2);\
		errors.length(2);\
		char *tmp = Tango::Except::print_CORBA_SystemException(&cex);\
		errors[0].desc = CORBA::string_dup(tmp);\
		errors[0].severity = sever;\
		errors[0].reason = CORBA::string_dup("API_CorbaException");\
		errors[0].origin = CORBA::string_dup(origin);\
		errors[1].desc = CORBA::string_dup(desc);\
		errors[1].severity = sever;\
		errors[1].reason = CORBA::string_dup(reason);\
		errors[1].origin = CORBA::string_dup(origin);\
		delete[] desc;\
		throw Tango::E(errors);\
	}\
\
	static inline void re_throw_exception(Tango::E &ex,\
					      const char *reason,char *desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		long nb_err = ex.errors.length();\
		ex.errors.length(nb_err + 1);\
		ex.errors[nb_err].severity = sever;\
		ex.errors[nb_err].desc = CORBA::string_dup(desc);\
		delete[] desc;\
		ex.errors[nb_err].origin = CORBA::string_dup(origin);\
		ex.errors[nb_err].reason = CORBA::string_dup(reason);\
		throw ex;\
	}\
\
	static inline void re_throw_exception(Tango::E &ex,\
					      const char *reason,string &desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		long nb_err = ex.errors.length();\
		ex.errors.length(nb_err + 1);\
		ex.errors[nb_err].severity = sever;\
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());\
		ex.errors[nb_err].origin = CORBA::string_dup(origin);\
		ex.errors[nb_err].reason = CORBA::string_dup(reason);\
		throw ex;\
	}\
	static inline void re_throw_exception(Tango::DevFailed &ex,\
					      const char *reason,char *desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		long nb_err = ex.errors.length();\
		ex.errors.length(nb_err + 1);\
		ex.errors[nb_err].severity = sever;\
		ex.errors[nb_err].desc = CORBA::string_dup(desc);\
		delete[] desc;\
		ex.errors[nb_err].origin = CORBA::string_dup(origin);\
		ex.errors[nb_err].reason = CORBA::string_dup(reason);\
		throw ex;\
	}\
\
	static inline void re_throw_exception(Tango::DevFailed &ex,\
					      const char *reason,string &desc,\
					      const char *origin,\
					      Tango::ErrSeverity sever = Tango::ERR)\
	{\
		long nb_err = ex.errors.length();\
		ex.errors.length(nb_err + 1);\
		ex.errors[nb_err].severity = sever;\
		ex.errors[nb_err].desc = CORBA::string_dup(desc.c_str());\
		ex.errors[nb_err].origin = CORBA::string_dup(origin);\
		ex.errors[nb_err].reason = CORBA::string_dup(reason);\
		throw ex;\
	}\
};

MAKE_EXCEPT(ConnectionFailed,ApiConnExcept)
MAKE_EXCEPT(CommunicationFailed,ApiCommExcept)
MAKE_EXCEPT(WrongNameSyntax,ApiWrongNameExcept)
MAKE_EXCEPT(NonDbDevice,ApiNonDbExcept)
MAKE_EXCEPT(WrongData,ApiDataExcept)
MAKE_EXCEPT(NonSupportedFeature,ApiNonSuppExcept)


} // End of Tango namespace

#endif /* _APIEXCEPT_H */

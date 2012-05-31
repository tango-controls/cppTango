static const char *RcsId = "$Id$\n$Name$";

//+===========================================================================
//
// file :		class_factory.cpp
//
// description :	C++ source file for a dummy DServer::class_factory()
//			method. For server, this method is redefined by the
//			DS programmer in its own file and the linker will not
//			use this file. For client where ther is no
//			class_factory() method, this one will be used by the
//			linker
//			This works only if class_factory() is in its own file
//
// project :		TANGO
//
// author(s) :		A.Gotz + E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
// $Revision$
//
//-===========================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <dserver.h>

namespace Tango
{

//
// There is a trick to build Windows DLL and to use it in a server.
// To build a DLL, you have to pass your code into the linker. The linker
// try to resolve all symbols. Therefore, it resolves the DServer::class_factory
// with this default class_factory. For a server, it's annoying because we
// want the user DServer::class_factory to be called !!
// For all other kinds of lib (Unix type or Windows static), the linker is used
// only when the application is built and symbols are resolved first from the
// supplied object files and only if not found in object files with symbols
// defined in libraries.
// Therefore, the trick for DLL, is inside the default DServer::class_factory
// to force calling the user DServer::class_factory
// To do this, we use the GetProcAddress and we defines in dserver.h file
// the class_factory has "export".
// There is another trick to cast the pointer returned by GetProcAddress
// which is a pointer to function into a pointer to method.
// This is done using an union and initializing the pointeur to method
// with a local method and then modifying it
//

#ifdef _TG_WINDOWS_
typedef void (DServer::*PTR)(void);
typedef union _convertor
{
	PTR d;
	FARPROC s;
}convertor;
#endif


#ifdef __darwin__
#include <dlfcn.h>

typedef void (DServer::*PTR)(void);
typedef union _convertor
{
	PTR d;
	void *s;
}convertor;
#endif


void DServer::class_factory()
{
#ifdef _TG_WINDOWS_
	Tango::Util *tg = Tango::Util::instance();
	string exe_name = tg->get_ds_exec_name();
	exe_name = exe_name + ".exe";
	HMODULE mod;
	FARPROC proc;
	convertor conv;
	PTR tmp;

	if (tg->is_py_ds() == false)
	{
		if ((mod = GetModuleHandle(exe_name.c_str())) == NULL)
		{
			cerr << "Oups, no class defined in this server. Exiting ..." << endl;
			exit(-1);
		}
	}
	else
	{
		if ((mod = GetModuleHandle(TANGO_PY_MOD_NAME)) == NULL)
		{
			cerr << "Oups, no class defined in this server. Exiting ..." << endl;
			exit(-1);
		}
	}

//
// Use the mangled name to find the user DServer::class_factory method
//
// Due to the fact that on Windows 64 bits we have both _WIN32 and _WIN64
// defined, start by testing _WIN64 (See tango_config.h)
//

#ifdef _WIN64
	if ((proc = GetProcAddress(mod,"?class_factory@DServer@Tango@@AEAAXXZ")) == NULL)
#elif _WIN32 /* WIN32 */
	if ((proc = GetProcAddress(mod,"?class_factory@DServer@Tango@@AAEXXZ")) == NULL)
#endif
	{
		cerr << "Oups, no class defined in this server. Exiting ..." << endl;
		exit(-1);
	}
	else
	{
		conv.d = &DServer::stop_polling;
		conv.s = proc;

		tmp = conv.d;
		(this->*tmp)();
	}

#elif __darwin__
	Tango::Util *tg = Tango::Util::instance();
	string exe_name = tg->get_ds_exec_name();
	exe_name = exe_name;

	void *mod;
	void *proc;
	convertor conv;
	PTR tmp;

	if (tg->is_py_ds() == false)
	{
		if ((mod = dlopen (exe_name.c_str(), RTLD_LAZY )) == NULL)
		{
			cerr << "Oups, no class defined in this server. Exiting ..." << endl;
			exit(-1);
		}
	}
	else
	{
		/*
		if ((mod = GetModuleHandle(TANGO_PY_MOD_NAME)) == NULL)
		{
			cerr << "Oups, no class defined in this server. Exiting ..." << endl;
			exit(-1);
		}
		*/
	}

//
// Use the mangled name to find the user DServer::class_factory method
//
// Due to the fact that on Windows 64 bits we have both WIN32 and WIN64
// defined, start by testing WIN64 (See tango_config.h)
//

	if ((proc = dlsym (mod,"_ZN5Tango7DServer13class_factoryEv")) == NULL)
	{
		cerr << "error : " << dlerror() << endl;
		cerr << "Oups, no class defined in this server. Exiting ..." << endl;
		exit(-1);
	}
	else
	{
		conv.d = &DServer::stop_polling;
		conv.s = proc;

		tmp = conv.d;
		(this->*tmp)();
	}
#else
		cerr << "Oups, no class defined in this server. Exiting ..." << endl;
		exit(-1);
#endif

}

} // End of Tango namespace

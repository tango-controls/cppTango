//=============================================================================
//
// file :               cbthread.h
//
// description :        Include for the CallBackThread object.
//			This class implements the callback thread
//
// project :            TANGO
//
// author(s) :          E.Taurel
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
//=============================================================================

#ifndef _CBTHREAD_H
#define _CBTHREAD_H

#include <tango.h>


namespace Tango
{

class CbThreadCmd: public omni_mutex
{
public:
	CbThreadCmd():stop(false) {};
	void stop_thread() {omni_mutex_lock(*this);stop=true;}
	void start_thread() {omni_mutex_lock(*this);stop=false;}
	bool is_stopped() {omni_mutex_lock(*this);return stop;}

	bool stop;
};

//=============================================================================
//
//			The CallBackThread class
//
// description :	Class to store all the necessary information for the
//			polling thread. It's run() method is the thread code
//
//=============================================================================


class CallBackThread: public omni_thread
{
public:
	CallBackThread(CbThreadCmd &cmd,AsynReq *as):shared_cmd(cmd),
						     asyn_ptr(as) {};

	void *run_undetached(void *);
	void start() {start_undetached();}

	CbThreadCmd	&shared_cmd;
	AsynReq		*asyn_ptr;
};



} // End of Tango namespace

#endif /* _CBTHREAD_ */

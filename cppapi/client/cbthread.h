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
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011
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
// $Log$
// Revision 3.4  2010/09/09 13:43:38  taurel
// - Add year 2010 in Copyright notice
//
// Revision 3.3  2009/01/21 12:45:15  taurel
// - Change CopyRights for 2009
//
// Revision 3.2  2008/10/06 15:02:17  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 3.1  2008/10/02 16:09:25  taurel
// - Add some licensing information in each files...
//
// Revision 3.0  2003/03/25 16:30:45  taurel
// Change revision number to 3.0 before release 3.0.0 of Tango lib
//
// Revision 1.1  2003/03/20 08:56:12  taurel
// New file to support asynchronous calls
//
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

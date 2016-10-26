static const char *RcsId = "$Id$\n$Name$";

//+============================================================================
//
// file :               cbthread.cpp
//
// description :        C++ source code for the CallBackThread class.
//			This class is used in automatic callback mode.
//			The thread mainly waits on aa asynchronous callback
//			request and will call the get_asynch_replies() call
//			to fire the callback when the answer is received
//
// project :            TANGO
//
// author(s) :          E.Taurel
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
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
//-============================================================================

#if HAVE_CONFIG_H
#include <ac_config.h>
#endif

#include <tango.h>
#include <tango/client/cbthread.h>

namespace Tango {

    void CbThreadCmd::stop_thread() { this->stop.store(true); }

    void CbThreadCmd::start_thread() { this->stop.store(false); }

    bool CbThreadCmd::is_stopped() { return this->stop.load(); }

} // End of Tango namespace

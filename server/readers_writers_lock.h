// -*- Mode: C++; -*-
//
// ReadersWritersLock.h     Author    : Tristan Richardson (tjr)
//										Jens Meyer
//										Emmanuel Taurel
//
// Copyright (C) :      1997-1999 AT&T Laboratories Cambridge 
//						2004,2005,2006,2007,2008,2009,2010,2011
//						European Synchrotron Radiation Facility
//                      BP 220, Grenoble 38043
//                      FRANCE
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.


#ifndef _ReadersWritersLock_h_
#define _ReadersWritersLock_h_

#include <omnithread.h>

class ReadersWritersLock {

public:
  omni_mutex mut;
  omni_condition cond;
  int n;	// 0 means no-one active, > 0 means n readers, < 0 means writer
		// (-n times).
  int writerId;

  ReadersWritersLock(void) : cond(&mut), n(0), writerId(0), auto_self(NULL) {}

  void readerIn(void)
  {
    mut.lock();
	 
	 // In the case of usage with another threading library, omni_thread::self() might
	 // return a NULL pointer!
	 int threadId = 0;
	 omni_thread *th = omni_thread::self(); 
	 if ( th != NULL )
	 {
		threadId = th->id();
	 }
	 	 
     if ((n < 0) && (writerId == threadId))
	 {
      // this thread already has lock as writer, simply decrement n
      n--;
      mut.unlock();
      return;
     }
     while (n < 0)
       cond.wait();
     n++;
     mut.unlock();
  }

  void readerOut(void)
  {
    mut.lock();
    if (n < 0)
	{
      // this thread already had lock as writer, simply increment n
      n++;
      mut.unlock();
      return;
    }
    n--;
    if (n == 0)
      cond.signal();
    mut.unlock();
  }

  void writerIn(void)
  {
    mut.lock();
	 
	 // In the case of usage with another threading library, omni_thread::self() might
	 // return a NULL pointer! 
	 int threadId = 0;
	 omni_thread *th = omni_thread::self(); 
	 if ( th != NULL )
	 {
		threadId = th->id();
	 }			 
			 
     if ((n < 0) && (writerId == threadId))
	 {
      // this thread already has lock as writer, simply decrement n
      n--;
      mut.unlock();
      return;
     }
     while (n != 0)
       cond.wait();
    
	 n--;
	 
	 // Now the writer lock was taken.
	 // Make sure we get a correct thread ID
	 // With the class ensure_self it should return always a thread ID.
	 // Create the ensure_self object only for the thread which takes the writer lock!
	 if (th == NULL)
	 	auto_self = new omni_thread::ensure_self();
	 writerId  = omni_thread::self()->id();
	 	 
     mut.unlock();
  }

  void writerOut(void)
  {
    mut.lock();
    n++;
    if (n == 0)
	{
		// delete the dummy thread when it was created.
		if (auto_self != NULL)
		{
			delete auto_self;
			auto_self = NULL;
		}
      
		cond.broadcast();	// might as well wake up all readers
	}
    mut.unlock();
  }
  
private:
	// in the case of usage with another threading library, omni_thread::self() might
	// return a NULL pointer!
    // To avoid this problem we use the class ensure_self to get a dummy thread ID!
	//
	// The class ensure_self should be created on the stack. If created in
    // a thread without an associated omni_thread, it creates a dummy
    // thread which is released when the ensure_self object is deleted.

	 omni_thread::ensure_self	*auto_self;
};


//
// As an alternative to:
// {
//   lock.readerIn();
//   .....
//   lock.readerOut();
// }
//
// you can use a single instance of the ReaderLock class:
//
// {
//   ReaderLock r(lock);
//   ....
// }
//
// This has the advantage that lock.readerOut() will be called automatically
// when an exception is thrown.
//

class ReaderLock {
  ReadersWritersLock& rwl;
public:
  ReaderLock(ReadersWritersLock& l) : rwl(l) { rwl.readerIn(); }
  ~ReaderLock(void) { rwl.readerOut(); }
};


//
// Similarly the WriterLock class can be used instead of lock.writerIn() and
// lock.writerOut().
//

class WriterLock {
  ReadersWritersLock& rwl;
public:
  WriterLock(ReadersWritersLock& l) : rwl(l) { rwl.writerIn(); }
  ~WriterLock(void) { rwl.writerOut(); }
};

#endif

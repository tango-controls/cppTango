static const char *RcsId = "$Id$\n$Name$";

//============================================================================
//
// file :               eventqueue.cpp
//
// description :        Implementation of the event queue classes for
//                      all Tango events
//
// project :            TANGO
//
// author(s) :          J. Meyer
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010
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
// Revision 1.10  2009/03/20 11:53:28  taurel
// - Fix some compiler warnings
//
// Revision 1.9  2009/03/18 12:18:45  taurel
// - Fix warnings reported when compiled with the option -Wall
//
// Revision 1.8  2009/01/30 09:18:09  taurel
// - End of first implementation of Data Ready event
//
// Revision 1.7  2009/01/29 15:25:41  taurel
// - First implementation of the Data Ready event
//
// Revision 1.6  2009/01/21 12:49:03  taurel
// - Change CopyRights for 2009
//
// Revision 1.5  2008/10/06 15:01:09  taurel
// - Changed the licensing info from GPL to LGPL
//
// Revision 1.4  2008/10/03 06:52:31  taurel
// - Add some licensing info in each files
//
// Revision 1.3  2008/09/15 12:31:09  jensmeyer
// Added an eventqueue reading method to call the usual callback method
// when reading event data from the queue.
//
// Revision 1.2  2008/09/15 08:50:53  jensmeyer
// Corrections when attributes are not yet available during subscription.
//
// Revision 1.1  2008/09/15 07:24:08  jensmeyer
// Added event queues for event reception.
// To clean-up event.cpp two new files have been added
// evnetkeepalive.cpp and eventqueue.cpp.
//
// Revision 1.1  2008/07/25 13:40:23  meyer
// Initial revision
//
//
//============================================================================


#include <tango.h>
#include <event.h>


namespace Tango
{

////////////////////////////////////////////////////////////////////////////
// EventQueue class implementation
////////////////////////////////////////////////////////////////////////////


//+-------------------------------------------------------------------------
//
// method :         EventQueue::EventQueue
// 
// description :    Two constructors for the EventQueue class. 
//                  The first one does not take any argument and will
//                  allow unlimited event buffering.
//                  The second one creates a circular buffer with the 
//                  maximum size given as argument.
//
// argument : in :  - max_size : the maximum buffer size
//
//--------------------------------------------------------------------------

EventQueue::EventQueue()
{
	max_elt     = 0;
	insert_elt  = 0;
	nb_elt      = 0;
}

EventQueue::EventQueue(long max_size)
{
	if ( max_size == 0 )
		max_elt = 0;
	else
		max_elt = max_size;

	insert_elt  = 0;
	nb_elt      = 0;
}

//+-------------------------------------------------------------------------
//
// method :         EventQueue::EventQueue
// 
// description :    The class destructor. It frees all the memory allocated
//                  to store event data.
//
//--------------------------------------------------------------------------

EventQueue::~EventQueue()
{
	cout3 << "Entering EventQueue::~EventQueue nb_elt = " << nb_elt << endl;
	
	// lock the event queue
	omni_mutex_lock l(modification_mutex);
	
	long nb = nb_elt;
	
	// check whether the events are not attribute configuration events
	if (event_buffer.size() > 0 )
		{ 
		for (long i=0; i<nb; i++)
			delete event_buffer[i];
		
		event_buffer.clear();
		}
	// for attibute configuration events
	else
	{
		if (conf_event_buffer.size() > 0)
		{
			for (long i=0; i<nb; i++)
				delete conf_event_buffer[i];
		
			conf_event_buffer.clear();
		}
		else
		{
			for (long i=0; i<nb; i++)
				delete ready_event_buffer[i];
		
			ready_event_buffer.clear();
		}
	}
}

//+-------------------------------------------------------------------------
//
// method :         EventQueue::insert_event
// 
// description :    Insert a new event in the event queue
//
// argument : in :  - new_event : A pointer to the allocated event data
//                                structure.
//
//--------------------------------------------------------------------------

void EventQueue::insert_event (EventData *new_event)
{
	cout3 << "Entering EventQueue::insert_event" << endl;
	
	// lock the event queue
	omni_mutex_lock l(modification_mutex);
	
	//
	// Insert data in the event queue
	//

	// when no maximum queue size is given, just add the new event
	if ( max_elt == 0 )
		{
		event_buffer.push_back (new_event);
		}
		
	// when a maximum size s given, handle a circular buffer
	else
		{
		// allocate ring buffer when not yet done
		if ( event_buffer.size() == 0 )
			{
			event_buffer.resize (max_elt, NULL);
			}
			
		// free data when necessary
		if ( event_buffer[insert_elt] != NULL )
			{
			delete event_buffer[insert_elt];
			}
		
		// insert the event data pointer into the queue	
		event_buffer[insert_elt] = new_event;	
		}

		// Manage insert and read indexes
		inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method :         EventQueue::insert_event
// 
// description :    Insert a new attribute configuration event in the 
//                  event queue
//
// argument : in :  - new_event : A pointer to the allocated attribute 
//                                configuration event data structure.
//
//--------------------------------------------------------------------------

void EventQueue::insert_event (AttrConfEventData *new_event)
{
	cout3 << "Entering EventQueue::insert_event" << endl;

	// lock the event queue
	omni_mutex_lock l(modification_mutex);
	
	//
	// Insert data in the event queue
	//

	// when no maximum queue size is given, just add the new event
	if ( max_elt == 0 )
		{
		conf_event_buffer.push_back (new_event);
		}
		
	// when a maximum size s given, handle a circular buffer
	else
		{
		// allocate ring buffer when not yet done
		if ( conf_event_buffer.size() == 0 )
			{
			conf_event_buffer.resize (max_elt, NULL);
			}
			
		// free data when necessary
		if ( conf_event_buffer[insert_elt] != NULL )
			{
			delete conf_event_buffer[insert_elt];
			}
		
		// insert the event data pointer into the queue	
		conf_event_buffer[insert_elt] = new_event;	
		}

		// Manage insert and read indexes
		inc_indexes();
}

//+-------------------------------------------------------------------------
//
// method :         EventQueue::insert_event
// 
// description :    Insert a new attribute data ready event in the 
//                  event queue
//
// argument : in :  - new_event : A pointer to the allocated attribute 
//                                data ready event data structure.
//
//--------------------------------------------------------------------------

void EventQueue::insert_event (DataReadyEventData *new_event)
{
	cout3 << "Entering EventQueue::insert_event" << endl;

	// lock the event queue
	omni_mutex_lock l(modification_mutex);
	
	//
	// Insert data in the event queue
	//

	// when no maximum queue size is given, just add the new event
	if ( max_elt == 0 )
		{
		ready_event_buffer.push_back (new_event);
		}
		
	// when a maximum size s given, handle a circular buffer
	else
		{
		// allocate ring buffer when not yet done
		if ( ready_event_buffer.size() == 0 )
			{
			ready_event_buffer.resize (max_elt, NULL);
			}
			
		// free data when necessary
		if ( ready_event_buffer[insert_elt] != NULL )
			{
			delete ready_event_buffer[insert_elt];
			}
		
		// insert the event data pointer into the queue	
		ready_event_buffer[insert_elt] = new_event;	
		}

		// Manage insert and read indexes
		inc_indexes();
}


//+-------------------------------------------------------------------------
//
// method :         EventQueue::inc_indexes
// 
// description :    This private method increments the indexes used to acces
//                  the queue itself. This is necessary because the queue must
//                  be managed as a circular buffer
//
//--------------------------------------------------------------------------


void EventQueue::inc_indexes()
{
	if (max_elt == 0)
		{
		// unlimited buffer size
		insert_elt++;
		nb_elt++;
		}
	else
		{
		// circular buffer
		
		insert_elt++;
		if (insert_elt == max_elt)
			insert_elt = 0;
		
		if (nb_elt != max_elt)
			nb_elt++;
		}
}

//+-------------------------------------------------------------------------
//
// method :         EventQueue::size
// 
// description :    Returns the number of events stored in the event queue
//
//--------------------------------------------------------------------------
int EventQueue::size()
{
	// lock the event queue
	omni_mutex_lock l(modification_mutex);
	
	return nb_elt;
}


//+-------------------------------------------------------------------------
//
// method :         EventQueue::::get_last_event_date
// 
// description :    Returns the date of the last inserted, and not yet
//                  extracted event in the circular buffer
//
//--------------------------------------------------------------------------

TimeVal EventQueue::get_last_event_date()
{
	cout3 << "Entering EventQueue::get_last_insert_date" << endl;
	
	// lock the event queue
	omni_mutex_lock l(modification_mutex);
	
	if ( event_buffer.size() > 0 )
		{
		if (insert_elt == 0)
			{
			return event_buffer[max_elt - 1]->get_date();
			}
		else
			{
			return event_buffer[insert_elt - 1]->get_date();
			}
		}
	else
	{
		if ( conf_event_buffer.size() > 0 )
			{
			if (insert_elt == 0)
				{
				return conf_event_buffer[max_elt - 1]->get_date();
				}
			else
				{
				return conf_event_buffer[insert_elt - 1]->get_date();
				}
			}
		else
		{
			if ( ready_event_buffer.size() > 0 )
			{
				if (insert_elt == 0)
					return ready_event_buffer[max_elt - 1]->get_date();
				else
					return conf_event_buffer[insert_elt - 1]->get_date();
			}
			else
			{
				TangoSys_OMemStream o;
				o << "No new events available!\n";
				o << "Cannot return any event date" << ends;
				EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventQueue::get_last_event_date()");		
			}
		}
	}
	
	// Should never reach here. To make compiler happy
	
	struct TimeVal tv;
	tv.tv_sec = tv.tv_usec = tv.tv_nsec = 0;
	return tv; 
}


//-------------------------------------------------------------------------
//
// method :         EventQueue::get_events
// 
// description :    Return a vector with all events in the circular buffer.
//                  Events are kept in the buffer since the last extraction
//                  with get_events().
//                  After returning the event data, the circular buffer gets
//                  emptied! 
//
//--------------------------------------------------------------------------

void EventQueue::get_events(EventDataList &event_list)
{
	cout3 << "Entering EventQueue::get_events" << endl;

	// lock the event queue
	omni_mutex_lock l(modification_mutex);

	//
	// Set index to read the ring buffer and to initialise the vector 
	// of pointers to return.
	// In the returned sequence , indice 0 is the oldest data
	//
	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;
	
	long seq_index = nb_elt - 1;
	
	// prepare the vector to be returned
	
	event_list.clear();
	event_list.resize(nb_elt);
	
	//
	// Read buffer
	//			
	for (long i=0; i < nb_elt; i++)
	{
		event_list[seq_index] = event_buffer[index];
		
		// we do not want to free the event data when cleaning-up
		// the vector
		event_buffer[index] = NULL;
			
		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}	

	// empty the event queue now
	event_buffer.clear();
	insert_elt  = 0;
	nb_elt      = 0;
	
	cout3 << "EventQueue::get_events() : size = " << event_list.size() << endl;
	return;
}

//-------------------------------------------------------------------------
//
// method :         EventQueue::get_events
// 
// description :    Return a vector with all events in the circular buffer.
//                  Events are kept in the buffer since the last extraction
//                  with get_events().
//                  After returning the event data, the circular buffer gets
//                  emptied! 
//
//--------------------------------------------------------------------------

void EventQueue::get_events(AttrConfEventDataList &event_list)
{
	cout3 << "Entering EventQueue::get_events" << endl;

	// lock the event queue
	omni_mutex_lock l(modification_mutex);

	//
	// Set index to read the ring buffer and to initialise the vector 
	// of pointers to return.
	// In the returned sequence , indice 0 is the oldest data
	//
	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;
	
	long seq_index = nb_elt - 1;
	
	// prepare the vector to be returned
	
	event_list.clear();
	event_list.resize(nb_elt);
	
	//
	// Read buffer
	//			
	for (long i=0; i < nb_elt; i++)
	{
		event_list[seq_index] = conf_event_buffer[index];
		
		// we do not want to free the event data when cleaning-up
		// the vector
		conf_event_buffer[index] = NULL;
			
		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}	

	// empty the event queue now
	conf_event_buffer.clear();
	insert_elt  = 0;
	nb_elt      = 0;
	
	cout3 << "EventQueue::get_events() : size = " << event_list.size() << endl;
	
	return;
}


//-------------------------------------------------------------------------
//
// method :         EventQueue::get_events
// 
// description :    Return a vector with all events in the circular buffer.
//                  Events are kept in the buffer since the last extraction
//                  with get_events().
//                  After returning the event data, the circular buffer gets
//                  emptied! 
//
//--------------------------------------------------------------------------

void EventQueue::get_events(DataReadyEventDataList &event_list)
{
	cout3 << "Entering EventQueue::get_events" << endl;

	// lock the event queue
	omni_mutex_lock l(modification_mutex);

	//
	// Set index to read the ring buffer and to initialise the vector 
	// of pointers to return.
	// In the returned sequence , indice 0 is the oldest data
	//
	long index = insert_elt;
	if (index == 0)
		index = max_elt;
	index--;
	
	long seq_index = nb_elt - 1;
	
	// prepare the vector to be returned
	
	event_list.clear();
	event_list.resize(nb_elt);
	
	//
	// Read buffer
	//			
	for (long i=0; i < nb_elt; i++)
	{
		event_list[seq_index] = ready_event_buffer[index];
		
		// we do not want to free the event data when cleaning-up
		// the vector
		ready_event_buffer[index] = NULL;
			
		if (index == 0)
			index = max_elt;
		index--;
		seq_index--;
	}	

	// empty the event queue now
	ready_event_buffer.clear();
	insert_elt  = 0;
	nb_elt      = 0;
	
	cout3 << "EventQueue::get_events() : size = " << event_list.size() << endl;
	
	return;
}


//-------------------------------------------------------------------------
//
// method :         EventQueue::get_events
// 
// description :    Call the callback method for all events in the 
//                  circular buffer.
//                  Events are kept in the buffer since the last extraction
//                  with get_events().
//                  After returning the event data, the circular buffer gets
//                  emptied! 
//
//--------------------------------------------------------------------------

void EventQueue::get_events(CallBack *cb)
{
	cout3 << "Entering EventQueue::get_events" << endl;

	if ( cb == NULL )
		{
		TangoSys_OMemStream o;
		o << "No callback object given!\n";
		o << "Cannot return any event data" << ends;
		EventSystemExcept::throw_exception((const char *)"API_EventQueues",
				        o.str(),
				        (const char *)"EventQueue::get_events()");
		}
	
	// Check the event type
	
	if ( event_buffer.size() > 0 )
	{
		// Get event data for a local data copy.
		// The event reception should not be blocked in
		// case of a problem in the callback method!
		EventDataList event_list;
		get_events (event_list);
		
		// Loop over all events
		EventDataList::iterator vpos;
		for (vpos=event_list.begin(); vpos!=event_list.end(); vpos++)
		{
			// call the callback method
			try
			{
				cb->push_event(*vpos);
			}
			catch (...)
			{
				cerr << "Tango::EventQueue::get_events() exception in callback method \nfor attribute " << 
			            (*vpos)->attr_name << "with event type " << (*vpos)->event << endl;
			}
		}
	}
	else if ( conf_event_buffer.size() > 0 )
	{
		// Get event data for a local data copy.
		// The event reception should not be blocked in
		// case of a problem in the callback method!
		AttrConfEventDataList attr_conf_event_list;
		get_events (attr_conf_event_list);
		
		// Loop over all events
		AttrConfEventDataList::iterator vpos;
		for (vpos=attr_conf_event_list.begin(); vpos!=attr_conf_event_list.end(); vpos++)
		{
			// call the callback method
			try
			{
				cb->push_event(*vpos);
			}
			catch (...)
			{
				cerr << "Tango::EventQueue::get_events() exception in callback method \nfor attribute " << 
			            (*vpos)->attr_name << "with event type " << (*vpos)->event << endl;
			}
		}
	
	}
	else
	{
		// Get event data for a local data copy.
		// The event reception should not be blocked in
		// case of a problem in the callback method!
		DataReadyEventDataList d_ready_event_list;
		get_events (d_ready_event_list);
		
		// Loop over all events
		DataReadyEventDataList::iterator vpos;
		for (vpos=d_ready_event_list.begin(); vpos!=d_ready_event_list.end(); vpos++)
		{
			// call the callback method
			try
			{
				cb->push_event(*vpos);
			}
			catch (...)
			{
				cerr << "Tango::EventQueue::get_events() exception in callback method \nfor attribute " << 
			            (*vpos)->attr_name << "with event type " << (*vpos)->event << endl;
			}
		}
	
	}
}



} // End of Tango namespace

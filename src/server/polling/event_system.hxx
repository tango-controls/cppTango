//
// Created by ingvord on 11/2/16.
//

#pragma once

#include <string>
#include <tango.h>
#include <tango/server/eventsupplier.h>

namespace Tango {
    class DeviceImpl;
    class WorkItem;
    class DevFailed;
    class EventSupplier;

    namespace polling {
        //TODO generalize and move into dedicated package
        class EventSystem {
            EventSupplier* event_supplier_nd_;
            EventSupplier* event_supplier_zmq_;

            //TODO public?
            //
            // Eventually push the event (if detected). When we have both notifd and zmq event supplier, do not detect the event
            // two times. The detect_and_push_events() method returns true if the event is detected.
            //
            void push_event(DeviceImpl*, struct EventSupplier::SuppliedEventData&&, DevFailed*, std::string&&, struct timeval *);
        public:
            /**
             * Notifd, ZMQ
             *
             * @constructor
             */
            EventSystem(EventSupplier *, EventSupplier *);

            void push_event(WorkItem&);
            //+---------------------------------------------------------------------------------------------------------------
            //
            // method :
            //		PollThread::push_error_event
            //
            // description :
            //		To force one event if the polling thread has discarded one work item because it is late
            //
            // args :
            //		in :
            // 			- to_do : The work item
            //
            //----------------------------------------------------------------------------------------------------------------
            void push_error_event(WorkItem&, DevFailed&);
        };
    }//polling
}//Tango



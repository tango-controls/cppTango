//
// Created by ingvord on 11/2/16.
//

#pragma once


namespace Tango {
    class WorkItem;
    class DevFailed;
    class EventSupplier;

    namespace polling {
        //TODO generalize and move into dedicated package
        class EventSystem {
            EventSupplier* event_supplier_nd_;
            EventSupplier* event_supplier_zmq_;
        public:
            /**
             * Notifd, ZMQ
             *
             * @return
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



//
// Created by ingvord on 10/31/16.
//

#pragma once

#include <list>
#include <condition_variable>

namespace Tango {

    namespace threading {
        template <typename Item>
        /**
         * This queue must be used in cases with single consumer multiple producers
         *
         *
         *
         */
        class asymmetric_unbound_blocking_queue {
            std::list<Item> queue_;
            std::condition_variable monitor_;
            std::mutex queue_guard_;
        public:
            typedef typename std::list<Item>::size_type size_type;

            asymmetric_unbound_blocking_queue();

            void push(Item&&);
            Item pop();

            //TODO swap - grab all items and empty queue
        };
    }//threading

}//Tango



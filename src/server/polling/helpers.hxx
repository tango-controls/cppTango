//
// Created by ingvord on 11/1/16.
//

#pragma once

#include <experimental/optional>
#include <list>
#include <chrono>
#include <sstream>
#include <iterator>

namespace Tango {

    template <typename Sequence, typename Item = typename Sequence::value_type>
    std::string vector_to_string(Sequence sequence){
        std::ostringstream oss;

        if (!sequence.empty())
        {
            // Convert all but the last element to avoid a trailing ","
            std::copy(sequence.begin(), sequence.end()-1,
                      std::ostream_iterator<Item>(oss, ","));

            // Now add the last element with no delimiter
            oss << sequence.back();
        }

        return oss.str();
    }

    class PollObj;

    class WorkItem;
    namespace polling {
        class Command;
        struct timeval time_diff(struct timeval before, struct timeval after_t);

        template<typename Duration>
        struct timeval to_timeval(Duration&& d) {
            std::chrono::seconds const sec = std::chrono::duration_cast<std::chrono::seconds>(d);
            struct timeval tv{};
            tv.tv_sec  = sec.count();
            tv.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count();
            return tv;
        }

    }//polling
}//Tango
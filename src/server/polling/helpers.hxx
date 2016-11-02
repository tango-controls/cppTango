//
// Created by ingvord on 11/1/16.
//

#pragma once

#include <chrono>
#include <sstream>
#include <iterator>
#include <string>

namespace Tango {

    template <typename Sequence, typename Item = typename Sequence::value_type>
    std::string sequence_to_string(Sequence sequence){
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

    namespace polling {
        template<typename Duration>
        struct timeval duration_to_timeval(Duration &&d) {
            std::chrono::seconds const sec = std::chrono::duration_cast<std::chrono::seconds>(d);
            struct timeval tv{};
            tv.tv_sec  = sec.count();
            tv.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count();
            return tv;
        }

    }//polling
}//Tango
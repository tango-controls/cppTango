//
// Created by ingvord on 11/1/16.
//

#include "helpers.hxx"
#include "command.hxx"

using namespace std;

using namespace Tango;
//Should be in PollThread but kept here due to std=c++1y


struct timeval Tango::polling::time_diff(struct timeval before,
                                         struct timeval after_t) {

    double bef_d = (double) before.tv_sec + ((double) before.tv_usec / 1000000);
    double aft_d = (double) after_t.tv_sec + ((double) after_t.tv_usec / 1000000);
    double diff_d = aft_d - bef_d;

    struct timeval result{};
    result.tv_sec = (long) diff_d;
    result.tv_usec = (long) ((diff_d - result.tv_sec) * 1000000);
    return result;
}
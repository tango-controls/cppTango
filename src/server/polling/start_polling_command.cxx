//
// Created by ingvord on 11/1/16.
//

#include "start_polling_command.hxx"

Tango::polling::StartPollingCommand::StartPollingCommand() : Command(
        nullptr, POLL_START, "", POLL_ATTR, -1, -1) {}

void Tango::polling::StartPollingCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::StartPollingCommand::execute(PollThread &poll_thread) {
    cout4 << "Execute a Start poll command" << endl;
    poll_thread->polling_stop.store(false);
    //TODO
    poll_thread->polling_future_ = async(launch::async,std::bind([](PollThread *poll_thread) {
        while (!poll_thread->polling_stop) {
            long sleep = poll_thread->sleep.load();
            cout3 << "Sleep for: " << sleep << endl;
            this_thread::sleep_for(chrono::milliseconds{sleep});//TODO use conditional variable


#ifdef _TG_WINDOWS_
            _ftime(&now_win);
                now.tv_sec = (unsigned long)now_win.time;
                now.tv_usec = (long)now_win.millitm * 1000;
#else
            gettimeofday(&poll_thread->now, NULL);
#endif
            poll_thread->now.tv_sec = poll_thread->now.tv_sec - DELTA_T;

            poll_thread->one_more_poll();

#ifdef _TG_WINDOWS_
            _ftime(&after_win);
                after.tv_sec = (unsigned long)after_win.time;
                after.tv_usec = (long)after_win.millitm * 1000;
#else
            gettimeofday(&poll_thread->after, NULL);
#endif
            poll_thread->after.tv_sec = poll_thread->after.tv_sec - DELTA_T;

            if (poll_thread->tune_ctr <= 0) {
                poll_thread->tune_list(true, 0);
                if (poll_thread->need_two_tuning == true) {
                    unsigned long nb_works = poll_thread->works.size();
                    poll_thread->tune_ctr = (nb_works << 2);
                    poll_thread->need_two_tuning = false;
                } else
                    poll_thread->tune_ctr = POLL_LOOP_NB;
            }

            poll_thread->compute_sleep_time();
        }
    }, poll_thread));
}

Tango::polling::StartPollingCommand::operator std::string() {
    return "StartPollingCommand";
}

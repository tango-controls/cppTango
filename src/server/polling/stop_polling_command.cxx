//
// Created by ingvord on 11/1/16.
//

#include "stop_polling_command.hxx"

Tango::polling::StopPollingCommand::StopPollingCommand() : Command(
        nullptr, POLL_STOP, "", POLL_ATTR, -1, 0) {}

void Tango::polling::StopPollingCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::StopPollingCommand::execute(PollThread &poll_thread) {
    poll_thread.polling_stop.store(true);
}

Tango::polling::StopPollingCommand::operator std::string() {
    return "StopPollingCommand";
}

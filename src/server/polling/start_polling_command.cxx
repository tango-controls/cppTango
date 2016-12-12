//
// Created by ingvord on 11/1/16.
//

#include "start_polling_command.hxx"
#include "polling_thread.hxx"

Tango::polling::StartPollingCommand::StartPollingCommand() : Command(
        nullptr, POLL_START, "", POLL_ATTR, -1, -1) {}

void Tango::polling::StartPollingCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::StartPollingCommand::execute(PollThread &poll_engine) {
    cout4 << "Execute a Start poll command" << endl;
    poll_engine.start_polling();
}

Tango::polling::StartPollingCommand::operator std::string() {
    return "StartPollingCommand";
}

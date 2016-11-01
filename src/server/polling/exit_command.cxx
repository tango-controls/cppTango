//
// Created by ingvord on 11/1/16.
//

#include "exit_command.hxx"

Tango::polling::ExitCommand::ExitCommand() : Command(nullptr, POLL_EXIT, "",
                                                     POLL_ATTR, -1,
                                                     -1) {}

void Tango::polling::ExitCommand::operator()(PollThread &poll_thread) {
    return execute(poll_thread);
}

void Tango::polling::ExitCommand::execute(PollThread &poll_thread) {
    //TODO interrupt and kill polling thread
}

Tango::polling::ExitCommand::operator std::string() {
    return "ExitCommand";
}

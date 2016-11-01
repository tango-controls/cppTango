//
// Created by ingvord on 11/1/16.
//

#pragma once

#include <experimental/optional>
#include <list>

namespace Tango {
    class PollObj;

    class WorkItem;
    namespace polling {
        class Command;


        std::experimental::optional<WorkItem>
        find_work_item(bool, const Command &, /*TODO const*/ PollObj &, const std::list <WorkItem> &);
    }//polling
}//Tango
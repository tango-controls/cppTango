#ifndef _EVENT_SUBSCRIPTION_STATE_H
#define _EVENT_SUBSCRIPTION_STATE_H

#include <map>
#include <vector>
#include <string>

namespace Tango
{

using EventClientLibVersion = int;
using EventClientLibVersions = std::vector<EventClientLibVersion>;

struct AttributeEventSubscriptionState
{
    std::string attribute_name;

    EventClientLibVersions change_event_clients = {};
    EventClientLibVersions archive_event_clients = {};
    EventClientLibVersions periodic_event_clients = {};
    EventClientLibVersions user_event_clients = {};
    EventClientLibVersions attr_conf_event_clients = {};

    bool has_quality_event_clients = false;
    bool has_data_ready_event_clients = false;

    bool has_zmq_event_channel = false;
    bool has_notifd_event_channel = false;
};

using AttributeEventSubscriptionStates = std::vector<AttributeEventSubscriptionState>;

struct DeviceEventSubscriptionState
{
    bool has_dev_intr_change_event_clients;

    AttributeEventSubscriptionStates attribute_events;
};

using ServerEventSubscriptionState = std::map<std::string, DeviceEventSubscriptionState>;
}

#endif

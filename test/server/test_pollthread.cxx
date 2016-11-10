//
// Created by ingvord on 11/9/16.
//

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <tango.h>
#include "server/polling/polling_thread.hxx"
#include "server/polling/polling_queue.hxx"
#include "server/polling/event_system.hxx"


using namespace std;
using namespace cgreen;

using namespace Tango;

static string device_name{"test/device_impl/mock"};

struct DeviceImplMock : public DeviceImpl {
    DeviceImplMock(DeviceClass *device_class, string &dev_name) : DeviceImpl(device_class, dev_name) {}

    void init_device() override {
        cout << "Init mock device..." << endl;
    }

    virtual long get_dev_idl_version() override {
        return 5;
    }
};

static DeviceImpl* mock_device;

Describe(PollThread);
BeforeEach(PollThread) {
    mock_device = new DeviceImplMock(nullptr, device_name);

}
AfterEach(PollThread) {
    delete mock_device;
}

Ensure(PollThread, test_find_work_item) {


    PollThread instance{"DummyPollThread", false, nullptr, nullptr};

    WorkItem item1{
            mock_device,
            nullptr,
            chrono::milliseconds{1000},
            chrono::milliseconds{100},
            PollObjType::POLL_ATTR,
            {1,"test_attr_1"}
    };
    instance.add_work_item(item1);

    auto result = instance.find_work_item(mock_device, PollObjType::POLL_ATTR, chrono::milliseconds{100}, std::string());

    assert_true(result.operator bool());
}


int main(int argc, char **argv) {

    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, PollThread, test_find_work_item);

    if (argc > 1) {
        return run_single_test(suite, argv[1], create_text_reporter());
    }
    return run_test_suite(suite, create_text_reporter());
}


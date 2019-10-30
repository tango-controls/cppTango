#include <tango.h>

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

namespace Tango
{

template <typename TEvent>
class CallbackMock : public Tango::CallBack
{
public:

    using Event = TEvent;

    void push_event(TEvent* event)
    {
        {
            std::unique_lock<std::mutex> lk(m);
            events.emplace_back(new Event(*event));
        }

        cv.notify_one();
    }

    void assert_called(int timeout_millis)
    {
        assert_called(timeout_millis, [](const Event&){});
    }

    template <typename Function>
    void assert_called(int timeout_millis, Function&& func)
    {
        std::unique_lock<std::mutex> lk(m);

        if (not events.empty())
        {
            verify_first_event(std::forward<Function>(func));
        }
        else
        {
            if (cv.wait_for(
                lk,
                std::chrono::milliseconds(timeout_millis),
                [this]{ return not events.empty(); }))
            {
                verify_first_event(std::forward<Function>(func));
            }
            else
            {
                assert(false);
            }
        }
    }

    void assert_not_called()
    {
        std::unique_lock<std::mutex> lk(m);
        assert(events.empty());
    }

    std::size_t get_call_count()
    {
        std::unique_lock<std::mutex> lk(m);
        return events.size();
    }

private:

    template <typename Func>
    void verify_first_event(Func&& func)
    {
        func(*events.front());
        events.pop_front();
    }

    std::deque<std::unique_ptr<TEvent>> events;
    std::mutex m;
    std::condition_variable cv{};
};

}

//
// Created by ingvord on 11/9/16.
//

#include <cgreen/cgreen.h>
#include <functional>
#include <experimental/optional>
#include <algorithm>
#include <iostream>
#include "server/threading/blocking_priority_queue.hxx"

using namespace cgreen;
using namespace std;
namespace threading = Tango::threading;
using namespace threading;

struct Item {
    int id;
    string value;
};

bool item_comparator(const Item& lhs, const Item& rhs){
    return lhs.id < rhs.id;
}

Describe(BlockingQeue);
BeforeEach(BlockingQeue) {

}
AfterEach(BlockingQeue) {
}

Ensure(BlockingQeue, test_push) {
    blocking_priority_queue<Item> instance{&item_comparator};


    instance.push({0, "Hello"});
    instance.push({3, "!!!"});
    instance.push({2, "World"});
    instance.push({1, "  "});

    assert_equal(instance.size(), 4);
}

Ensure(BlockingQeue, test_pop) {
    blocking_priority_queue<Item> instance{&item_comparator};


    instance.push({0, "Hello"});
    instance.push({3, "!!!"});
    instance.push({2, "World"});
    instance.push({1, "  "});

    auto one = instance.pop();
    auto two = instance.pop();

    assert_equal(instance.size(), 2);

    assert_string_equal(one.value.c_str(), "!!!");
    assert_string_equal(two.value.c_str(), "World");
}

Ensure(BlockingQeue, test_inherite_find_if) {
    struct InheritedQueue : public blocking_priority_queue<Item>{
        InheritedQueue() : blocking_priority_queue(&item_comparator) {}

        experimental::optional<Item> find_if(function<bool(const Item&)> predicate) {
            Lock lock{queue_guard_};
            auto it = std::remove_if(c.begin(), c.end(), predicate);

            if (it == c.end()) {
                return experimental::optional<Item>{};
            }

            auto result = experimental::make_optional(*it);
            c.erase(it, c.end());
            return result;
        }


        void for_each(function<void(const Item &)> f) {
            Lock lock{queue_guard_};
            std::for_each(c.begin(), c.end(), f);
        }
    };

    InheritedQueue instance{};


    instance.push({0, "Hello"});
    instance.push({3, "!!!"});
    instance.push({2, "World"});
    instance.push({1, "  "});
    instance.push({4, "World"});



    auto result = instance.find_if([](const Item& item){ return item.value == "World";});

    auto one = instance.pop();
    instance.push({10, "Hi!"});
    auto two = instance.pop();

    assert_true(result.operator bool());
    assert_equal(instance.size(), 2);
    assert_string_equal(result->value.c_str(), "World");

    assert_string_equal(one.value.c_str(), "!!!");
    assert_string_equal(two.value.c_str(), "Hi!");
    instance.for_each([](const Item& item){
        cout <<  item.id << ":" << item.value << endl;
    });
}

int main(int argc, char **argv) {

    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, BlockingQeue, test_push);
    add_test_with_context(suite, BlockingQeue, test_pop);
    add_test_with_context(suite, BlockingQeue, test_inherite_find_if);
    if (argc > 1) {
        return run_single_test(suite, argv[1], create_text_reporter());
    }
    return run_test_suite(suite, create_text_reporter());
}
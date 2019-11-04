#include <zmq.hpp>

int main(int, char**)
{
  zmq::context_t c(1);
  zmq::socket_t s(c, ZMQ_REQ);
  s.disconnect("some endpoint");
}

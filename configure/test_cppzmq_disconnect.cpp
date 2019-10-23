#include <zmq.hpp>

int main(int, char**)
{
  zmq::context_t c;
  zmq::socket_t s(c, ZMQ_REQ);
  s.disconnect("some endpoint");
}

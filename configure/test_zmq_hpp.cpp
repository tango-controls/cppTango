#include <zmq.hpp>

int main(int argc, char** argv)
{
  zmq::context_t c;
  zmq::socket_t s(c, ZMQ_REQ);

  // we require `operator void*()`
  void *ptr = s;
}

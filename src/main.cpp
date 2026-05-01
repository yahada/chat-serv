#include "../include/include.hpp"
#include "server.hpp"
int main() {
  using namespace chat;
  io::io_context io_context;
  Server srv(io_context, 15001);
  srv.async_accept();
  io_context.run();
  return 0;
}
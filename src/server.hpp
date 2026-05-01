#ifndef SERVER_HPP
#define SERVER_HPP
#include "../include/include.hpp"
#include "session.hpp"

namespace chat
{
  class Server {
  public:
    Server(io::io_context& io_context, std::uint16_t port);
    void async_accept();
    void post(const std::string& message);
    void send(const std::string& message, std::shared_ptr< Session > session);
    void showUsers(std::shared_ptr< Session > session);
  private:
    io::io_context& io_context;
    tcp::acceptor acceptor;
    std::optional<tcp::socket> socket;
    std::unordered_set< std::shared_ptr< Session > > clients;
  };
}

#endif

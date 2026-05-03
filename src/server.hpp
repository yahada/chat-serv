#ifndef SERVER_HPP
#define SERVER_HPP
#include "../include/include.hpp"
#include "session.hpp"

namespace chat
{
  class Server {
  public:
    Server(io::io_context& io_context, const std::uint16_t& port);
    void async_accept();
    void post(std::shared_ptr< Session > session, const std::string& message);
    void send(const std::string& message, std::shared_ptr< Session > session);
    void showUsers(std::shared_ptr< Session > session);
    void executeFunc(std::shared_ptr< Session > session, std::string line);
  private:
    std::vector< std::string > splitWhitespace(const std::string& msg);
    io::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::unordered_set< std::shared_ptr< Session > > clients_;
  };
}

#endif

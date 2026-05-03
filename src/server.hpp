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
    void informationPost(const std::string& message);
    void send(std::shared_ptr< Session > session, const std::string& message);
    void showUsers(std::shared_ptr< Session > session);
    void executeFunc(std::shared_ptr< Session > session, std::string line);
  private:
    std::vector< std::string > splitWhitespace(const std::string& msg);
    std::string getStrParams(const std::vector< std::string >& params, size_t start);
    io::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::unordered_set< std::shared_ptr< Session > > clients_;
  };
}

#endif

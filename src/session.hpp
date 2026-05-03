#ifndef SESSION_HPP
#define SESSION_HPP
#include "../include/include.hpp"

namespace chat
{
  class Session: public std::enable_shared_from_this< Session > {
  public:
    Session(tcp::socket&& socket);
    void post(const std::string& message);
    void start(message_handler&& on_message, error_handler&& on_error);
    tcp::endpoint id(error_code error);
  private:
    void async_read();
    void on_read(error_code error, std::size_t bytes_transferred);
    void async_write();
    void on_write(error_code error, size_t);

    tcp::socket socket_;
    io::streambuf streambuf_;
    std::queue< std::string > outgoing_;
    message_handler on_message_;
    error_handler on_error_;
  };
}
#endif
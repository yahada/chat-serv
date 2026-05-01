#include "session.hpp"

chat::Session::Session(tcp::socket&& socket):
  socket(std::move(socket))
{}

void chat::Session::post(const std::string& message)
{
  bool idle = outgoing.empty();
  outgoing.push(message);

  if (idle)
  {
    async_write();
  }
}

void chat::Session::start(message_handler &&on_message, error_handler &&on_error)
{
  on_message = std::move(on_message);
  on_error = std::move(on_error);
  async_read();
}

void chat::Session::async_read()
{
  io::async_read_until(socket, streambuf, "\n", std::bind(&Session::on_read, shared_from_this(), _1, _2));
}

void chat::Session::on_read(error_code error, std::size_t bytes_transferred)
{
  if (!error)
  {
    std::stringstream message;
    message << socket.remote_endpoint(error) << ": " << std::istream(&streambuf).rdbuf();
    streambuf.consume(bytes_transferred);
    on_message(message.str());
    async_read();
  }
  else
  {
    socket.close(error);
    on_error();
  }
}

void chat::Session::async_write()
{
  io::async_write(socket, io::buffer(outgoing.front()), std::bind(&Session::on_write, shared_from_this(), _1, _2));
}

void chat::Session::on_write(error_code error, std::size_t bytes_transferred)
{
  if (!error)
  {
    outgoing.pop();

    if (!outgoing.empty())
    {
      async_write();
    }
  }
  else
  {
    socket.close(error);
    on_error();
  }
}

tcp::endpoint chat::Session::id()
{
  return socket.remote_endpoint();
}

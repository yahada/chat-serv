#include "session.hpp"
using std::placeholders::_1;
using std::placeholders::_2;

chat::Session::Session(tcp::socket&& socket):
  socket_(std::move(socket))
{}

void chat::Session::informationMsg(const std::string& msg)
{
  bool idle = outgoing_.empty();
  outgoing_.push(msg + "\n");
  if (idle)
  {
    async_write();
  }
}

void chat::Session::post(std::shared_ptr< Session > session, const std::string& msg)
{
  bool idle = outgoing_.empty();
  std::stringstream message;
  error_code error;
  message << session->id(error) << ": " << msg << '\n';
  outgoing_.push(message.str());

  if (idle)
  {
    async_write();
  }
}

void chat::Session::start(message_handler &&on_message, error_handler &&on_error)
{
  on_message_ = std::move(on_message);
  on_error_ = std::move(on_error);
  async_read();
}

void chat::Session::async_read()
{
  auto self = shared_from_this();

  boost::asio::async_read_until(
    socket_,
    streambuf_,
    "\n",
    [this, self](error_code ec, size_t bytes)
    {
      on_read(ec, bytes);
    }
  );
}

void chat::Session::async_write()
{
  auto self = shared_from_this();

  boost::asio::async_write(
    socket_,
    boost::asio::buffer(outgoing_.front()),
    [this, self](error_code ec, size_t bytes)
    {
      on_write(ec, bytes);
    }
  );
}
void chat::Session::on_read(error_code error, size_t bytes_transferred)
{
  if (!error)
  {
    std::stringstream message;
    message << socket_.remote_endpoint(error) << ": " << std::istream(&streambuf_).rdbuf() << '\n';
    streambuf_.consume(bytes_transferred);
    on_message_(message.str());
    async_read();
  }
  else
  {
    socket_.close(error);
    on_error_();
  }
}

void chat::Session::on_write(error_code error, size_t)
{
  if (!error)
  {
    outgoing_.pop();

    if (!outgoing_.empty())
    {
      async_write();
    }
  }
  else
  {
    socket_.close(error);
    on_error_();
  }
}

tcp::endpoint chat::Session::id(error_code error)
{
  return socket_.remote_endpoint(error);
}

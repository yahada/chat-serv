#include "server.hpp"
using std::placeholders::_1;

chat::Server::Server(io::io_context& io_context, std::uint16_t port):
  io_context_(io_context),
  acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{}

void chat::Server::async_accept()
{
  acceptor_.async_accept([this](error_code ec, tcp::socket socket) {
    if (ec)
    {
      async_accept();
      return;
    }

    auto client = std::make_shared<Session>(std::move(socket));
    std::stringstream message;
    error_code error_id;
    message << "[Welcome to chat, " << client->id(error_id) << "]\n\r";
    client->post(message.str());
    post("[We have a newcomer]\n\r");

    clients_.insert(client);

    client->start(
    [this](const std::string& msg)
    {
      post(msg);
    },
    [this, weak = std::weak_ptr<Session>(client)]
    {
      if (auto shared = weak.lock(); shared && clients_.erase(shared))
      {
        post("[We are one less]\n\r");
      }
    });
    async_accept();
  });
}

void chat::Server::post(const std::string& message)
{
  for (auto& client : clients_)
  {
    client->post(message);
  }
}

void chat::Server::showUsers(std::shared_ptr< Session > session)
{
  session->post("[Users]\n");
  size_t i = 1;
  for (const auto& client: clients_)
  {
    std::stringstream message;
    error_code error_msg;
    message << std::to_string(i) << ". " << client->id(error_msg) << "\n";
    session->post(message.str());
  }
}


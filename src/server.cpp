#include "server.hpp"
#include <iostream>

chat::Server::Server(io::io_context& io_context, std::uint16_t port):
  io_context(io_context),
  acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{}

void chat::Server::async_accept()
{
  socket.emplace(io_context);
  std::cout << socket->remote_endpoint() << '\n';
  acceptor.async_accept(*socket, [&](error_code error) {
    auto client = std::make_shared< Session>(std::move(*socket));
    std::stringstream message;
    message << "[Welcome to chat, " << client->id() << "\n\r";
    client->post(message.str());
    post("[We have a newcomer]\n\r");

    clients.insert(client);

    client->start(
    std::bind(&Server::post, this, _1),
    [&, weak = std::weak_ptr(client)] {
    if (auto shared = weak.lock(); shared && clients.erase(shared))
    {
      post("[We are one less]\n\r");
    }
    });
    async_accept();
  });
}

void chat::Server::post(const std::string& message)
{
  for (auto& client : clients)
  {
    client->post(message);
  }
}

void chat::Server::showUsers(std::shared_ptr< Session > session)
{
  session->post("[Users]\n");
  size_t i = 1;
  for (const auto& client: clients)
  {
    std::stringstream message;
    message << std::to_string(i) << ". " << session->id() << "\n";
    session->post(message.str());
  }
}


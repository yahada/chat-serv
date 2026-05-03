#include "server.hpp"
#include <unordered_map>
using std::placeholders::_1;

chat::Server::Server(io::io_context& io_context, const std::uint16_t& port):
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
    std::stringstream id;
    error_code error_id;
    id << client->id(error_id).port();
    client->informationMsg("[Welcome to chat, User" + id.str() + "]");
    informationPost("[User" + id.str() + " has joined the chat]");

    clients_.insert(client);

    client->start(
    [this, clientVar = client](const std::string& msg)
    {
      executeFunc(clientVar, msg);
    },
    [this, weak = std::weak_ptr<Session>(client)]
    {
      if (auto shared = weak.lock(); shared && clients_.erase(shared))
      {
        std::stringstream id;
        error_code error_id;
        id << shared->id(error_id).port();
        informationPost("[User" + id.str() + " has left the chat]");
      }
    });
    async_accept();
  });
}


void chat::Server::informationPost(const std::string& message)
{
  for (auto& client : clients_)
  {
    client->informationMsg(message);
  }
}
void chat::Server::post(std::shared_ptr< Session > session, const std::string& message)
{
  for (auto& client : clients_)
  {
    client->post(session, message);
  }
}

std::vector< std::string > chat::Server::splitWhitespace(const std::string& msg)
{
  std::stringstream ss(msg);
  std::vector< std::string > result;
  std::string word;

  while (ss >> word)
  {
    result.push_back(word);
  }
  return result;
}

void chat::Server::executeFunc(std::shared_ptr< Session > session, std::string line)
{
  using cmd_t = void (chat::Server::*)(std::shared_ptr< Session > session, const std::string& msg);
  std::unordered_map< std::string, cmd_t > cmds;
  cmds["post"] = &chat::Server::post;

  std::vector< std::string > params = splitWhitespace(line);
  session->post(session, "[" + params[0] + "]\n");
  session->post(session, "[" + params[1] + "]\n");
  session->post(session, line);
  if (params.size() == 0)
  {
    return;
  }
  if (params.size() < 2)
  {
    session->informationMsg("Usage: post <message>");
    return;
  }
  try
  {
    (this->*cmds.at(params[1]))(session, params[2]);
  }
  catch(const std::out_of_range&)
  {
    std::cerr << params[0] << '\n';
    std::cerr << "Unknown command\n";
  }
}


void chat::Server::showUsers(std::shared_ptr< Session > session)
{
  session->informationMsg("[Users]");
  size_t i = 1;
  for (const auto& client: clients_)
  {
    std::stringstream message;
    error_code error_msg;
    message << std::to_string(i) << ". " << client->id(error_msg) << "\n";
    session->informationMsg(message.str());
  }
}


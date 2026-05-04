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
    client->informationMsg("[INFO] Welcome to chat, User" + id.str());
    informationPost("[INFO] User" + id.str() + " has joined the chat");

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
        informationPost("[INFO] User" + id.str() + " has left the chat");
      }
    });
    async_accept();
  });
}


void chat::Server::informationPost(const std::string& msg)
{
  for (auto& client : clients_)
  {
    client->informationMsg(msg);
  }
}
void chat::Server::post(std::shared_ptr< Session > session, const std::string& msg)
{
  if (msg.empty())
  {
    session->informationMsg("[WARN] Usage: post <msg>");
    return;
  }
  for (auto& client : clients_)
  {
    if (client != session)
    {
      client->post(session, msg);
    }
  }
  session->informationMsg("[DONE] The message was sent to everyone");
}

void chat::Server::send(std::shared_ptr< Session > session, const std::string& msg)
{
  std::vector< std::string > params = splitWhitespace(msg);
  if (params.size() < 2)
  {
    session->informationMsg("[WARN] Usage: send <User's port> <msg>");
    return;
  }

  std::string clientId = params[0];
  std::string msgText = getStrParams(params, 1); 

  for (auto& client: clients_)
  {
    std::stringstream id;
    error_code error;
    id << client->id(error).port();
    if (id.str() == clientId)
    {
      client->post(session, msgText);
      session->informationMsg("[DONE] The message was sent to User" + id.str());
      return;
    }
  }
  session->informationMsg("[WARN] Can't find User" + clientId);
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

std::string chat::Server::getStrParams(const std::vector< std::string >& params, size_t start)
{
  std::string res;
  if (params.size() < start + 1)
  {
    return res;
  }
  res = params[start];
  for (size_t i = start + 1; i < params.size(); ++i)
  {
    res += (" " + params[i]);
  }
  return res;
}

void chat::Server::executeFunc(std::shared_ptr< Session > session, std::string line)
{
  using cmd_t = void (chat::Server::*)(std::shared_ptr< Session > session, const std::string& msg);
  std::unordered_map< std::string, cmd_t > cmds;
  cmds["/post"] = &chat::Server::post;
  cmds["/send"] = &chat::Server::send;
  cmds["/users"] = &chat::Server::showUsers;

  std::vector< std::string > params = splitWhitespace(line);
  if (params.size() < 1)
  {
    return;
  }
  try
  {
    (this->*cmds.at(params[1]))(session, getStrParams(params, 2));
  }
  catch(const std::out_of_range&)
  {
    session->informationMsg("[ERROR] Unknown command: " + params[1]);
  }
}


void chat::Server::showUsers(std::shared_ptr< Session > session, const std::string&)
{
  session->informationMsg("[Users]");
  size_t i = 1;
  for (const auto& client: clients_)
  {
    std::stringstream msg;
    error_code error_msg;
    msg << std::to_string(i) << ". User" << client->id(error_msg).port();
    session->informationMsg(msg.str());
    ++i;
  }
}


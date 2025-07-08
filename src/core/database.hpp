#pragma once

#include "../network/server.hpp"
#include "engine.hpp"
#include "interpreter.hpp"

class DatabaseServer
{
public:
  DatabaseServer(const std::string &dir,
           const std::string &port = "65535",
           const std::string &host = "127.0.0.1")
      : engine_(dir), server_(port, host, 1024, SOMAXCONN)
  {
    server_.setHandler([this](const std::string &msg)
                       { return handleCommand(msg); });
  }

  bool start()
  {
    return server_.start();
  }

  void stop()
  {
    server_.stop();
  }

private:
  Engine engine_;
  Server server_;

  std::string handleCommand(const std::string &msg)
  {
    try
    {
      auto args = parser_engine::readCommand(msg);
      auto result = engine_.exec(args);
      return result.toJson();
    }
    catch (const std::exception &ex)
    {
      return std::string("Error: ") + ex.what();
    }
  }
};

#pragma once

#include "../network/server.hpp"
#include "engine.hpp"
#include "interpreter.hpp"

class DatabaseServer
{
public:
  DatabaseServer(const std::string &dir,
           const std::string &port = "65535",
           size_t bufSize = 1024,
           int backlog = SOMAXCONN)
      : engine_(dir), server_(port, bufSize, backlog)
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
      std::ostringstream result;
      
      auto oldCoutBuf = std::cout.rdbuf(result.rdbuf());
      engine_.exec(args);
      std::cout.rdbuf(oldCoutBuf);
      return result.str();
    }
    catch (const std::exception &ex)
    {
      return std::string("❌ Error: ") + ex.what();
    }
  }
};

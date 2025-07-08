#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class SubCmd : public EngineCommand
{
public:
  explicit SubCmd(const ArgsCommandSub &args, std::function<void(const std::string, SOCKET)> sc)
      : args_(args), send_client(sc) {}

  CommandResult execute(DatabaseNode &db, std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events) const override
  {
    SOCKET clientCopy = args_.client;
    auto send = send_client;
    events->subscribe(args_.tableName, [clientCopy, send](const CommandResult &evt)
                      {
                        send(evt.toJson(), clientCopy);
                      });

    return CommandResult::Ok("Cliente suscrito: " + std::to_string(args_.client));
  }

  void log(std::ostream &os) const override
  {
  }

private:
  ArgsCommandSub args_;
  std::function<void(const std::string, SOCKET)> send_client;
};

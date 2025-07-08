#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class UpdateCmd : public EngineCommand
{
public:
  explicit UpdateCmd(const ArgsCommandUpdate &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db, std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events) const override
  {
    if (db.update(args_.tableName, args_.key, args_.record))
    {
      std::cout << "Actualizado: " << args_.key << "\n";
      auto result = CommandResult::WithData({args_.record}, "Actualizado: " + std::to_string(args_.key));
      events->emit(args_.tableName, result);
      return result;
    }
    std::cerr << "No se encontro la clave\n";
    return CommandResult::Fail("No se encontro la clave");
  }

  void log(std::ostream &os) const override
  {
    os << operationToString(Operation::UPDATE) << " " << args_.tableName;
    for (auto &rv : args_.record.values)
      os << ", " << rv.column << ":" << rv.value << ":" << rv.kind;
    os << "\n";
  }

private:
  ArgsCommandUpdate args_;
};

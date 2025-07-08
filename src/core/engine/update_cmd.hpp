#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class UpdateCmd : public EngineCommand
{
public:
  explicit UpdateCmd(const ArgsCommandUpdate &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db) const override
  {
    if (db.update(args_.tableName, args_.key, args_.record))
    {
      std::cout << "Actualizado: " << args_.key << "\n";
      return CommandResult::WithData({args_.record}, "Actualizado: " + std::to_string(args_.key));
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

#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class InsertCmd : public EngineCommand
{
public:
  explicit InsertCmd(const ArgsCommandInsert &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db, std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events) const override
  {
    if (db.insert(args_.tableName, args_.record))
    {
      auto result = CommandResult::WithData({args_.record}, "Insertado en " + args_.tableName);
      events->emit(args_.tableName, result);
      return result;
    }
    std::cerr << "Clave duplicada al insertar\n";
    return CommandResult::Fail("Clave duplicada al insertar");
  }

  void log(std::ostream &os) const override
  {
    os << operationToString(Operation::INSERT) << " " << args_.tableName;
    for (auto &rv : args_.record.values)
      os << ", " << rv.column << ":" << rv.value << ":" << rv.kind;
    os << "\n";
  }

private:
  ArgsCommandInsert args_;
};
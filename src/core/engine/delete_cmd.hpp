#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class DeleteCmd : public EngineCommand
{
public:
  explicit DeleteCmd(const ArgsCommandDelete &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db) const override
  {
    if (db.remove(args_.tableName, args_.key))
    {
      std::cout << "Eliminado: " << args_.key << "\n";
      return CommandResult::Success;
    }
    std::cerr << "No existe clave para eliminar\n";
    return CommandResult::Failure;
  }

  void log(std::ostream &os) const override
  {
    os << operationToString(Operation::REMOVE) << " " << args_.tableName
       << ", id:" << args_.key << ":INT\n";
  }

private:
  ArgsCommandDelete args_;
};

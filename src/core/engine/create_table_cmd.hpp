#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class CreateTableCmd : public EngineCommand
{
public:
  explicit CreateTableCmd(const ArgsCommandCreate &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db) const override
  {
    if (db.createTable(args_.tableName, args_.schema))
    {
      std::cout << "Tabla creada: " << args_.tableName << "\n";
      return CommandResult::Success;
    }
    std::cerr << "Ya existe la tabla " << args_.tableName << "\n";
    return CommandResult::Failure;
  }

  void log(std::ostream &os) const override {
    os << operationToString(Operation::CREATE_TABLE) << " " << args_.tableName;
    for (auto &col : args_.schema.getColumns()) {
      os << ", " << col.first << ":" << kindColumnToString(col.second);
    }
    os << "\n";
  }

private:
  ArgsCommandCreate args_;
};
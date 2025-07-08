#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class CreateTableCmd : public EngineCommand
{
public:
  explicit CreateTableCmd(const ArgsCommandCreate &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db, std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events) const override
  {
    if (db.createTable(args_.tableName, args_.schema))
    {
      std::cout << "Tabla creada: " << args_.tableName << "\n";
      return CommandResult::Ok("Tabla creada: " + args_.tableName);
    }
    std::cerr << "Ya existe la tabla " << args_.tableName << "\n";
    return CommandResult::Fail("Ya existe la tabla " + args_.tableName);
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
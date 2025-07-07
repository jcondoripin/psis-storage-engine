#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class SelectCmd : public EngineCommand
{
public:
  explicit SelectCmd(const ArgsCommandSelect &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db) const override
  {
    std::cerr << "Funcion no implementada: " << args_.tableName << "\n";
    return CommandResult::Failure;
  }

  void log(std::ostream & /*os*/) const override
  {
    // no log on GET
  }

private:
  ArgsCommandSelect args_;
};
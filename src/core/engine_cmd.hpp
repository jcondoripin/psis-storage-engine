#pragma once

#include "../storage/database_node.hpp"

enum class CommandResult
{
  Success,
  Failure
};

std::string commandResultToString(CommandResult r)
{
  if (r == CommandResult::Success)
  {
    return "Success operation";
  }
  else
  {
    return "Failure operation";
  }
}

class EngineCommand
{
public:
  virtual ~EngineCommand() = default;
  virtual CommandResult execute(DatabaseNode &db) const = 0;
  virtual void log(std::ostream &os) const = 0;
};
#pragma once

#include "../engine_cmd.hpp"
#include "../../storage/database_node.hpp"
#include "../../util/event.hpp"
#include <memory>

class QueryCommand : public EngineCommand {
public:
  ArgsQuery args;

  QueryCommand(ArgsQuery q) : args(std::move(q)) {} 

  CommandResult execute(DatabaseNode &db, std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events_subs_) const override {
    try {
      auto result = db.searchFilter(args.tableName, args.filter);
      return CommandResult::WithData(result, "Consulta WHERE ejecutada.");
    } catch (const std::exception &e) {
      return CommandResult::Fail(e.what());
    }
  }

  void log(std::ostream &os) const override {
    os << "SELECT FROM " << args.tableName << " WHERE ";
    for (const auto &v : args.filter.values)
      os << v.column << "='" << v.value << "' ";
  }
};

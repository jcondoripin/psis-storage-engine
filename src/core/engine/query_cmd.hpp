#pragma once

#include "../engine_cmd.hpp"
#include "../../storage/database_node.hpp"
#include "../../util/event.hpp"

class QueryCmd : public EngineCommand
{
public:
  explicit QueryCmd(const ArgsCommandQuery &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db, std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events_subs_) const override
  {
    auto records = db.searchFilter(args_.tableName, args_.filter);
    for (auto &record : records)
    {
      for (const auto &v : record.values)
        std::cout << v.column << "=" << v.value << " ";
      std::cout << "\n";
    }
    return CommandResult::WithData(records, "Registros obtenidos");
  }

  void log(std::ostream & /*os*/) const override
  {
    // no log on GET
  }

private:
  ArgsCommandQuery args_;
};

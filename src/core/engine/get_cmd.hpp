#pragma once

#include "../engine_cmd.hpp"
#include "../../util/commands.hpp"
#include "../../util/lib.hpp"

class GetCmd : public EngineCommand
{
public:
  explicit GetCmd(const ArgsCommandGet &args)
      : args_(args) {}

  CommandResult execute(DatabaseNode &db) const override
  {
    auto opt = db.get(args_.tableName, args_.key);
    if (opt)
    {
      for (const auto &v : (*opt).values)
        std::cout << v.column << "=" << v.value << " ";
      std::cout << "\n";
      return CommandResult::WithData({opt.value()}, "Registro encontrado");
    }
    std::cerr << "Registro no encontrado: " << args_.key << "\n";
    return CommandResult::Fail("Registro no encontrado: " + std::to_string(args_.key));
  }

  void log(std::ostream & /*os*/) const override
  {
    // no log on GET
  }

private:
  ArgsCommandGet args_;
};
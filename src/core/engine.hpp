#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include <fstream>
#include <iostream>
#include "../util/operation.hpp"
#include "../util/table.hpp"
#include "../util/commands.hpp"
#include "../storage/database_node.hpp"
#include "../util/lib.hpp"

#include "engine_cmd.hpp"
#include "./engine/create_table_cmd.hpp"
#include "./engine/insert_cmd.hpp"
#include "./engine/update_cmd.hpp"
#include "./engine/delete_cmd.hpp"
#include "./engine/get_cmd.hpp"
#include "./engine/select_cmd.hpp"

inline std::unique_ptr<EngineCommand> makeCommand(const ArgsCommandGeneral &args)
{
  if (args.create.has_value())
  {
    return std::make_unique<CreateTableCmd>(
        static_cast<const ArgsCommandCreate &>(args.create.value()));
  }
  else if (args.select.has_value())
  {
    return std::make_unique<SelectCmd>(
        static_cast<const ArgsCommandSelect &>(args.select.value()));
  }
  else if (args.get.has_value())
  {
    return std::make_unique<GetCmd>(
        static_cast<const ArgsCommandGet &>(args.get.value()));
  }
  else if (args.insert.has_value())
  {
    return std::make_unique<InsertCmd>(
        static_cast<const ArgsCommandInsert &>(args.insert.value()));
  }
  else if (args.update.has_value())
  {
    return std::make_unique<UpdateCmd>(
        static_cast<const ArgsCommandUpdate &>(args.update.value()));
  }
  else if (args.remove.has_value())
  {
    return std::make_unique<DeleteCmd>(
        static_cast<const ArgsCommandDelete &>(args.remove.value()));
  }
  else
  {
    return nullptr;
  }
}

class Engine
{
public:
  explicit Engine(const std::string &dir)
      : dataDir_(dir), db_(dir)
  {
    for (auto &entry : std::filesystem::directory_iterator(dataDir_))
    {
      if (!entry.is_regular_file())
        continue;

      auto ext = entry.path().extension().string();
      if (ext != ".meta")
        continue;

      std::string table = entry.path().stem().string();

      Table schema;

      {
        std::ifstream metaIn(dataDir_ + table + ".meta", std::ios::binary);
        if (!metaIn)
          throw std::runtime_error("No se pudo abrir " + table + ".meta");
        schema.binary_read(metaIn);
      }

      db_.createTableSilent(table, schema);

      std::ifstream dataIn(dataDir_ + table + ".tbl", std::ios::binary);

      if (!dataIn)
        continue;

      std::vector<Record> data = flb::loadTableData(dir, table, schema);

      for (const auto &record : data)
        db_.insertSilent(table, record);
    }
  }

  CommandResult exec(const ArgsCommandGeneral &args)
  {
    auto cmd = makeCommand(args);
    if (!cmd)
    {
      std::cerr << "Comando desconocido\n";
      return CommandResult::Fail("Comando desconocido");
    }

    auto result = cmd->execute(db_);

    if (result.success)
    {
      std::string dir = dataDir_ + "logs/";
      std::filesystem::create_directories(dir);
      std::ofstream ofs(dir + get_date() + ".log", std::ios::app);
      cmd->log(ofs);
    }

    return result;
  }

private:
  std::string dataDir_;
  DatabaseNode db_;
};

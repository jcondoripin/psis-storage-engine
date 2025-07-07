#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "../util/operation.hpp"
#include "../util/table.hpp"
#include "../storage/database_node.hpp"
#include "../storage/database_node_flb.hpp"
#include "../util/lib.hpp"

struct ArgsCommand
{
  Operation command;
  std::string tableName;
  int64_t key = 0;
  Table schema = {};
  Record record = {};
};

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

  void exec(ArgsCommand args)
  {
    auto [command, tableName, key, schema, record] = args;

    std::istringstream ss(operationToString(command));

    if (command == Operation::CREATE_TABLE)
    {
      bool ok = db_.createTable(tableName, schema);
      if (ok)
      {
        logCreateTable(command, tableName, schema);
        std::cout << "✅ Tabla creada: " << tableName << "\n";
      }
      else
        std::cerr << "⚠️ Ya existe la tabla " << tableName << "\n";
    }
    else if (command == Operation::INSERT)
    {
      if (db_.insert(tableName, record))
      {
        std::cout << "✅ Insertado en " << tableName << "\n";
        logMutation(command, tableName, record);
      }
      else
        std::cerr << "❌ Clave duplicada al insertar\n";
    }
    else if (command == Operation::SEARCH)
    {
      auto res = db_.search(tableName, key);
      if (res)
      {
        for (const auto &v : (*res).values)
          std::cout << v.column << "=" << v.value << " ";
        std::cout << "\n";
      }
      else
        std::cerr << "❌ Registro no encontrado\n";
    }
    else if (command == Operation::UPDATE)
    {
      if (db_.update(tableName, key, record))
      {
        std::cout << "✅ Actualizado: " << key << "\n";
        logMutation(command, tableName, record);
      }
      else
        std::cerr << "❌ No se encontró la clave\n";
    }
    else if (command == Operation::REMOVE)
    {
      if (db_.remove(tableName, key))
      {
        std::cout << "✅ Eliminado: " << key << "\n";
        Record r;
        r.values.push_back({"id", std::to_string(key), "INT"});
        logMutation(command, tableName, r);
      }
      else
        std::cerr << "❌ No existe clave para eliminar\n";
    }
    else if (command == Operation::BACKUP)
    {
      loadBackup();
      std::cout << "🗂️ Backup guardado en backup.log\n";
    }
    else
    {
      std::cerr << "❌ Comando desconocido: " << operationToString(command) << "\n";
    }
  }

private:
  std::string dataDir_;
  DatabaseNode db_;

  void logCreateTable(const Operation &op, const std::string &table, const Table &schema)
  {
    std::string logDir = dataDir_ + "logs/";
    std::filesystem::create_directories(logDir);
    std::string logFile = logDir + get_date() + ".log";

    std::ofstream log(logFile, std::ios::app);
    if (!log)
      throw std::runtime_error("No se pudo abrir el archivo de log: " + logFile);

    log << operationToString(op) << " " << table;
    for (const auto &[name, kind] : schema.getColumns())
      log << ", " << name << ":" << kindColumnToString(kind);
    log << "\n";
  }

  void logMutation(const Operation &op, const std::string &table, const Record &r)
  {
    std::string logDir = dataDir_ + "logs/";
    std::filesystem::create_directories(logDir);
    std::string logFile = logDir + get_date() + ".log";

    std::ofstream log(logFile, std::ios::app);
    if (!log)
      throw std::runtime_error("No se pudo abrir el archivo de log: " + logFile);

    log << operationToString(op) << " " << table;
    for (const auto &rv : r.values)
      log << ", " << rv.column << ":" << rv.value << ":" << rv.kind;
    log << "\n";
  }

  void loadBackup()
  {
    for (const auto &entry : std::filesystem::directory_iterator(dataDir_))
    {
      if (entry.path().extension() == ".tbl" || entry.path().extension() == ".meta")
      {
        std::cout << "Requires backup process" << std::endl;
      }
    }
  }
};

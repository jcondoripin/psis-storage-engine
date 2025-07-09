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
#include "../events/event.hpp"

#include "engine_cmd.hpp"
#include "./engine/create_table_cmd.hpp"
#include "./engine/insert_cmd.hpp"
#include "./engine/update_cmd.hpp"
#include "./engine/delete_cmd.hpp"
#include "./engine/sub_cmd.hpp"
#include "./engine/get_cmd.hpp"
#include "./engine/query_cmd.hpp"

/**
 * @brief Fábrica de comandos del motor.
 * Crea una instancia del comando correspondiente en base al tipo de operación encapsulado en ArgsCommandGeneral.
 *
 * @param args Estructura que puede contener un único tipo de comando (CREATE, SELECT, GET, etc.).
 * @return std::unique_ptr<EngineCommand> Instancia polimórfica del comando correspondiente o nullptr si no hay comando válido.
 */
inline std::unique_ptr<EngineCommand> makeCommand(const ArgsCommandGeneral &args, std::function<void(const std::string, SOCKET)> listenner)
{
  if (args.create.has_value())
  {
    return std::make_unique<CreateTableCmd>(
        static_cast<const ArgsCommandCreate &>(args.create.value()));
  }
  else if (args.get.has_value())
  {
    return std::make_unique<GetCmd>(
        static_cast<const ArgsCommandGet &>(args.get.value()));
  }
  else if (args.query.has_value())
  {
    return std::make_unique<QueryCmd>(
        static_cast<const ArgsCommandQuery &>(args.query.value()));
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
  else if (args.sub.has_value())
  {
    return std::make_unique<SubCmd>(
        static_cast<const ArgsCommandSub &>(args.sub.value()), listenner);
  }
  else
  {
    return nullptr;
  }
}

/**
 * @brief Motor principal de la base de datos.
 *
 * Encargado de:
 *  - Cargar el esquema (`.meta`) y los datos (`.tbl`) desde disco.
 *  - Ejecutar comandos recibidos y delegarlos a su implementación correspondiente.
 *  - Registrar logs de operaciones exitosas.
 */
class EngineNode
{
public:
  /**
   * @brief Constructor que inicializa el motor cargando todas las tablas desde el directorio especificado.
   *
   * @param dir Directorio raíz donde se almacenan los archivos `.meta` y `.tbl`.
   *
   * Este constructor escanea el directorio, reconstruye las tablas silenciosamente (`createTableSilent`)
   * y carga sus datos (`insertSilent`). Las tablas inválidas o sin datos se omiten.
   */
  explicit EngineNode(const std::string &dir)
      : dataDir_(dir), db_(dir)
  {
    this->events_subs_ = std::make_shared<EventKeyHandler<std::string, CommandResult>>();

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

  /**
   * @brief Ejecuta un comando general (CREATE, GET, INSERT, etc.) sobre la base de datos.
   *
   * @param args Argumento polimórfico que encapsula un único tipo de comando.
   * @return CommandResult Resultado con estado de éxito, mensaje y datos opcionales.
   *
   * Este método:
   *  - Instancia el comando correspondiente (vía `makeCommand`).
   *  - Lo ejecuta sobre `DatabaseNode`.
   *  - Si fue exitoso, registra el log del comando.
   */
  CommandResult exec(const ArgsCommandGeneral &args, std::function<void(const std::string, SOCKET)> send_client)
  {
    auto cmd = makeCommand(args, send_client);
    if (!cmd)
    {
      std::cerr << "Comando desconocido\n";
      return CommandResult::Fail("Comando desconocido");
    }

    auto result = cmd->execute(db_, events_subs_);

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
  std::shared_ptr<EventKeyHandler<std::string, CommandResult>> events_subs_;
};

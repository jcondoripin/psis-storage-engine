#pragma once

#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <vector>
#include <string>
#include "../util/table.hpp"
#include "../util/record.hpp"

/**
 * @namespace flb
 * @brief File Load Backup - utilidades para persistencia de datos binarios en disco.
 *
 * El namespace `flb` agrupa funciones auxiliares para manejar la lectura y escritura
 * de archivos binarios asociados a tablas de una base de datos. Proporciona utilidades para:
 * - Serialización y deserialización de registros (`Record`) según su tipo (`INT`, `TEXT`, etc.).
 * - Escritura incremental (`append`) o completa (`rewrite`) de datos al archivo `.tbl`.
 * - Carga del esquema desde archivo `.meta`.
 * - Carga de registros completos desde el archivo `.tbl` usando el esquema.
 *
 * Estas funciones permiten separar la lógica de almacenamiento físico del núcleo
 * de la clase `DatabaseNode`, facilitando mantenimiento, pruebas y reutilización.
 */
namespace flb

{
  /**
   * @brief Serializa un valor al stream según su tipo.
   */
  inline void writeByKind(std::ofstream &out, const std::string &kind, const std::string &val)
  {
    if (kind == "INT")
    {
      int64_t v = std::stoll(val);
      out.write(reinterpret_cast<const char *>(&v), sizeof(v));
    }
    else if (kind == "LONG")
    {
      long v = std::stol(val);
      out.write(reinterpret_cast<const char *>(&v), sizeof(v));
    }
    else if (kind == "DOUBLE")
    {
      double v = std::stod(val);
      out.write(reinterpret_cast<const char *>(&v), sizeof(v));
    }
    else if (kind == "TEXT")
    {
      uint32_t len = static_cast<uint32_t>(val.size());
      out.write(reinterpret_cast<const char *>(&len), sizeof(len));
      out.write(val.data(), len);
    }
    else if (kind == "BOOL")
    {
      uint8_t b = (val == "true");
      out.write(reinterpret_cast<const char *>(&b), sizeof(b));
    }
    else if (kind == "DATE")
    {
      std::istringstream ss(val);
      std::tm tm = {};
      ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
      auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
      int64_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
      out.write(reinterpret_cast<const char *>(&secs), sizeof(secs));
    }
    else
    {
      throw std::runtime_error("Unsupported kind: " + kind);
    }
  }

  /**
   * @brief Apéndice binario de un registro al archivo .tbl.
   * @param path Directorio base.
   * @param name Nombre de la tabla.
   * @param record Registro a escribir.
   */
  inline void appendRecordToFile(const std::string &path,
                                 const std::string &name,
                                 const Record &record)
  {
    std::filesystem::create_directories(path);
    std::ofstream out(path + name + ".tbl", std::ios::binary | std::ios::app);
    if (!out)
      throw std::runtime_error("Failed to open data file for insert: " + name);
    for (const auto &rv : record.values)
      writeByKind(out, rv.kind, rv.value);
  }

  /**
   * @brief Reescribe completamente el archivo .tbl a partir de registros dados.
   * @param path Directorio base.
   * @param name Nombre de la tabla.
   * @param records Vector de registros.
   */
  inline void rewriteDataFile(const std::string &path,
                              const std::string &name,
                              const std::vector<Record> &records)
  {
    std::filesystem::create_directories(path);
    std::ofstream out(path + name + ".tbl", std::ios::binary | std::ios::trunc);
    if (!out)
      throw std::runtime_error("Failed to open data file for rewrite: " + name);
    for (const auto &rec : records)
      for (const auto &rv : rec.values)
        writeByKind(out, rv.kind, rv.value);
  }

  /**
   * @brief Carga el esquema de la tabla desde el archivo .meta.
   * @param path Directorio base.
   * @param name Nombre de la tabla.
   * @return Table esquema.
   */
  inline Table loadSchemaFromFile(const std::string &path,
                                  const std::string &name)
  {
    std::filesystem::create_directories(path);
    std::ifstream metaFile(path + name + ".meta", std::ios::binary);
    if (!metaFile)
      throw std::runtime_error("Failed to open metadata file: " + name);
    Table schema;
    schema.binary_read(metaFile);
    return schema;
  }

  /**
   * @brief Lee todos los registros del archivo .tbl según el esquema.
   * @param path Directorio base.
   * @param name Nombre de la tabla.
   * @param schema Esquema previamente cargado.
   * @return Vector de registros.
   */
  inline std::vector<Record> loadTableData(const std::string &path,
                                           const std::string &name,
                                           const Table &schema)
  {
    std::filesystem::create_directories(path);
    std::ifstream in(path + name + ".tbl", std::ios::binary);
    if (!in)
      throw std::runtime_error("Failed to open data file: " + name);
    std::vector<Record> records;
    auto cols = schema.getColumnNames();
    while (in.peek() != EOF)
    {
      Record rec;
      for (const auto &col : cols)
      {
        KindColumn k = schema.getColumnKind(col);
        RecordValue rv{col, "", kindColumnToString(k)};
        switch (k)
        {
        case KindColumn::INT:
        {
          int64_t v;
          in.read(reinterpret_cast<char *>(&v), sizeof(v));
          rv.value = std::to_string(v);
          break;
        }
        case KindColumn::LONG:
        {
          long v;
          in.read(reinterpret_cast<char *>(&v), sizeof(v));
          rv.value = std::to_string(v);
          break;
        }
        case KindColumn::DOUBLE:
        {
          double v;
          in.read(reinterpret_cast<char *>(&v), sizeof(v));
          rv.value = std::to_string(v);
          break;
        }
        case KindColumn::TEXT:
        {
          uint32_t len;
          in.read(reinterpret_cast<char *>(&len), sizeof(len));
          std::string s(len, '\0');
          in.read(&s[0], len);
          rv.value = s;
          break;
        }
        case KindColumn::BOOL:
        {
          uint8_t b;
          in.read(reinterpret_cast<char *>(&b), sizeof(b));
          rv.value = (b ? "true" : "false");
          break;
        }
        case KindColumn::DATE:
        {
          int64_t secs;
          in.read(reinterpret_cast<char *>(&secs), sizeof(secs));
          std::time_t t = secs;
          std::tm tm = *std::gmtime(&t);
          char buf[32];
          std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
          rv.value = buf;
          break;
        }
        }
        rec.values.push_back(rv);
      }
      records.push_back(rec);
    }
    return records;
  }

  /**
   * @brief Guarda todos los datos sobrescribiendo el archivo .tbl (alias de rewriteDataFile).
   */
  inline void saveTableData(const std::string &path,
                            const std::string &name,
                            const Table &schema,
                            const std::vector<Record> &records)
  {
    rewriteDataFile(path, name, records);
  }
}
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <filesystem>
#include "../index/bplustree.hpp"
#include "../util/exceptions/database_exceptions.hpp"
#include "../util/table.hpp"
#include "database_node_flb.hpp"

using Record = std::vector<RecordValue>;

/**
 * @class DatabaseNode
 * @brief Gestor de tablas y registros en disco usando B+Tree para índices.
 *
 * Ofrece operaciones CRUD con persistencia en archivos binarios. Cada tabla
 * mantiene un archivo de metadatos (.meta) y un archivo de datos (.tbl).
 */
class DatabaseNode
{
public:
  /**
   * @brief Constructor que define el directorio de almacenamiento.
   * @param dataDir Ruta base para archivos de tablas.
   */
  explicit DatabaseNode(const std::string &dataDir = "")
      : dataDir_(dataDir)
  {
    if (!dataDir_.empty() && dataDir_.back() != '/')
      dataDir_ += '/';
    std::filesystem::create_directories(dataDir_);
  }

  /**
   * @brief Crea una nueva tabla con esquema definido.
   */
  bool createTable(const std::string &name,
                   const Table &schema,
                   size_t keyColumnIndex,
                   int order = 4)
  {
    if (tables_.count(name))
      return false;
    if (keyColumnIndex >= schema.getColumnNames().size())
      throw std::invalid_argument("Invalid key column index");

    tables_[name] = std::make_unique<TableHolder>(schema, keyColumnIndex, order);
    
    std::ofstream metaOut(dataDir_ + name + ".meta", std::ios::binary);
    if (!metaOut)
      throw std::runtime_error("Failed to create metadata file for table: " + name);
    schema.binary_write(metaOut);

    std::ofstream(dataDir_ + name + ".tbl", std::ios::binary);
    return true;
  }

  /**
   * @brief Inserta un registro en la tabla indicada.
   */
  bool insert(const std::string &name, const Record &record)
  {
    auto h = getHolder(name);
    validateRecordSize(h, record);
    int64_t key = std::stoll(record[h->keyCol].value);
    if (!h->tree.insert(key, record))
      return false;
    flb::appendRecordToFile(dataDir_, name, record);
    return true;
  }

  /**
   * @brief Actualiza un registro existente.
   */
  bool update(const std::string &name, int64_t key, const Record &rec)
  {
    auto h = getHolder(name);
    validateRecordSize(h, rec);
    if (!h->tree.update(key, rec))
      return false;
    flb::rewriteDataFile(dataDir_, name, h->tree.traverse());
    return true;
  }

  /**
   * @brief Elimina un registro por clave.
   */
  bool remove(const std::string &name, int64_t key)
  {
    auto h = getHolder(name);
    if (!h->tree.remove(key))
      return false;
    flb::rewriteDataFile(dataDir_, name, h->tree.traverse());
    return true;
  }

  /**
   * @brief Busca un registro por clave.
   */
  std::optional<Record> search(const std::string &name, int64_t key) const
  {
    return getHolder(name)->tree.search(key);
  }

private:
  struct BaseHolder
  {
    virtual ~BaseHolder() = default;
  };

  struct TableHolder : BaseHolder
  {
    Table schema;
    BPlusTree<int64_t, Record> tree;
    size_t keyCol;
    TableHolder(const Table &s, size_t k, int o) : schema(s), tree(o), keyCol(k) {}
  };

  TableHolder *getHolder(const std::string &name) const
  {
    auto it = tables_.find(name);
    if (it == tables_.end())
      throw DatabaseTableNotFoundException(name);
    auto ptr = dynamic_cast<TableHolder *>(it->second.get());
    if (!ptr)
      throw TypeMismatchException(name);
    return ptr;
  }

  void validateRecordSize(TableHolder *h, const Record &r) const
  {
    if (r.size() != h->schema.getColumnNames().size())
      throw std::invalid_argument("Record size does not match schema");
  }

  std::string dataDir_;
  std::unordered_map<std::string, std::unique_ptr<BaseHolder>> tables_;
};

#pragma once

#include <functional>
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
#include "../util/record.hpp"

/**
 * @class DatabaseNode
 * @brief Gestor de tablas y registros en disco usando B+Tree.
 *
 * Ofrece operaciones CRUD con persistencia en archivos binarios. Cada tabla
 * mantiene un archivo de metadatos (.meta) y un archivo de datos (.tbl) en un directorio data.
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
                   int order = 4)
  {
    if (tables_.count(name))
      return false;

    tables_[name] = std::make_unique<TableHolder>(schema, schema.getKeyColumn(), name, order);

    std::ofstream metaOut(dataDir_ + name + ".meta", std::ios::binary);
    if (!metaOut)
      throw std::runtime_error("Failed to create metadata file for table: " + name);
    schema.binary_write(metaOut);

    std::ofstream(dataDir_ + name + ".tbl", std::ios::binary);
    return true;
  }

  /**
   * @brief Crea una nueva tabla con esquema definido sin registrar en log.
   */
  bool createTableSilent(const std::string &name,
                         const Table &schema,
                         int order = 4)
  {
    if (tables_.count(name))
      return false;

    tables_[name] = std::make_unique<TableHolder>(schema, schema.getKeyColumn(), name, order);
    return true;
  }

  /**
   * @brief Inserta un registro en la tabla indicada.
   */
  bool insert(const std::string &name, const Record &record)
  {
    auto h = getHolder(name);
    validateRecordSize(h, record);
    int64_t key = std::stoll(record.values[h->keyCol].value);
    if (!h->tree.insert(key, record))
      return false;
    flb::appendRecordToFile(dataDir_, name, record);
    return true;
  }

  /**
   * @brief Inserta un registro en la tabla indicada sin añadir al log de datos.
   */
  bool insertSilent(const std::string &name, const Record &record)
  {
    auto h = getHolder(name);
    validateRecordSize(h, record);
    int64_t key = std::stoll(record.values[h->keyCol].value);
    if (!h->tree.insert(key, record))
      return false;
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
   * @brief Realiza una búsqueda con filtros por columna.
   * Solo funciona para columnas no-clave, recorre los nodos hoja.
   */
  std::vector<Record> searchFilter(const std::string &name, const Record &rec)
  {
    auto h = getHolder(name);
    validateRecordSize(h, rec);
    if (rec.values.size() == 0) {
      return h->tree.traverse();
    }
    auto results = h->tree.filter(rec);
    return results;
  }

  /**
   * @brief Busca un registro por clave.
   */
  std::optional<Record> get(const std::string &name, int64_t key) const
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
    std::string name;
    Table schema;
    BPlusTree<int64_t, Record> tree;
    size_t keyCol;
    TableHolder(const Table &s, size_t k, std::string n, int o) : schema(s), tree(o), name(n), keyCol(k)
    {
      tree.events.subscribe(
          [this](const BPlusTreeEvent<int64_t, Record> &evt)
          {
            std::cout
                << "[Tree event - " << name << "] " << to_string_bplustree_event(evt.type)
                << " | key=" << (evt.key ? std::to_string(*evt.key) : "-")
                << " | msg=" << evt.message
                << std::endl;
          });
    }
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
    if (r.values.size() > h->schema.getColumnNames().size())
      throw std::invalid_argument("Record size does not match schema");
  }

  std::string dataDir_;
  std::unordered_map<std::string, std::unique_ptr<BaseHolder>> tables_;
};

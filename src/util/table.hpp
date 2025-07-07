#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <cstdint>
#include <stdexcept>
#include "./exceptions/database_exceptions.hpp"
#include "column.hpp"

class Table
{
private:
  std::size_t keyCol;
  std::unordered_map<std::string, KindColumn> columns_; // name -> kind (ahora es KindColumn)
  std::vector<std::string> column_names_;               // ordered column names

public:
  virtual ~Table() = default;

  void addColumn(const Column &col)
  {
    if (columns_.count(col.name))
    {
      throw ColumnAlreadyExistsException(col.name);
    }
    columns_[col.name] = col.kind;
    column_names_.push_back(col.name);
  }

  std::size_t getKeyColumn() const
  {
    return this->keyCol;
  }

  void setKeyColumn(std::size_t idx)
  {
    if (idx >= column_names_.size())
      throw std::invalid_argument("Key column index out of range");
    keyCol = idx;
  }

  KindColumn getColumnKind(const std::string &name) const
  {
    auto it = columns_.find(name);
    if (it == columns_.end())
    {
      throw ColumnNotFoundException(name);
    }
    return it->second;
  }

  const std::vector<std::string> &getColumnNames() const
  {
    return column_names_;
  }

  const std::unordered_map<std::string, KindColumn> &getColumns() const
  {
    return columns_;
  }

  void binary_write(std::ofstream &out) const
  {
    uint32_t totalCols = static_cast<uint32_t>(column_names_.size());
    out.write(reinterpret_cast<const char *>(&totalCols), sizeof(totalCols));
    uint32_t keyIdx = static_cast<uint32_t>(keyCol);
    out.write(reinterpret_cast<const char *>(&keyIdx), sizeof(keyIdx));
    for (const auto &colName : column_names_)
    {
      Column col{colName, columns_.at(colName)};
      col.binary_write(out);
    }
  }

  void binary_read(std::ifstream &in)
  {
    columns_.clear();
    column_names_.clear();
    uint32_t totalCols;
    in.read(reinterpret_cast<char *>(&totalCols), sizeof(totalCols));
    uint32_t keyIdx;
    in.read(reinterpret_cast<char *>(&keyIdx), sizeof(keyIdx));
    keyCol = static_cast<std::size_t>(keyIdx);
    for (uint32_t i = 0; i < totalCols; ++i)
    {
      Column col;
      col.binary_read(in);
      columns_[col.name] = col.kind;
      column_names_.push_back(col.name);
    }
  }
};

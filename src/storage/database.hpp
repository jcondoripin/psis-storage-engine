#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <stdexcept>
#include "../index/bplustree.hpp"
#include "../util/database_exceptions.hpp"

template <typename Key, typename Value>
class Table;

class DatabaseNode
{
public:
  DatabaseNode() = default;

  template <typename Key, typename Value>
  void createTable(
      const std::string &name,
      std::function<Key(const Value &)> keyExtractor,
      int order = 4)
  {
    if (tables_.count(name))
    {
      throw DatabaseTableAlreadyExistsException(name);
    }
    tables_[name] = std::make_unique<TableHolder<Key, Value>>(keyExtractor, order);
  }

  template <typename Key, typename Value>
  void insert(const std::string &name, const Value &record)
  {
    auto base = getHolder(name);
    auto holder = dynamic_cast<TableHolder<Key, Value> *>(base);
    if (!holder)
    {
      throw TypeMismatchException(name);
    }
    holder->table.insert(holder->keyExtractor(record), record);
  }

  template <typename Key, typename Value>
  std::optional<Value> search(const std::string &name, const Key &key) const
  {
    auto base = getHolder(name);
    auto holder = dynamic_cast<const TableHolder<Key, Value> *>(base);
    if (!holder)
    {
      throw TypeMismatchException(name);
    }
    return holder->table.search(key);
  }

private:
  struct BaseHolder
  {
    virtual ~BaseHolder() = default;
  };

  template <typename Key, typename Value>
  struct TableHolder : BaseHolder
  {
    using Tree = index::BPlusTree<Key, Value>;

    Tree table;
    std::function<Key(const Value &)> keyExtractor;

    TableHolder(std::function<Key(const Value &)> extractor, int order)
        : table(order), keyExtractor(std::move(extractor)) {}
  };

  BaseHolder *getHolder(const std::string &name) const
  {
    auto it = tables_.find(name);
    if (it == tables_.end())
    {
      throw DatabaseTableNotFoundException(name);
    }
    return it->second.get();
  }

  std::unordered_map<std::string, std::unique_ptr<BaseHolder>> tables_;
};
#pragma once

#include "bplustree_node.hpp"
#include <memory>
#include <optional>
#include <utility>

template <typename Key, typename Value>
class BPlusTree
{
private:
  std::shared_ptr<Node<Key, Value>> root;
  int order;

public:
  explicit BPlusTree(int m = 4);

  // Inserta una clave y valor en el árbol
  void insert(const Key &key, const Value &value);

  // Busca un valor asociado a una clave
  std::optional<Value> search(const Key &key) const;

  // Elimina una clave del árbol
  bool remove(const Key &key);

  // Imprime el árbol (para depuración)
  void print() const;
};

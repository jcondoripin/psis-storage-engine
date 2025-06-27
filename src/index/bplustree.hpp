#pragma once

#include "bplustree_node.hpp"
#include <memory>
#include <optional>
#include <utility>
#include <iostream>

template <typename Key, typename Value>
class BPlusTree
{
private:
  std::shared_ptr<Node<Key, Value>> root;
  int order;

public:
  explicit BPlusTree(int m = 4);

  /**
   * Inserta un par clave-valor en el árbol B+
   * @param key La clave a insertar
   * @param value El valor asociado a la clave
   * @return void
   */
  void insert(const Key &key, const Value &value);

  /**
   * Busca un valor asociado a una clave en el árbol B+
   * @param key La clave a buscar
   * @return Un std::optional<Value> que contiene el valor si se encuentra, o std::nullopt si no
   * se encuentra
   */
  std::optional<Value> search(const Key &key) const;

  /**
   * Elimina una clave y su valor asociado del árbol B+
   * @param key La clave a eliminar
   * @return true si la clave fue eliminada, false si no se encontró
   */
  bool remove(const Key &key);

  /**
   * Imprime el contenido del árbol B+ en orden
   * @return void
   */
  void print() const;
};
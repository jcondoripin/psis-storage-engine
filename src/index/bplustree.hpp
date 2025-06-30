#pragma once

#include "./bplustree_node/bplustree_node.hpp"
#include "./bplustree_node/bplustree_leaf_node.hpp"
#include "./bplustree_node/bplustree_internal_node.hpp"
#include "../util/bplustree_exceptions.hpp"
#include <memory>
#include <tuple>
#include <optional>
#include <utility>
#include <iostream>

template <typename Key, typename Value>
class BPlusTree
{
private:
  std::shared_ptr<BPlusTreeNode<Key, Value>> root;
  std::shared_ptr<BPlusTreeLeafNode<Key, Value>> rootLinked;
  int order;

public:
  BPlusTree<Key, Value>(int m = 4);

  /**
   * Insert a key-value pair into the B+ tree
   * @param key The key to insert
   * @param value The value associated with the key
   * @return void
   * @throws KeyAlreadyExistsException if the key already exists in the tree
   */
  void insert(const Key &key, const Value &value)
  {
    if (!root)
    {
      root = std::make_shared<BPlusTreeLeafNode<Key, Value>>();
      rootLinked = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(root);
      auto leaf = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(root);
      leaf->insertValue(key, value);
      return;
    }

    try
    {
      auto currentNode = this->root;
      auto tupleInsert = insertRecursive(key, value, currentNode);
      if (tupleInsert.has_value())
      {
        auto [firstNode, secondNode, promotedKey] = *tupleInsert;
        auto newRootNode = std::make_shared<BPlusTreeInternalNode<Key, Value>>();
        newRootNode->insertChild(firstNode, secondNode, promotedKey);
        this->root = newRootNode;
      }
    }
    catch (const KeyAlreadyExistsException &)
    {
      throw KeyAlreadyExistsException("Key already exists in B+ Tree: " + std::to_string(key));
    }
  }

  /**
   * Busca un valor asociado a una clave en el árbol B+
   * @param key La clave a buscar
   * @return Un std::optional<Value> que contiene el valor si se encuentra, o std::nullopt si no
   * se encuentra
   */
  std::optional<Value> search(const Key &key) const
  {
    try
    {
      auto currentNode = this->root;
      while (currentNode && !std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(currentNode))
      {
        auto internalNode = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(currentNode);
        currentNode = internalNode->getChildForKey(key);
      }
      return currentNode->getValueByKey(key);
    }
    catch (const KeyNotFoundException &)
    {
      return std::nullopt;
    }
  };

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
  void print() const
  {
    if (!root)
    {
      std::cout << "El árbol B+ está vacío." << std::endl;
      return;
    }
    std::cout << "Contenido del árbol B+ en orden:" << std::endl;
    printRecursive(root);
  }

private:
  /**
   * Insert recursive with split operator
   * @param key The key to insert
   * @param value The value associated with the key
   * @return std::optional<std::pair<Key, Value>> Promoted key if split occurs, otherwise std::nullopt
   * @throws KeyAlreadyExistsException if the key already exists in the tree
   */
  std::optional<std::tuple<std::shared_ptr<BPlusTreeNode<Key, Value>>, std::shared_ptr<BPlusTreeNode<Key, Value>>, Key>> insertRecursive(const Key &key, const Value &value, std::shared_ptr<BPlusTreeNode<Key, Value>> node)
  {
    if (auto leafNode = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(node))
    {
      leafNode->insertValue(key, value);
      if (leafNode->isFull(this->order))
      {
        return leafNode->split();
      }
      else
      {
        return std::nullopt;
      }
    }
    else
    {
      auto internalNode = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(node);
      auto child = internalNode->getChildForKey(key);
      auto tupleInsert = insertRecursive(key, value, child);

      if (tupleInsert.has_value())
      {
        auto [firstNode, secondNode, promotedKey] = *tupleInsert;
        internalNode->insertChild(firstNode, secondNode, promotedKey);
        if (internalNode->isFull(this->order))
        {
          return internalNode->split();
        }
        else
        {
          return std::nullopt;
        }
      }
      return std::nullopt;
    }
  }

  /**
   * Prints the B+ tree recursively
   * @param node The current node to print
   * @return void
   * @throws EmptyArrayException if the node is empty
   */
  void printRecursive(const std::shared_ptr<BPlusTreeNode<Key, Value>> &node) const
  {
    if (!node)
    {
      throw EmptyArrayException("El nodo está vacío, no se puede imprimir.");
    }
    if (auto leafNode = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(node))
    {
      for (const auto &key : leafNode->keys.getArray())
      {
        std::cout << key << " ";
      }
      std::cout << " = ";
      for (const auto &value : leafNode->values)
      {
        std::cout << value << " ";
      }
      std::cout << "| ";
    }
    else
    {
      auto internalNode = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(node);
      for (const auto &key : internalNode->keys.getArray())
      {
        std::cout << key << " ";
      }
      std::cout << std::endl;
      for (const auto &child : internalNode->children)
      {
        printRecursive(child);
      }
    }
  }
};

template <typename Key, typename Value>
inline BPlusTree<Key, Value>::BPlusTree(int m)
{
  if (m < 2)
  {
    throw std::invalid_argument("El orden del árbol B+ debe ser al menos 2.");
  }
  this->order = m;
  this->root = nullptr;
  this->rootLinked = nullptr;
}

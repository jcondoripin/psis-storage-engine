#pragma once

#include "./bplustree_node/bplustree_node.hpp"
#include "./bplustree_node/bplustree_leaf_node.hpp"
#include "./bplustree_node/bplustree_internal_node.hpp"
#include "../../util/bplustree_exceptions.hpp"
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
   * Search for a leaf node that contains the specified key
   * @param key Key to search for
   * @return std::shared_ptr<BPlusTreeLeafNode<Key, Value>> Pointer to the leaf node containing the key
   * @throws KeyNotFoundException if the key is not found in the tree
   */
  std::shared_ptr<BPlusTreeLeafNode<Key, Value>> findLeafNode(const Key &key) const
  {
    // Implementación de búsqueda del nodo hoja
    std::shared_ptr<BPlusTreeNode<Key, Value>> currentNode = root;
    while (currentNode)
    {
      if (BPlusTreeLeafNode<Key, Value> *leafNode = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(currentNode))
      {
        try
        {
          return leafNode->keys.getValueByKey(key) != Value()
        }
        catch (const KeyNotFoundException &e)
        {
          throw KeyNotFoundException("Clave no encontrada en el nodo hoja: " + std::to_string(key));
        }
      }
      else
      {
        // es nodo interno
        BPlusTreeInternalNode<Key, Value> *internalNode = static_cast<BPlusTreeInternalNode<Key, Value> *>(currentNode);
        int position = internalNode->keys.searchPosition(key);
        if (position < 0)
        {
          position = -position - 1; // Convertir a posición de inserción
        }

        return currentNode; // Retorna el nodo hoja encontrado
      }
    }
    throw KeyNotFoundException("Key not found in B+ Tree");
  }

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
      root->insertValue(key, value);
      return;
    }

    try
    {
      auto currentNode = this->root;
      while (currentNode && !std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(currentNode))
      {
        auto internalNode = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(currentNode);
        currentNode = internalNode->getChildForKey(key)
      }
      auto leafNode = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(currentNode);
      leafNode->insertValue(key, value);

      // split method
      if (leafNode->isFull(this->order))
      {
        auto newLeafNode = std::make_shared<BPlusTreeLeafNode<Key, Value>>();
        leafNode->split(newLeafNode, this->order);
        if (leafNode == this->root)
        {
          auto newRoot = std::make_shared<BPlusTreeInternalNode<Key, Value>>();
          newRoot->insertChild(leafNode->keys.getElementByPosition(0), leafNode);
          newRoot->insertChild(newLeafNode->keys.getElementByPosition(0), newLeafNode);
          this->root = newRoot;
        }
        else
        {
          auto parent = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(leafNode->parent);
          parent->insertChild(newLeafNode->keys.getElementByPosition(0), newLeafNode);
        }
      }
    }
    catch (const KeyAlreadyExistsException &)
    {
      throw KeyAlreadyExistsException("Key already exists in B+ Tree: " + std::to_string(key));
    }
  }

  /**
   * Insert recursive with split operator
   * @param key The key to insert
   * @param value The value associated with the key
   * @return void
   * @throws KeyAlreadyExistsException if the key already exists in the tree
   */
  std::optional<std::pair<Key, Value>> insertRecursive(const Key &key, const Value &value, const BPlusTreeNode<Key, Value> &node)
  {
    if (std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(node))
    {
      auto leafNode = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(node);
      leafNode->insertValue(key, value);
      if (leafNode->isFull(this->order))
      {
        auto newLeafNode = std::make_shared<BPlusTreeLeafNode<Key, Value>>();
        // divide entre 2 y sube la clabe del nood derecho (el menor del mas derecho) method split manual, no se crea
        newLeafNode

      } else {
        return nullptr;
      }
    }
    else
    {
      auto internalNode = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(node);
      auto child = internalNode->getChildForKey(key);
      insertRecursive(key, value, *child);
    }
  }
  {
    if (!root)
    {
      root = std::make_shared<BPlusTreeLeafNode<Key, Value>>();
      root->insertValue(key, value);
      return;
    }

    try
    {
      insertRecursive(key, value, *root);
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
        currentNode = internalNode->getChildForKey(key)
      }
      return leafNode->getValueByKey(key);
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
  void print() const;
};
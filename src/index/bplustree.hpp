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
      if (key == 8)
      {
        std::cout << currentNode << std::endl;
        currentNode->print();
      }
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
  bool remove(const Key &key)
  {
    if (!root)
      return false;
    auto hasRemoved = removeRecursive(root, key);
    this->print();
    return !hasRemoved;
  };

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

  /**
   * Imprime el contenido de las hojas mediante lista enlazada
   * @return void
   */
  void printLinked() const
  {
    if (!rootLinked)
    {
      std::cout << "El árbol B+ está vacío." << std::endl;
      return;
    }
    auto current = this->rootLinked;

    std::cout << "Contenido del árbol B+ en orden:" << std::endl;

    do
    {
      for (const auto &key : current->keys.getArray())
      {
        std::cout << key << " ";
      }
      current = current->next;
    } while ((current != nullptr));

    std::cout << std::endl;
  }

private:
  /**
   * @brief Elimina recursivamente una clave y su valor asociado de un subárbol B+.
   *
   * Si la eliminación causa underflow, intenta redistribuir o fusionar nodos.
   * Si se fusiona un nodo, este método devuelve `true` para que el padre pueda
   * ajustar su estructura (eliminar clave separadora y fusionar hijos).
   *
   * @param node El nodo actual donde se realiza la eliminación.
   * @param key La clave que se debe eliminar.
   * @return bool `true` si el nodo actual se fusionó y requiere ajuste en el padre,
   *              `false` si no se necesita fusión (borrado y balanceo resueltos localmente).
   * @throws KeyNotFoundException si la clave no existe.
   */
  bool removeRecursive(std::shared_ptr<BPlusTreeNode<Key, Value>> node, const Key &key)
  {
    if (auto leaf = toLeaf(node))
    {
      leaf->removeValueByKey(key);
      if (leaf == this->root)
      {
        return false;
      }
      return leaf->isUnderflow(order);
    }
    else
    {
      auto internal = toInternal(node);
      auto child = internal->getChildForKey(key);
      bool childUnderflow = removeRecursive(child, key);

      auto [leftSibling, rightSibling] = internal->getSiblingsFor(child);
      bool childLeaf = toLeaf(child) != nullptr;

      if (!childLeaf)
      {
        int position = internal->keys.searchElement(key);
        if (position != -1)
        {
          auto current = child;
          do
          {
            current = toInternal(current)->children[0];
          } while (toLeaf(current) == nullptr);

          internal->keys.getArray()[position] = current->keys.getArray()[0];
          if (key == 8)
          {
            this->print();
            std::cout << "five position awwd" << position << " " << internal->keys.getArray()[position] << std::endl;
          }
        }
      }

      if (childLeaf && child->keys.size() > 0)
      {
        int position =
            toLeaf(leftSibling) != nullptr ? internal->keys.searchPosition(toLeaf(leftSibling)->keys.getArray().back()) : internal->keys.searchPosition(toLeaf(rightSibling)->keys.getArray().front()) - 1;

        if (position >= 0)
        {
          internal->keys.getArray()[position] = child->keys.getArray().front();
        }
      }

      if (childUnderflow)
      {

        if (leftSibling && leftSibling->canBorrow(this->order))
        {
          Key keyleft = leftSibling->keys.getArray().back();

          if (childLeaf)
          {
            toLeaf(child)->insertValue(keyleft, leftSibling->getValueByKey(keyleft));
            toLeaf(leftSibling)->removeValueByKey(keyleft);

            Key internKey = child->keys.getArray().front();
            int position = internal->keys.searchPosition(internKey);
            internal->keys.getArray()[position] = internKey;
            if (key == 5)
            {
              std::cout << position << " " << internal->keys.getArray()[position] << std::endl;
            }
          }
          else
          {
            auto nodeChildLeft = toInternal(leftSibling)->children.back();
            toInternal(leftSibling)->removeChildByKey(keyleft);

            int position = internal->keys.searchPosition(keyleft);
            toInternal(child)->insertChild(nodeChildLeft, toInternal(child)->children.front(), internal->keys.getArray()[position]);
            internal->keys.getArray()[position] = keyleft;
          }

          return false;
        }
        else if (rightSibling && rightSibling->canBorrow(this->order))
        {
          Key keyright = rightSibling->keys.getArray().front();

          if (childLeaf)
          {
            toLeaf(child)->insertValue(keyright, toLeaf(rightSibling)->getValueByKey(keyright));
            toLeaf(rightSibling)->removeValueByKey(keyright);

            Key internKey = child->keys.getArray().back();
            int position = internal->keys.searchPosition(key);
            internal->keys.getArray()[position] = internKey;
          }
          else
          {
            auto nodeChildRight = toInternal(rightSibling)->children.front();
            toInternal(rightSibling)->removeChildByKey(keyright, true);

            int position = internal->keys.searchPosition(keyright) - 1;
            toInternal(child)->insertChild(toInternal(child)->children.back(), nodeChildRight, internal->keys.getArray()[position]);
            internal->keys.getArray()[position] = keyright;
          }

          return false;
        }
        else
        {
          if (leftSibling)
          {
            if (childLeaf)
            {
              for (int i = 0; i < toLeaf(child)->keys.size(); i++)
              {
                Key k = toLeaf(child)->keys.getArray()[i];
                Value v = toLeaf(child)->values[i];
                toLeaf(leftSibling)->insertValue(k, v);
              }
              toLeaf(leftSibling)->next = toLeaf(child)->next;

              internal->removeChildByKey(toLeaf(child)->keys.getArray().front(), false);
            }
            else
            {
              auto internalLeft = toInternal(leftSibling);
              auto childInternal = toInternal(child);

              for (int i = 0; i < childInternal->children.size(); i++)
              {
                internalLeft->children.push_back(childInternal->children[i]);
              }

              Key keyleft = internalLeft->keys.getArray().back();
              int position = internal->keys.searchPosition(keyleft);
              internalLeft->keys.insertElement(internal->keys.getArray()[position]);

              for (int i = 0; i < childInternal->keys.size(); i++)
              {
                internalLeft->keys.insertElement(childInternal->keys.getArray()[i]);
              }
              internal->removeChildByKey(internal->keys.getArray()[position], false);

              if (internal == this->root && internal->keys.empty())
              {
                this->root = leftSibling;
                return false;
              }
            }
          }
          else if (rightSibling)
          {
            if (childLeaf)
            {
              for (int i = 0; i < toLeaf(rightSibling)->keys.size(); i++)
              {
                Key k = toLeaf(rightSibling)->keys.getArray()[i];
                Value v = toLeaf(rightSibling)->values[i];
                toLeaf(child)->insertValue(k, v);
              }
              toLeaf(child)->next = toLeaf(rightSibling)->next;

              internal->removeChildByKey(toLeaf(rightSibling)->keys.getArray().front(), false);
            }
            else
            {
              auto internalRight = toInternal(rightSibling);
              auto childInternal = toInternal(child);

              for (int i = 0; i < internalRight->children.size(); i++)
              {
                childInternal->children.push_back(internalRight->children[i]);
              }

              Key keyleft = childInternal->keys.getArray().back();
              int position = internal->keys.searchPosition(keyleft);
              childInternal->keys.insertElement(internal->keys.getArray()[position]);

              for (int i = 0; i < internalRight->keys.size(); i++)
              {
                childInternal->keys.insertElement(internalRight->keys.getArray()[i]);
              }

              internal->removeChildByKey(internal->keys.getArray()[position], false);

              if (internal == this->root && internal->keys.empty())
              {
                this->root = childInternal;
                return false;
              }
            }
          }

          if (internal->isUnderflow(order) && internal == root)
          {
            return false;
          }

          return internal->isUnderflow(order);
        }
      }

      return false;
    }
  }

  /**
   * Insert recursive with split operator
   * @param key The key to insert
   * @param value The value associated with the key
   * @return std::optional<std::pair<Key, Value>> Promoted key if split occurs, otherwise std::nullopt
   * @throws KeyAlreadyExistsException if the key already exists in the tree
   */
  std::optional<std::tuple<std::shared_ptr<BPlusTreeNode<Key, Value>>, std::shared_ptr<BPlusTreeNode<Key, Value>>, Key>> insertRecursive(const Key &key, const Value &value, std::shared_ptr<BPlusTreeNode<Key, Value>> node)
  {
    if (auto leafNode = toLeaf(node))
    {
      toLeaf(leafNode)->insertValue(key, value);
      if (leafNode->isOverflow(this->order))
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
      if (key == 8)
      {
        internalNode->print();
      }
      auto child = internalNode->getChildForKey(key);
      child->print();
      auto tupleInsert = insertRecursive(key, value, child);
      if (tupleInsert.has_value())
      {
        auto [firstNode, secondNode, promotedKey] = *tupleInsert;
        internalNode->insertChild(firstNode, secondNode, promotedKey);
        if (internalNode->isOverflow(this->order))
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

  /**
   * @brief Intenta castear un nodo genérico a hoja.
   * @param node Puntero al nodo base.
   * @return Puntero a leaf node o nullptr si no es hoja.
   */
  std::shared_ptr<BPlusTreeLeafNode<Key, Value>> toLeaf(
      const std::shared_ptr<BPlusTreeNode<Key, Value>> &node) const
  {
    return std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(node);
  }

  /**
   * @brief Intenta castear un nodo genérico a interno.
   * @param node Puntero al nodo base.
   * @return Puntero a internal node o nullptr si no es interno.
   */
  std::shared_ptr<BPlusTreeInternalNode<Key, Value>> toInternal(
      const std::shared_ptr<BPlusTreeNode<Key, Value>> &node) const
  {
    return std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(node);
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

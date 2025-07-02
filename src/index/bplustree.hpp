#pragma once

#include "./bplustree_node/bplustree_node.hpp"
#include "./bplustree_node/bplustree_leaf_node.hpp"
#include "./bplustree_node/bplustree_internal_node.hpp"
#include "../util/bplustree_exceptions.hpp"
#include "../util/event.hpp"
#include "bplustree_event.hpp"
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
  mutable Event<BPlusTreeEvent<Key, Value>> events;
  BPlusTree<Key, Value>(int m = 4);

  /**
   * Insert a key-value pair into the B+ tree
   * @param key The key to insert
   * @param value The value associated with the key
   * @return bool If the key has inserted
   */
  bool insert(const Key &key, const Value &value)
  {
    emitEvent(BPlusTreeEventType::INSERT_STARTED, key, value, "Inicio de inserción");

    if (!root)
    {
      root = std::make_shared<BPlusTreeLeafNode<Key, Value>>();
      rootLinked = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(root);
      auto leaf = std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(root);
      leaf->insertValue(key, value);

      emitEvent(BPlusTreeEventType::INSERT_COMPLETED, key, value, "Clave insertada correctamente");

      return true;
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
        emitEvent(BPlusTreeEventType::ROOT_REPLACED, std::nullopt, std::nullopt, "Nueva raíz tras split");
      }

      emitEvent(BPlusTreeEventType::INSERT_COMPLETED, key, value, "Clave insertada correctamente");

      return true;
    }
    catch (const KeyAlreadyExistsException &)
    {
      emitEvent(BPlusTreeEventType::INSERT_FAILED_EXISTS, key, value, "Clave ya existe");

      return false;
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
    emitEvent(BPlusTreeEventType::SEARCH_STARTED, key, std::nullopt, "Inicio de search");

    try
    {
      auto currentNode = this->root;
      while (currentNode && !std::dynamic_pointer_cast<BPlusTreeLeafNode<Key, Value>>(currentNode))
      {
        auto internalNode = std::dynamic_pointer_cast<BPlusTreeInternalNode<Key, Value>>(currentNode);
        currentNode = internalNode->getChildForKey(key);
      }
      auto value = currentNode->getValueByKey(key);

      emitEvent(BPlusTreeEventType::SEARCH_HIT, key, value, "Search hit");

      return value;
    }
    catch (const KeyNotFoundException &)
    {
      emitEvent(BPlusTreeEventType::SEARCH_MISS, key, std::nullopt, "Search miss");

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
    emitEvent(BPlusTreeEventType::REMOVE_STARTED, key, std::nullopt, "Inicio de remove");

    if (!root)
    {
      emitEvent(BPlusTreeEventType::REMOVE_FAILED_NOT_FOUND, key, std::nullopt, "Arbol vacio");
      return false;
    }

    try
    {
      auto hasRemoved = !removeRecursive(root, key);

      if (hasRemoved)
        emitEvent(BPlusTreeEventType::REMOVE_COMPLETED, key, std::nullopt, "Valor eliminado");
      else
        emitEvent(BPlusTreeEventType::REMOVE_FAILED_NOT_FOUND, key, std::nullopt, "Clave no encontrada");

      return hasRemoved;
    }
    catch (const KeyNotFoundException &)
    {
      emitEvent(BPlusTreeEventType::REMOVE_FAILED_NOT_FOUND, key, std::nullopt, "Clave no encontrada");

      return false;
    }
  };

  /**
   * Actualiza el valor asociado a una clave existente en el árbol B+.
   *
   * Si la clave se encuentra, su valor es reemplazado por el nuevo.
   * Se emiten eventos que indican el inicio, éxito o fallo de la operación.
   *
   * @param key La clave cuyo valor se desea actualizar.
   * @param newValue El nuevo valor a asociar a la clave.
   * @return true si la actualización fue exitosa, false si la clave no existe.
   */
  bool update(const Key &key, const Value &newValue)
  {
    emitEvent(BPlusTreeEventType::UPDATE_STARTED, key, newValue, "Inicio de update");
    try
    {
      auto node = root;

      while (node && !toLeaf(node))
      {
        node = toInternal(node)->getChildForKey(key);
      }

      toLeaf(node)->updateValueByKey(key, newValue);

      emitEvent(BPlusTreeEventType::UPDATE_COMPLETED, key, newValue, "Update completado");

      return true;
    }
    catch (const KeyNotFoundException &)
    {
      emitEvent(BPlusTreeEventType::UPDATE_FAILED_NOT_FOUND, key, std::nullopt, "Update fallido: clave no existe");

      return false;
    }
  }

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
  void emitEvent(
      BPlusTreeEventType type,
      std::optional<Key> key = std::nullopt,
      std::optional<Value> value = std::nullopt,
      const std::string &message = "") const
  {
    BPlusTreeEvent<Key, Value> evt{type, key, value, message};
    this->events.emit(evt);
  }

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

      bool underflow = leaf->isUnderflow(order);

      if (underflow)
        emitEvent(BPlusTreeEventType::TREE_EMPTY, std::nullopt, std::nullopt, "Underflow detectado");

      return underflow;
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

          emitEvent(BPlusTreeEventType::BORROW_FROM_LEFT, keyleft, std::nullopt, "Borrow desde nodo izquierdo");

          if (childLeaf)
          {
            toLeaf(child)->insertValue(keyleft, leftSibling->getValueByKey(keyleft));
            toLeaf(leftSibling)->removeValueByKey(keyleft);

            Key internKey = child->keys.getArray().front();
            int position = internal->keys.searchPosition(internKey);
            internal->keys.getArray()[position] = internKey;
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

          emitEvent(BPlusTreeEventType::BORROW_FROM_RIGHT, keyright, std::nullopt, "Borrow desde nodo derecho");

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

              emitEvent(BPlusTreeEventType::NODE_MERGED_LEAF, toLeaf(child)->keys.getArray().front(), std::nullopt, "Merge de nodos hoja");
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

              emitEvent(BPlusTreeEventType::NODE_MERGED_INTERNAL, keyleft, std::nullopt, "Merge de nodos internos");

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

              emitEvent(BPlusTreeEventType::NODE_MERGED_LEAF, toLeaf(child)->keys.getArray().front(), std::nullopt, "Merge de nodos hoja");
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

              emitEvent(BPlusTreeEventType::NODE_MERGED_INTERNAL, keyleft, std::nullopt, "Merge de nodos internos");

              if (internal == this->root && internal->keys.empty())
              {
                this->root = childInternal;

                emitEvent(BPlusTreeEventType::ROOT_REPLACED, std::nullopt, std::nullopt, "Reemplazo de raíz tras merge");

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
        auto splitResult = leafNode->split();

        emitEvent(BPlusTreeEventType::NODE_SPLIT_LEAF, std::get<2>(splitResult), std::nullopt, "Split de nodo hoja");

        return splitResult;
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
        if (internalNode->isOverflow(this->order))
        {
          auto splitResult = internalNode->split();

          emitEvent(BPlusTreeEventType::NODE_SPLIT_INTERNAL, std::get<2>(splitResult), std::nullopt, "Split de nodo interno");

          return splitResult;
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
  if (m < 4)
  {
    throw std::invalid_argument("El orden del árbol B+ debe ser al menos 4.");
  }
  this->order = m;
  this->root = nullptr;
  this->rootLinked = nullptr;
}

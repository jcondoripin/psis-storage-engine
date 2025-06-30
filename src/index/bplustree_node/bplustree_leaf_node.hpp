#pragma once

#include "bplustree_node.hpp"
#include "../../util/bplustree_exceptions.hpp"

/**
 * @brief Leaf node for a B+ Tree.
 *
 * Stores sorted keys and associated values.
 * Leaf nodes are linked together in a linked list.
 *
 * This class inherits from BPlusTreeNode and implements
 * methods for inserting, removing, and retrieving values
 * associated with keys.
 * @tparam Key Type of keys stored in the leaf node.
 * @tparam Value Type of values associated with the keys.
 */
template <typename Key, typename Value>
class BPlusTreeLeafNode : public BPlusTreeNode<Key, Value>
{
public:
  /// Values corresponding to keys
  std::vector<Value> values;

  /// Pointer to the next leaf node
  std::shared_ptr<BPlusTreeLeafNode<Key, Value>> next;

public:
  /**
   * @brief Constructor for a leaf node.
   */
  BPlusTreeLeafNode() : next(nullptr) {}

  /**
   * @brief Insert a key (and value for leaf) into the node.
   * This method ensures that the key is inserted in sorted order
   * and that the value is associated with the key.
   * @param key Key to insert.
   * @param value Value to insert.
   * @throws KeyAlreadyExistsException if the key already exists in the leaf node.
   * @throws EmptyArrayException if the leaf node is empty.
   * @note This method uses BPlusVector's searchPosition to find the correct
   */
  void insertValue(const Key &key, const Value &value)
  {
    if (this->keys.size() == 0 && values.size() == 0)
    {
      this->keys.insertElement(key);
      values.push_back(value);
      return;
    }

    int position = this->keys.searchElement(key);
    if (position < 0)
    {
      position = this->keys.searchPosition(key);
      this->keys.insertElement(key);
      values.insert(values.begin() + position, value);
      values[position] = value;
    }
    else
    {
      throw KeyAlreadyExistsException(std::to_string(key));
    }
  };

  /**
   * @brief Remove a key (and value for leaf) from the node.
   * @param key Key to remove.
   * @throws KeyNotFoundException if the key is not found in the leaf node.
   * @throws EmptyArrayException if the leaf node is empty.
   * @note This method uses BPlusVector's searchElement to find the key.
   */
  void removeValueByKey(const Key &key) override
  {
    int position = this->keys.searchElement(key);
    if (position >= 0)
    {
      this->keys.removeElementByPosition(position);
      values.erase(values.begin() + position);
    }
    else
    {
      throw KeyNotFoundException(std::to_string(key));
    }
  };

  /**
   * @brief Get the value associated with a key.
   * Only relevant for leaf nodes.
   * @param key Key to search.
   * @return Value if found.
   * @throws KeyNotFoundException if the key is not found in the leaf node.
   * @throws EmptyArrayException if the leaf node is empty.
   */
  Value getValueByKey(const Key &key) const override
  {
    int position = this->keys.searchElement(key);
    if (position >= 0)
    {
      return values[position];
    }
    else
    {
      throw KeyNotFoundException(std::to_string(key));
    }
  };

  /**
   * @brief Return a tuple with two nodes after splitting the leaf node.
   * This method creates a new leaf node and fills it with half of the keys and values
   * from the current node.
   * @param newNode The new node to fill with half of the keys and values.
   * @param order The tree's order.
   * @return A tuple containing the new leaf node and the key that will be promoted to the parent.
   * @throws EmptyLeafNodeException if the leaf node is empty.
   */
  std::tuple<std::shared_ptr<BPlusTreeNode<Key, Value>>, std::shared_ptr<BPlusTreeNode<Key, Value>>, Key> split() const override
  {
    if (this->keys.size() == 0)
    {
      throw EmptyLeafNodeException("Leaf node is empty, cannot split.");
    }

    int posMed = this->keys.size() / 2;
    
    auto keysNode = this->keys.getArray();

    Key promotedKey = keysNode[posMed];
    auto firstNode = std::make_shared<BPlusTreeLeafNode<Key, Value>>();
    auto secondNode = std::make_shared<BPlusTreeLeafNode<Key, Value>>();

    for (int i = 0; i < keysNode.size(); ++i)
    {
      if (i < posMed)
      {
        firstNode->insertValue(keysNode[i], values[i]);
      }
      else
      {
        secondNode->insertValue(keysNode[i], values[i]);
      }
    }

    return std::make_tuple(firstNode, secondNode, promotedKey);
  }
};
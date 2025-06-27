#pragma once

#include "bplustree_node.hpp"

/**
 * @brief Leaf node for a B+ Tree.
 *
 * Stores sorted keys and associated values.
 * Leaf nodes are linked together in a linked list.
 *
 * This class inherits from BPlusTreeNode and implements
 * the methods specific to leaf nodes, such as inserting and removing values.
 */
template <typename Key, typename Value>
class BPlusTreeLeafNode : public BPlusTreeNode<Key, Value>
{
private:
  /// Values corresponding to keys
  std::vector<Value> values;

  /// Pointer to the next leaf node
  std::shared_ptr<BPlusTreeLeafNode<Key, Value>> next;

  /**
   * @brief Constructor for a leaf node.
   */
  BPlusTreeLeafNode() : next(nullptr) {}

public:
  /**
   * @brief Insert a key (and value for leaf) into the node.
   * This method ensures that the key is inserted in sorted order
   * and that the value is associated with the key.
   * @param key Key to insert.
   * @param value Value to insert.
   * @throws std::runtime_error if the key already exists in the leaf node.
   * @note This method uses BPlusVector's searchPosition to find the correct
   */
  void insertValue(const Key &key, const Value &value)
  {
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
      throw std::runtime_error("Key already exists in leaf node");
    }
  };

  /**
   * @brief Remove a key (and value for leaf) from the node.
   * @param key Key to remove.
   * @throws std::runtime_error if the key is not found in the leaf node.
   * @note This method uses BPlusVector's searchElement to find the key.
   */
  void removeValueByKey(const Key &key) override
  {
    int position = this->keys.searchElement(key);
    if (position >= 0)
    {
      this->keys.removeElement(position);
      values.erase(values.begin() + position);
    }
    else
    {
      throw std::runtime_error("Key not found in leaf node");
    }
  };

  /**
   * @brief Get the value associated with a key.
   * Only relevant for leaf nodes.
   * @param key Key to search.
   * @return Value if found.
   * @throws std::runtime_error if the key is not found in the leaf node.
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
      throw std::runtime_error("Key not found in leaf node");
    }
  };
};
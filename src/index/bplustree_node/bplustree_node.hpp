#pragma once

#include "../bplus_vector.hpp"
#include <memory>
#include <vector>
#include <tuple>

/**
 * @brief Abstract base class for a B+ Tree node.
 *
 * Defines the basic structure and common interface for internal and leaf nodes.
 *
 * @tparam Key Type of the key (must support comparison operators).
 * @tparam Value Type of the value (only relevant for leaf nodes).
 */
template <typename Key, typename Value>
class BPlusTreeNode
{
public:
  /// Sorted keys managed by BPlusVector
  BPlusVector<Key> keys;

  /**
   * @brief Virtual destructor.
   */
  virtual ~BPlusTreeNode() = default;

  /**
   * @brief Returns true if the node is full (keys > order - 1).
   * @param order The tree's order.
   * @return True if full.
   */
  bool isOverflow(int order) const
  {
    return keys.size() > order - 1;
  }

  /**
   * @brief Returns true if the node is full (keys >= order - 1).
   * @param order The tree's order.
   * @return True if underflow.
   */
  bool isUnderflow(int order) const
  {
    return keys.size() < (order - 1) / 2;
  }

  /**
   * @brief Returns true if the node can borrow.
   * @param order The tree's order.
   * @return True if can.
   */
  bool canBorrow(int order) const
  {
    return (keys.size() - 1) >= (order - 1) / 2;
  }

  /**
   * @brief Get the value associated with a key.
   * Only relevant for leaf nodes.
   * @param key Key to search.
   * @return Value if found.
   */
  virtual Value getValueByKey(const Key &key) const = 0;

  /**
   * @brief Print the contents of the node.
   * This method is for debugging purposes and prints the keys in the node.
   */
  virtual void print()
  {
    for (auto &key : keys.getArray())
    {
      std::cout << key << " ";
    }
    std::cout << std::endl;
  }

  /**
   * @brief Return a tuple with two nodes after split the root node.
   * @param newNode The new node to fill with half of the keys and values.
   * @param order The tree's order.
   * @return
   * @throws EmptyArrayException if the node is empty.
   */
  virtual std::tuple<std::shared_ptr<BPlusTreeNode<Key, Value>>, std::shared_ptr<BPlusTreeNode<Key, Value>>, Key> split() = 0;
};
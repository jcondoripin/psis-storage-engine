#pragma once

#include "../bplus_vector.hpp"
#include <memory>
#include <vector>

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
protected:
  /// Sorted keys managed by BPlusVector
  BPlusVector<Key> keys;

  /**
   * @brief Virtual constructor.
   */
  virtual BPlusTreeNode() = default;

  /**
   * @brief Virtual destructor.
   */
  virtual ~BPlusTreeNode() = default;

  /**
   * @brief Returns true if the node is full (keys >= order - 1).
   * @param order The tree's order.
   * @return True if full.
   */
  bool isFull(int order) const
  {
    return keys.size() >= order - 1;
  }

  /**
   * @brief Remove a key (and value for leaf) from the node.
   * @param key Key to remove.
   */
  virtual void removeValueByKey(const Key &key) = 0;

  /**
   * @brief Get the value associated with a key.
   * Only relevant for leaf nodes.
   * @param key Key to search.
   * @return Value if found.
   */
  virtual Value getValueByKey(const Key &key) const = 0;
};
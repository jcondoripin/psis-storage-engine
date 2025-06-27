#pragma once

#include "bplustree_node.hpp"

/**
 * @brief Internal node for a B+ Tree.
 *
 * Stores sorted keys and child pointers.
 */
template <typename Key, typename Value>
class BPlusTreeInternalNode : public BPlusTreeNode<Key, Value>
{
public:
  /// Pointers to child nodes
  std::vector<std::shared_ptr<BPlusTreeNode<Key, Value>>> children;

  /**
   * @brief Insert a key and child pointer into the internal node.
   * This method ensures that the key is inserted in sorted order
   * and that the child pointer is associated with the key.
   * @param key Key to insert.
   * @param child Pointer to the child node.
   */
  void insertChild(const Key &key, std::shared_ptr<BPlusTreeNode<Key, Value>> child)
  {
    int position = this->keys.searchPosition(key);
    this->keys.insertElement(key);
    children.insert(children.begin() + position, child);
  }
};
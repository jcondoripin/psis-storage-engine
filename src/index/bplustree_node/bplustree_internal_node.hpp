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
  /// Pointers to child nodes, always keys + 1
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
    if (this->keys.size() == 0 && children.size() == 0)
    {
      this->keys.insertElement(key);
      children.push_back(child);
      return;
    }
    int position = this->keys.searchPosition(key);
    this->keys.insertElement(key);
    children.insert(children.begin() + position, child);
  }

  /**
   * Search key and return the child node pointer.
   * @param key Key to search for.
   * @return Pointer to the child node if found, or nullptr if not found.
   * @throws KeyNotFoundException if the key is not found in the internal node.
   */
  std::shared_ptr<BPlusTreeNode<Key, Value>> getChildForKey(const Key &key) const
  {
    int position = this->keys.searchElement(key);
    if (position != -1)
    {
      return children[position + 1];
    }
    else
    {
      return children[this->keys.searchPosition(key)];
    }
  }
};
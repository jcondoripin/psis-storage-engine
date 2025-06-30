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
   * @brief Remove a key for an internal node.
   */
  void removeValueByKey(const Key &) override
  {
    throw std::runtime_error("Cannot remove value by key in internal node.");
  }

  /**
   * @brief Get the value associated with a key.
   */
  Value getValueByKey(const Key&) const override
  {
    throw std::runtime_error("Cannot get value by key in internal node.");
  }


  /**
   * @brief Insert a key and child pointer into the internal node.
   * This method ensures that the key is inserted in sorted order
   * and that the child pointer is associated with the key.
   * @param key Key to insert.
   * @param child Pointer to the child node.
   */
  void insertChild(std::shared_ptr<BPlusTreeNode<Key, Value>> firstNode, std::shared_ptr<BPlusTreeNode<Key, Value>> secondNode, const Key &key)
  {
    if (this->keys.size() == 0)
    {
      this->keys.insertElement(key);
      children.push_back(firstNode);
      children.push_back(secondNode);
      return;
    }

    int position = this->keys.searchElement(key);
    if (position < 0)
    {
      position = this->keys.searchPosition(key);
      this->keys.insertElement(key);
      children.insert(children.begin() + position + 1, secondNode);
      children[position + 1] = secondNode;
      children[position] = firstNode;
    }
    else
    {
      throw KeyAlreadyExistsException("Key already exists in internal node: " + std::to_string(key));
    }
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

  /**
   * @brief Return a tuple with two nodes after splitting the internal node.
   * This method creates a new internal node and fills it with half of the keys and values
   * from the current node.
   * @param newNode The new node to fill with half of the keys and values.
   * @param order The tree's order.
   * @return A tuple containing the new internal node and the key that will be promoted to the parent.
   * @throws EmptyLeafNodeException if the internal node is empty.
   */
  std::tuple<std::shared_ptr<BPlusTreeNode<Key, Value>>, std::shared_ptr<BPlusTreeNode<Key, Value>>, Key> split() override
  {
    if (this->keys.size() == 0)
    {
      throw EmptyArrayException("Internal node is empty, cannot split.");
    }

    int posMed = this->keys.size() / 2;
    auto keysNode = this->keys.getArray();

    Key promotedKey = keysNode[posMed];
    auto firstNode = std::make_shared<BPlusTreeInternalNode<Key, Value>>();
    auto secondNode = std::make_shared<BPlusTreeInternalNode<Key, Value>>();

    for (int i = 0; i < keysNode.size(); ++i)
    {
      if (i < posMed)
      {
        firstNode->insertChild(children[i], children[i + 1], keysNode[i]);
      }
      else if (i > posMed)
      {
        secondNode->insertChild(children[i], children[i + 1], keysNode[i]);
      }
    }

    return std::make_tuple(firstNode, secondNode, promotedKey);
  }
};
#pragma once
#include <vector>
#include <memory>
#include <optional>

template <typename Key, typename Value>
class Node
{
public:
  bool isLeaf;
  std::vector<Key> keys;

  explicit Node(bool leaf) : isLeaf(leaf) {}
  virtual ~Node() = default;

  virtual bool isFull(int order) const = 0;
};

template <typename Key, typename Value>
class LeafNode : public Node<Key, Value>
{
public:
  std::vector<Value> values;
  LeafNode *next;

  LeafNode() : Node<Key, Value>(true), next(nullptr) {}

  bool isFull(int order) const override
  {
    return this->keys.size() >= order - 1;
  }
};

template <typename Key, typename Value>
class InternalNode : public Node<Key, Value>
{
public:
  std::vector<std::shared_ptr<Node<Key, Value>>> children;

  InternalNode() : Node<Key, Value>(false) {}

  bool isFull(int order) const override
  {
    return this->keys.size() >= order - 1;
  }
};

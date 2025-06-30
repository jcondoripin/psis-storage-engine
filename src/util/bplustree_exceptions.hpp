#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>

/**
 * @brief Exception thrown when a key already exists in the B+ Tree leaf node.
 */
class KeyAlreadyExistsException : public std::runtime_error
{
public:
  explicit KeyAlreadyExistsException(const std::string &key)
      : std::runtime_error("Key already exists in leaf node: " + key) {}
};

/**
 * @brief Exception thrown when a key is not found in the B+ Tree node.
 */
class KeyNotFoundException : public std::runtime_error
{
public:
  explicit KeyNotFoundException(const std::string &key)
      : std::runtime_error("Key not found in node: " + key) {}
};

/**
 * @brief Exception thrown when the B+ Tree leaf node is full and cannot accommodate more keys.
 */
class LeafNodeFullException : public std::runtime_error
{
public:
  explicit LeafNodeFullException(int order)
      : std::runtime_error("Leaf node is full, cannot insert more keys. Order: " + std::to_string(order)) {}
};

/**
 * @brief Exception thrown when an operation is attempted on an empty B+ Tree leaf node.
 */
class EmptyLeafNodeException : public std::runtime_error
{
public:
  EmptyLeafNodeException()
      : std::runtime_error("Operation cannot be performed on an empty leaf node.") {}

  EmptyLeafNodeException(const std::string &message)
      : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when a split operation fails in the B+ Tree leaf node.
 */
class LeafNodeSplitException : public std::runtime_error
{
public:
  explicit LeafNodeSplitException(const std::string &message)
      : std::runtime_error("Leaf node split failed: " + message) {}
};

/**
 * @brief Exception thrown when an invalid operation is attempted on the B+ Tree leaf node.
 */
class InvalidLeafNodeOperationException : public std::runtime_error
{
public:
  explicit InvalidLeafNodeOperationException(const std::string &message)
      : std::runtime_error("Invalid operation on leaf node: " + message) {}
};
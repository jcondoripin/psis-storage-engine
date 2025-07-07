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
  explicit KeyNotFoundException()
      : std::runtime_error("Key not found in node ") {}

  explicit KeyNotFoundException(const std::string &key)
      : std::runtime_error("Key not found in node: " + key) {}
};

/**
 * @brief Exception thrown when a child is not found in a Internal node.
 */
class ChildNotFoundException : public std::runtime_error
{
public:
  explicit ChildNotFoundException()
      : std::runtime_error("Child not found in node ") {}
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


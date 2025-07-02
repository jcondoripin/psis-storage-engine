#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>

/**
 * @brief Exception thrown when a database table already exists.
 */
class DatabaseTableAlreadyExistsException : public std::runtime_error
{
public:
  explicit DatabaseTableAlreadyExistsException(const std::string &table)
      : std::runtime_error("Database table already exists: " + table) {}
};

/**
 * @brief Exception thrown when a database table doesn't exist.
 */
class DatabaseTableNotFoundException : public std::runtime_error
{
public:
  explicit DatabaseTableNotFoundException(const std::string &table)
      : std::runtime_error("Database table not found: " + table) {}
};

/**
 * @brief Exception thrown when a database table doesn't exist.
 */
class TypeMismatchException : public std::runtime_error
{
public:
  explicit TypeMismatchException(const std::string &table)
      : std::runtime_error("Type mismatch for table: " + table) {}
};

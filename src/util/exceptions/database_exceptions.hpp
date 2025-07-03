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

/**
 * @brief Exception thrown when a column already exists.
 */
class ColumnAlreadyExistsException : public std::runtime_error
{
public:
  explicit ColumnAlreadyExistsException(const std::string &column)
      : std::runtime_error("Column already exists in the table: " + column) {}
};

/**
 * @brief Exception thrown when a column doesn't exist.
 */
class ColumnNotFoundException : public std::runtime_error
{
public:
  explicit ColumnNotFoundException(const std::string &column)
      : std::runtime_error("Column not found: " + column) {}
};

/**
 * @brief Exception thrown when a column doesn't exist.
 */
class DatabaseRecordNotFoundException : public std::runtime_error
{
public:
  explicit DatabaseRecordNotFoundException(const std::string &column, const int64_t &key)
      : std::runtime_error("Record for key: " + std::to_string(key) + " not found in column: " + column) {}
};

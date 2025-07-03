#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "../index/bplustree.hpp"
#include "../util/exceptions/database_exceptions.hpp"
#include "../util/table.hpp"

/**
 * Operaciones soportadas para logging.
 */
enum class Operation
{
  CREATE_TABLE,
  INSERT,
  UPDATE,
  REMOVE,
  SEARCH,
  BACKUP,
};

inline std::string operationToString(Operation op)
{
  switch (op)
  {
  case Operation::CREATE_TABLE:
    return "CREATE_TABLE";
  case Operation::INSERT:
    return "INSERT";
  case Operation::UPDATE:
    return "UPDATE";
  case Operation::REMOVE:
    return "REMOVE";
  case Operation::SEARCH:
    return "SEARCH";
  case Operation::BACKUP:
    return "BACKUP";
  default:
    return "UNKNOWN";
  }
}

Operation stringToOperation(const std::string &str)
{
  if (str == "CREATE_TABLE")
    return Operation::CREATE_TABLE;
  if (str == "INSERT")
    return Operation::INSERT;
  if (str == "UPDATE")
    return Operation::UPDATE;
  if (str == "REMOVE")
    return Operation::REMOVE;
  if (str == "SEARCH")
    return Operation::SEARCH;
  if (str == "BACKUP")
    return Operation::BACKUP;
  throw std::invalid_argument("Unknown Operation string: " + str);
}
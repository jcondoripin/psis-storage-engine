#pragma once

#include "record.hpp"
#include "operation.hpp"

struct ArgsQuery {
  Operation op = Operation::QUERY;
  std::string tableName;
  Record filter;
};

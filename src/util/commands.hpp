#pragma once

#include "operation.hpp"
#include "record.hpp"

struct ArgsCommand
{
  Operation command;
  std::string tableName;
};

struct ArgsCommandSelect : ArgsCommand
{
  Record record;
};

struct ArgsCommandGet : ArgsCommand
{
  int64_t key;
};

struct ArgsCommandCreate : ArgsCommand
{
  Table schema;
};

struct ArgsCommandInsert : ArgsCommand
{
  Record record;
};

struct ArgsCommandUpdate : ArgsCommand
{
  int64_t key;
  Record record;
};

struct ArgsCommandDelete : ArgsCommand
{
  int64_t key;
};

struct ArgsCommandSub : ArgsCommand
{
  SOCKET client = 0;
};

struct ArgsCommandQuery: ArgsCommand {
  Record filter;
};

struct ArgsCommandGeneral
{
  std::optional<ArgsCommandCreate> create = std::nullopt;
  std::optional<ArgsCommandGet> get = std::nullopt;
  std::optional<ArgsCommandSelect> select = std::nullopt;
  std::optional<ArgsCommandInsert> insert = std::nullopt;
  std::optional<ArgsCommandUpdate> update = std::nullopt;
  std::optional<ArgsCommandDelete> remove = std::nullopt;
  std::optional<ArgsCommandSub> sub = std::nullopt;
  std::optional<ArgsCommandQuery> query = std::nullopt;

};
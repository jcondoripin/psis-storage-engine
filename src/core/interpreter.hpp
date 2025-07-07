#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include "../util/exceptions/general_exceptions.hpp"
#include "../util/column.hpp"
#include "../storage/database_node.hpp"
#include "../storage/database_node_flb.hpp"
#include "engine.hpp"

namespace parser_engine
{
  using namespace std;

  const regex CREATE_PATTERN(R"(CREATE\s+(\w+)\s+(\d+)\s+(.*))");
  const regex SEARCH_PATTERN(R"(SELECT\s+(\w+)\s+(\d+))");
  const regex INSERT_PATTERN(R"(INSERT\s+(\w+)\s+(\w+:(?:"[^\"]*"|[^:\s]+):\w+(?:\s+\w+:(?:"[^\"]*"|[^:\s]+):\w+)*))");
  const regex UPDATE_PATTERN(R"(UPDATE\s+(\w+)\s+(\d+)\s+(\w+:(?:"[^\"]*"|[^:\s]+):\w+(?:\s+\w+:(?:"[^\"]*"|[^:\s]+):\w+)*))");
  const regex DELETE_PATTERN(R"(DELETE\s+(\w+)\s+(\d+))");

  ArgsCommand _read_create(const smatch &match);
  ArgsCommand _read_search(const smatch &match);
  ArgsCommand _read_insert(const smatch &match);
  ArgsCommand _read_update(const smatch &match);
  ArgsCommand _read_delete(const smatch &match);

  ArgsCommand readCommand(string yytext)
  {
    smatch match;
    if (regex_match(yytext, match, CREATE_PATTERN))
      return _read_create(match);
    if (regex_match(yytext, match, INSERT_PATTERN))
      return _read_insert(match);
    if (regex_match(yytext, match, SEARCH_PATTERN))
      return _read_search(match);
    if (regex_match(yytext, match, UPDATE_PATTERN))
      return _read_update(match);
    if (regex_match(yytext, match, DELETE_PATTERN))
      return _read_delete(match);

    throw PatternException("Comando no reconocido o mal formado.");
  }

  ArgsCommand _read_create(const smatch &match)
  {
    string tableName = match[1];
    int keyColumn = stoi(match[2]);
    string schemaStr = match[3];

    Table t;

    stringstream ss(schemaStr);
    string token;

    while (ss >> token)
    {
      auto pos = token.find(':');
      if (pos == string::npos)
        throw runtime_error("Error en definición de columna: " + token);

      string col = token.substr(0, pos);
      string kind = token.substr(pos + 1);
      t.addColumn({col, stringToKindColumn(kind)});
    }

    t.setKeyColumn(keyColumn);

    return ArgsCommand{
        Operation::CREATE_TABLE,
        tableName,
        0,
        t,
        {}};
  }

  ArgsCommand _read_search(const smatch &match)
  {
    string tableName = match[1];
    int id = stoi(match[2]);

    return ArgsCommand{
        Operation::SEARCH,
        tableName,
        id,
        {},
        {}};
  }

  ArgsCommand _read_insert(const smatch &match)
  {
    string tableName = match[1];
    string valuesStr = match[2];

    Record r;
    size_t pos = 0;

    while (pos < valuesStr.size())
    {
      size_t p1 = valuesStr.find(':', pos);
      if (p1 == string::npos)
        throw runtime_error("❌ Falta ':' después de columna");
      string col = valuesStr.substr(pos, p1 - pos);
      pos = p1 + 1;

      string val;
      if (valuesStr[pos] == '"')
      {
        size_t p2 = valuesStr.find('"', pos + 1);
        if (p2 == string::npos)
          throw runtime_error("❌ Comillas no cerradas en valor");
        val = valuesStr.substr(pos + 1, p2 - pos - 1);
        pos = p2 + 1;
        if (valuesStr[pos] != ':')
          throw runtime_error("❌ Falta ':' después de valor con comillas");
        ++pos;
      }
      else
      {
        size_t p2 = valuesStr.find(':', pos);
        if (p2 == string::npos)
          throw runtime_error("❌ Falta ':' después de valor");
        val = valuesStr.substr(pos, p2 - pos);
        pos = p2 + 1;
      }

      size_t next = valuesStr.find(' ', pos);
      string kind = valuesStr.substr(pos, next - pos);
      pos = (next == string::npos) ? valuesStr.size() : next + 1;

      r.values.push_back({col, val, kind});
    }

    return ArgsCommand{
        Operation::INSERT,
        tableName,
        0,
        {},
        r};
  }

  ArgsCommand _read_update(const smatch &match)
  {
    string tableName = match[1];
    int id = stoi(match[2]);
    string updateStr = match[3];

    Record r;
    size_t pos = 0;

    while (pos < updateStr.size())
    {
      size_t p1 = updateStr.find(':', pos);
      if (p1 == string::npos)
        throw runtime_error("❌ Falta ':' después de columna");
      string col = updateStr.substr(pos, p1 - pos);
      pos = p1 + 1;

      string val;
      if (updateStr[pos] == '"')
      {
        size_t p2 = updateStr.find('"', pos + 1);
        if (p2 == string::npos)
          throw runtime_error("❌ Comillas no cerradas en valor");
        val = updateStr.substr(pos + 1, p2 - pos - 1);
        pos = p2 + 1;
        if (updateStr[pos] != ':')
          throw runtime_error("❌ Falta ':' después de valor con comillas");
        ++pos;
      }
      else
      {
        size_t p2 = updateStr.find(':', pos);
        if (p2 == string::npos)
          throw runtime_error("❌ Falta ':' después de valor");
        val = updateStr.substr(pos, p2 - pos);
        pos = p2 + 1;
      }

      size_t next = updateStr.find(' ', pos);
      string kind = updateStr.substr(pos, next - pos);
      pos = (next == string::npos) ? updateStr.size() : next + 1;

      r.values.push_back({col, val, kind});
    }

    return ArgsCommand{
        Operation::UPDATE,
        tableName,
        id,
        {},
        r};
  }

  ArgsCommand _read_delete(const smatch &match)
  {
    string tableName = match[1];
    int id = stoi(match[2]);

    return ArgsCommand{
        Operation::REMOVE,
        tableName,
        id,
        {},
        {}};
  }
}

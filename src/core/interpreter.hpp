#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include "../util/exceptions/general_exceptions.hpp"
#include "../util/column.hpp"
#include "../util/record.hpp"
#include "../storage/database_node.hpp"
#include "../storage/database_node_flb.hpp"
#include "../util/query_args.hpp"


/**
 * @brief Espacio de nombres encargado de interpretar y parsear los comandos tipo texto.
 *
 * Este parser convierte líneas de texto en estructuras `ArgsCommand*` que el motor ejecutará.
 */
namespace parser_engine
{
  using namespace std;

  /// Expresión regular para comando CREATE <tabla> <columna_clave> <esquema>
  const regex CREATE_PATTERN(R"(CREATE\s+(\w+)\s+(\d+)\s+(.*))");

  /// Expresión regular para comando GET <tabla> <clave>
  const regex GET_PATTERN(R"(GET\s+(\w+)\s+(\d+))");

  /// Expresión regular para comando INSERT <tabla> <columna:valor:tipo ...>
  const regex INSERT_PATTERN(R"(INSERT\s+(\w+)\s+(\w+:(?:"[^\"]*"|[^:\s]+):\w+(?:\s+\w+:(?:"[^\"]*"|[^:\s]+):\w+)*))");

  /// Expresión regular para comando UPDATE <tabla> <clave> <columna:valor:tipo ...>
  const regex UPDATE_PATTERN(R"(UPDATE\s+(\w+)\s+(\d+)\s+(\w+:(?:"[^\"]*"|[^:\s]+):\w+(?:\s+\w+:(?:"[^\"]*"|[^:\s]+):\w+)*))");

  /// Expresión regular para comando DELETE <tabla> <clave>
  const regex DELETE_PATTERN(R"(DELETE\s+(\w+)\s+(\d+))");

  /// Expresión regular para comando SUB <tabla>
  const regex SUB_PATTERN(R"(SUB\s+(\w+))");

  /// Expresión regular para comando SELECT <tabla> WHERE <filtros>
  const regex QUERY_PATTERN(
  R"(SELECT\s+(\w+)\s+WHERE\s+((?:(?:\w*):(?:"[^"]*"|[^:\s]+):\w+)(?:\s+(?:\w*):(?:"[^"]*"|[^:\s]+):\w+)*))"
);





  ArgsCommandCreate _read_create(const smatch &match);
  ArgsCommandGet _read_get(const smatch &match);
  ArgsCommandInsert _read_insert(const smatch &match);
  ArgsCommandUpdate _read_update(const smatch &match);
  ArgsCommandDelete _read_delete(const smatch &match);
  ArgsCommandSub _read_sub(const smatch &match);

  



  /**
   * @brief Parsea un comando en texto plano y retorna el argumento estructurado.
   *
   * @param yytext Cadena del comando (ej: "GET users 2").
   * @return ArgsCommandGeneral Contenedor con el comando detectado.
   * @throws PatternException si el formato no es reconocido.
   */

  // Las funciones _read_create, _read_get, _read_insert, _read_update, _read_delete, _read_sub se mantienen igual
  std::string trimAll(const std::string &input) {
  std::string output;
  bool first = true;
  for (char c : input) {
    if (first && c == '>') {
      first = false;
      continue; // saltar '>'
    }
    if (c != '\n' && c != '\r') {
      output += c;
    }
    first = false;
  }
  return output;
}


ArgsQuery _read_query(const smatch &match)
{
  string tableName = match[1];
  string filters = match[2];

  std::cout << "🧪 _read_query: tabla=" << tableName << " | filtros='" << filters << "'\n";

  Record r;
  stringstream ss(filters);
  string token;

  while (ss >> token)
  {
    auto parts = std::count(token.begin(), token.end(), ':');
    if (parts == 2) {
      // columna:valor:tipo
      auto pos1 = token.find(':');
      auto pos2 = token.rfind(':');
      string column = token.substr(0, pos1);
      string value = token.substr(pos1 + 1, pos2 - pos1 - 1);
      string kind = token.substr(pos2 + 1);
      r.values.push_back({column, value, kind});
    }
    else if (parts == 1) {
      // valor:tipo → usar columna vacía como comodín
      auto pos = token.find(':');
      string value = token.substr(0, pos);
      string kind = token.substr(pos + 1);
      r.values.push_back({"", value, kind});
    }
    else {
      throw runtime_error("❌ Filtro mal formado: " + token);
    }
  }

  return ArgsQuery{Operation::QUERY, tableName, r};
}


  
  
  
  ArgsCommandGeneral readCommand(string yytext, SOCKET client)
 {
  std::cout << "⏩ Recibido crudo: ";
  for (char c : yytext)
    std::cout << "[" << (int)c << "]";  // Imprime el valor ASCII de cada carácter
  std::cout << std::endl;

  yytext = trimAll(yytext);
 std::cout << "⏩ Luego de trim: ";
for (char c : yytext)
  std::cout << "[" << (int)c << "]";
std::cout << std::endl;


  smatch match;
  ArgsCommandGeneral general;

  if (regex_match(yytext, match, CREATE_PATTERN))
    general.create = _read_create(match);
  else if (regex_match(yytext, match, INSERT_PATTERN))
    general.insert = _read_insert(match);
  else if (regex_match(yytext, match, GET_PATTERN))
    general.get = _read_get(match);
  else if (regex_match(yytext, match, UPDATE_PATTERN))
    general.update = _read_update(match);
  else if (regex_match(yytext, match, DELETE_PATTERN))
    general.remove = _read_delete(match);
  else if (regex_match(yytext, match, SUB_PATTERN))
  {
    auto command = _read_sub(match);
    command.client = client;
    general.sub = command;
  }
  else if (regex_match(yytext, match, QUERY_PATTERN))
  {
    std::cout << "✅ MATCH SELECT/WHERE" << std::endl;
    auto command = _read_query(match);
    general.query = std::make_optional(command);
  }
  else
    throw PatternException("Comando no reconocido o mal formado.");

  return general;
}

 
  

  /**
   * @brief Parsea un comando CREATE y genera ArgsCommandCreate.
   */
  ArgsCommandCreate _read_create(const smatch &match)
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

    return ArgsCommandCreate{Operation::CREATE_TABLE, tableName, t};
  }

  





  

  /**
   * @brief Parsea un comando GET y genera ArgsCommandGet.
   */
  ArgsCommandGet _read_get(const smatch &match)
  {
    string tableName = match[1];
    int64_t id = stoi(match[2]);

    return ArgsCommandGet{Operation::GET, tableName, id};
  }

  /**
   * @brief Parsea un comando INSERT y genera ArgsCommandInsert.
   */
  ArgsCommandInsert _read_insert(const smatch &match)
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

    return ArgsCommandInsert{Operation::INSERT, tableName, r};
  }

  /**
   * @brief Parsea un comando UPDATE y genera ArgsCommandUpdate.
   */
  ArgsCommandUpdate _read_update(const smatch &match)
  {
    string tableName = match[1];
    int64_t id = stoi(match[2]);
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

    return ArgsCommandUpdate{Operation::UPDATE, tableName, id, r};
  }

  /**
   * @brief Parsea un comando DELETE y genera ArgsCommandDelete.
   */
  ArgsCommandDelete _read_delete(const smatch &match)
  {
    string tableName = match[1];
    int64_t id = stoi(match[2]);

    return ArgsCommandDelete{Operation::REMOVE, tableName, id};
  }

  /**
   * @brief Parsea un comando SUB y genera ArgsCommandSub.
   */
  ArgsCommandSub _read_sub(const smatch &match)
  {
    string tableName = match[1];

    return ArgsCommandSub{Operation::SUB, tableName};
  } 

}

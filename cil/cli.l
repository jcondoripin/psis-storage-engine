%{
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <iomanip>
#include "../src/storage/database_node.hpp"
#include "../src/storage/database_node_flb.hpp"

DatabaseNode db("cli_data/");
using namespace std;

KindColumn kindFromString(const std::string& s) {
  if (s == "INT") return KindColumn::INT;
  if (s == "TEXT") return KindColumn::TEXT;
  if (s == "DOUBLE") return KindColumn::DOUBLE;
  if (s == "LONG") return KindColumn::LONG;
  if (s == "BOOL") return KindColumn::BOOL;
  if (s == "DATE") return KindColumn::DATE;
  throw std::invalid_argument("Tipo inválido: " + s);
}
%}

%%

CREATE[^\n]* {
  string line(yytext);
  smatch match;
  regex pattern(R"(CREATE\s+(\w+)\s+(.*))");

  if (regex_match(line, match, pattern)) {
    string tableName = match[1];
    string schemaStr = match[2];
    Table t;
    stringstream ss(schemaStr);
    string token;

    while (ss >> token) {
      auto pos = token.find(':');
      if (pos == string::npos) {
        cout << "❌ Error en columna: " << token << endl;
        return 0;
      }
      string col = token.substr(0, pos);
      string kind = token.substr(pos + 1);
      t.addColumn({col, kindFromString(kind)});
    }

    db.createTable(tableName, t, 0);
    cout << "✅ Tabla '" << tableName << "' creada.\n";
  } else {
    cout << "❌ Error de sintaxis CREATE.\n";
  }
}

INSERT[^\n]* {
  string line(yytext);
  smatch match;
  regex pattern(R"(INSERT\s+(\w+)\s+(.+))");

  if (regex_match(line, match, pattern)) {
    string tableName = match[1];
    string valuesStr = match[2];

    Table schema = flb::loadSchemaFromFile("cli_data/", tableName);
    auto cols = schema.getColumnNames();
    stringstream ss(valuesStr);
    string val;
    vector<string> tokens;

    while (ss >> quoted(val)) tokens.push_back(val);

    if (tokens.size() != cols.size()) {
      cout << "❌ Número de valores incorrecto.\n";
      return 0;
    }

    Record r;
    for (size_t i = 0; i < cols.size(); ++i) {
      KindColumn k = schema.getColumnKind(cols[i]);
      r.push_back({cols[i], tokens[i], kindColumnToString(k)});
    }

    db.insert(tableName, r);
    cout << "✅ Insertado en '" << tableName << "'.\n";
  } else {
    cout << "❌ Error de sintaxis INSERT.\n";
  }
}

SELECT[^\n]* {
  string line(yytext);
  smatch match;
  regex pattern(R"(SELECT\s+(\w+)\s+(\d+))");

  if (regex_match(line, match, pattern)) {
    string name = match[1];
    int id = stoi(match[2]);
    cout << "🔍 SELECT en '" << name << "' con id=" << id << endl;

    try {
      auto res = db.search(name, id);
      if (res) {
        for (auto &v : *res)
          cout << v.column << "=" << v.value << " ";
        cout << endl;
      } else {
        cout << "❌ No encontrado\n";
      }
    } catch (const EmptyArrayException &e) {
      cout << "⚠️ Árbol vacío: " << e.what() << endl;
    } catch (const std::exception &e) {
      cout << "⚠️ Excepción: " << e.what() << endl;
    }

  } else {
    cout << "❌ Error de sintaxis SELECT\n";
  }
}


DELETE[^\n]* {
  string line(yytext);
  smatch match;
  regex pattern(R"(DELETE\s+(\w+)\s+(\d+))");

  if (regex_match(line, match, pattern)) {
    string name = match[1];
    int id = stoi(match[2]);

    Table schema = flb::loadSchemaFromFile("cli_data/", name);
    auto allData = flb::loadTableData("cli_data/", name, schema);

    bool found = false;
    vector<Record> updatedData;
    for (const auto& rec : allData) {
      if (rec[0].value != to_string(id)) {
        updatedData.push_back(rec);
      } else {
        found = true;
      }
    }

    if (found) {
      db.remove(name, id);
      flb::saveTableData("cli_data/", name, schema, updatedData);
      cout << "🗑️ Registro eliminado\n";
    } else {
      cout << "❌ No se encontró el registro\n";
    }
  } else {
    cout << "❌ Error de sintaxis DELETE\n";
  }
}

UPDATE[^\n]* {
  string line(yytext);
  smatch match;
  regex pattern(R"(UPDATE\s+(\w+)\s+(\d+)\s+(\w+)\s+\"([^\"]+)\")");

  if (regex_match(line, match, pattern)) {
    string name = match[1];
    int id = stoi(match[2]);
    string col = match[3];
    string newVal = match[4];

    Table schema = flb::loadSchemaFromFile("cli_data/", name);
    auto allData = flb::loadTableData("cli_data/", name, schema);

    bool found = false;
    for (auto& rec : allData) {
      if (rec[0].value == to_string(id)) {
        found = true;
        for (auto& rv : rec) {
          if (rv.column == col) rv.value = newVal;
        }
        db.update(name, id, rec);
        break;
      }
    }

    if (found) {
      flb::saveTableData("cli_data/", name, schema, allData);
      cout << "✅ Actualizado: " << col << "=" << newVal << endl;
    } else {
      cout << "❌ Clave no encontrada para UPDATE\n";
    }
  } else {
    cout << "❌ Error de sintaxis UPDATE\n";
  }
}

.|\n   { /* Ignorar */ }

%%

int main() {
  cout << "📦 CLI Flex para Citas Médicas (Redis-style)\n";
  cout << "Ejemplos:\n";
  cout << "  CREATE users id:INT name:TEXT score:DOUBLE\n";
  cout << "  INSERT users 1 \"Alice\" 99.5\n";
  cout << "  SELECT users 1\n";
  cout << "  UPDATE users 1 name \"Alicia\"\n";
  cout << "  DELETE users 1\n\n";
  yylex();
  return 0;
}

int yywrap() { return 1; }

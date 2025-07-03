%{
#include <iostream>
#include <string>
#include <regex>
#include "../src/storage/database_node.hpp"
DatabaseNode db("cli_data/");
using namespace std;
%}

%%
DELETE[ ]+users[ ]+[0-9]+ {
  int id;
  sscanf(yytext, "DELETE users %d", &id);
  bool ok = db.remove("users", id);
  if (ok)
    cout << "🗑️ Registro con ID " << id << " eliminado.\n";
  else
    cout << "❌ No se encontró el ID " << id << " para eliminar.\n";
}
CREATE[ ]+users    {
  Table t;
  t.addColumn({"id", KindColumn::INT});
  t.addColumn({"name", KindColumn::TEXT});
  db.createTable("users", t, 0);
  cout << "✅ Tabla 'users' creada.\n";
}
INSERT[ ]+users[ ]+[0-9]+[ ]+\"[^\"]+\" {
  std::string input(yytext);
  std::smatch match;
  std::regex pattern("INSERT\\s+users\\s+(\\d+)\\s+\"([^\"]+)\""); // ✅ corregido
  if (std::regex_match(input, match, pattern)) {
    int id = std::stoi(match[1]);
    std::string value = match[2];
    Record r = {{"id", std::to_string(id), "INT"}, {"name", value, "TEXT"}};
    db.insert("users", r);
    cout << "✅ Insertado: ID=" << id << " NAME=" << value << endl;
  } else {
    cout << "❌ Comando INSERT inválido\n";
  }
}
SELECT[ ]+users[ ]+[0-9]+ {
  int id;
  sscanf(yytext, "SELECT users %d", &id);
  auto res = db.search("users", id);
  if (res)
    for (auto &rv : *res)
      cout << rv.column << "=" << rv.value << " ";
  else
    cout << "❌ No encontrado\n";
}
.|\n   { /* Ignorar todo lo demás */ }
%%

int main() {
  cout << "🧪 CLI Flex para Citas Médicas. Escribe comandos como:\n";
  cout << "  CREATE users\n  INSERT users 1 \"Alice\"\n  SELECT users 1\n\n";
  yylex();
  return 0;
}

int yywrap() { return 1; }
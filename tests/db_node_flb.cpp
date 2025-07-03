#include <cassert>
#include <filesystem>
#include <iostream>
#include "../src/util/table.hpp"
#include "../src/storage/database_node.hpp"
#include "../src/storage/database_node_flb.hpp"

int main()
{
  const std::string dir = "engine_test/";
  const std::string table = "users";
  // Parte 2: Simular reinicio - cargar todo desde disco
  {
    DatabaseNode db(dir);

    Table schema = flb::loadSchemaFromFile(dir, table);
    auto data = flb::loadTableData(dir, table, schema);

    // Registrar en memoria
    db.createTable(table, schema, 0); // solo reserva
    for (const auto &record : data)
      db.insert(table, record); // reconstruye árbol

    std::cout << "🔁 Base de datos restaurada desde archivos:\n";

    // Verifica que los datos están en memoria (árbol)
    auto res = db.search(table, 1);
    if (res)
    {
      std::cout << "✅ Registro encontrado (ID 1): ";
      for (const auto &v : *res)
        std::cout << v.column << "=" << v.value << " ";
      std::cout << "\n";
    }
    else
    {
      std::cerr << "❌ Registro con ID 1 no encontrado\n";
    }

    // Otro test: actualiza y verifica persistencia
    db.update(table, 1, {{"id", "1", "INT"}, {"name", "ALICE UPDATEING", "TEXT"}});
    auto updated = db.search(table, 1);
    if (updated)
    {
      std::cout << "✅ Registro actualizado (ID 1): ";
      for (const auto &v : *updated)
        std::cout << v.column << "=" << v.value << " ";
      std::cout << "\n";
    }
  }

  return 0;
}

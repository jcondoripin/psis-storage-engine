#include <cassert>
#include <filesystem>
#include <iostream>
#include "../src/util/table.hpp"
#include "../src/storage/database_node.hpp"
#include "../src/storage/database_node_flb.hpp"

int main()
{
  const std::string dir = "test_restore/";
  const std::string table = "products";

  // Parte 1: Crear y guardar datos en disco
  {
    DatabaseNode db(dir);

    Table schema;
    schema.addColumn({"id", KindColumn::INT});
    schema.addColumn({"name", KindColumn::TEXT});
    schema.addColumn({"price", KindColumn::DOUBLE});

    db.createTable(table, schema, 0);
    Record r1 = {{"id", "1", "INT"}, {"name", "Laptop", "TEXT"}, {"price", "999.99", "DOUBLE"}};
    Record r2 = {{"id", "2", "INT"}, {"name", "Mouse", "TEXT"}, {"price", "25.5", "DOUBLE"}};

    assert(db.insert(table, r1));
    assert(db.insert(table, r2));

    std::cout << "✅ Tabla y datos guardados en disco\n";
  }

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
    db.update(table, 2, {{"id", "2", "INT"}, {"name", "Mouse", "TEXT"}, {"price", "19.99", "DOUBLE"}});
    auto updated = db.search(table, 2);
    if (updated)
    {
      std::cout << "✅ Registro actualizado (ID 2): ";
      for (const auto &v : *updated)
        std::cout << v.column << "=" << v.value << " ";
      std::cout << "\n";
    }
  }

  return 0;
}

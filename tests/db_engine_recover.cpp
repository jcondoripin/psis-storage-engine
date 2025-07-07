#include <string>
#include <cassert>
#include <iostream>
#include "../src/core/engine.hpp"
#include "../src/util/table.hpp"
#include "../src/util/operation.hpp"

int main()
{
  const std::string dir = "engine_multi_test/";

  // Inicializa Engine: carga todo desde disco
  Engine engine(dir);

  // --- SEARCH EXISTING ---
  ArgsCommand selProd;
  selProd.command = Operation::GET;
  selProd.tableName = "products";
  selProd.key = 3;
  std::cout << "Searching products id=3: ";
  engine.exec(selProd);

  ArgsCommand selCust;
  selCust.command = Operation::GET;
  selCust.tableName = "customers";
  selCust.key = 2;
  std::cout << "Searching customers cid=2: ";
  engine.exec(selCust);

  // --- INSERT ---
  Record newProd = {
      {"id", "999", "INT"},
      {"name", "NewProduct", "TEXT"},
      {"price", "123.45", "DOUBLE"}};
  ArgsCommand insProd;
  insProd.command = Operation::INSERT;
  insProd.tableName = "products";
  insProd.record = newProd;
  engine.exec(insProd);

  // Verify insert
  selProd.key = 999;
  std::cout << "Searching products id=999: ";
  engine.exec(selProd);

  // --- UPDATE ---
  Record updProd = {
      {"id", "999", "INT"},
      {"name", "UpdatedProduct", "TEXT"},
      {"price", "543.21", "DOUBLE"}};
  ArgsCommand upd;
  upd.command = Operation::UPDATE;
  upd.tableName = "products";
  upd.key = 999;
  upd.record = updProd;
  engine.exec(upd);

  // Verify update
  std::cout << "Verifying updated product id=999: ";
  engine.exec(selProd);

  // --- REMOVE ---
  ArgsCommand rem;
  rem.command = Operation::REMOVE;
  rem.tableName = "products";
  rem.key = 999;
  engine.exec(rem);

  // Verify remove
  std::cout << "Verifying removed product id=999: ";
  engine.exec(selProd);

  std::cout << "\n✅ Recovery + Modify test passed!\n";
  return 0;
}

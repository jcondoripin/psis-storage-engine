// test_multi_tables.cpp

#include <string>
#include <cassert>
#include <filesystem>
#include <iostream>
#include "../src/core/engine.hpp"
#include "../src/util/table.hpp"
#include "../src/util/operation.hpp"

int main()
{
  const std::string dir = "engine_multi_test/";
  std::filesystem::create_directories(dir);

  Engine engine(dir);

  //
  // Table A: products (id INT, name TEXT, price DOUBLE)
  //
  Table productsSchema;
  productsSchema.addColumn({"id", KindColumn::INT});
  productsSchema.addColumn({"name", KindColumn::TEXT});
  productsSchema.addColumn({"price", KindColumn::DOUBLE});
  productsSchema.setKeyColumn(0);

  ArgsCommand createA;
  createA.command = Operation::CREATE_TABLE;
  createA.tableName = "products";
  createA.schema = productsSchema;
  engine.exec(createA);

  assert(std::filesystem::exists(dir + "products.meta"));
  assert(std::filesystem::exists(dir + "products.tbl"));

  //
  // Table B: customers (cid INT, first TEXT, last TEXT, vip BOOL)
  //
  Table customersSchema;
  customersSchema.addColumn({"cid", KindColumn::INT});
  customersSchema.addColumn({"first", KindColumn::TEXT});
  customersSchema.addColumn({"last", KindColumn::TEXT});
  customersSchema.addColumn({"vip", KindColumn::BOOL});
  customersSchema.setKeyColumn(0);

  ArgsCommand createB;
  createB.command = Operation::CREATE_TABLE;
  createB.tableName = "customers";
  createB.schema = customersSchema;
  engine.exec(createB);

  assert(std::filesystem::exists(dir + "customers.meta"));
  assert(std::filesystem::exists(dir + "customers.tbl"));

  //
  // INSERT some products
  //
  for (int i = 1; i <= 5; ++i)
  {
    Record r = {
        {"id", std::to_string(i), "INT"},
        {"name", "Product" + std::to_string(i), "TEXT"},
        {"price", std::to_string(10.0 * i), "DOUBLE"}};
    ArgsCommand ins;
    ins.command = Operation::INSERT;
    ins.tableName = "products";
    ins.record = r;
    engine.exec(ins);
  }

  //
  // INSERT some customers
  //
  std::vector<std::pair<int, std::string>> names = {
      {1, "Alice"}, {2, "Bob"}, {3, "Carol"}};
  for (auto &p : names)
  {
    Record r = {
        {"cid", std::to_string(p.first), "INT"},
        {"first", p.second, "TEXT"},
        {"last", p.second + "son", "TEXT"},
        {"vip", (p.first % 2 == 0 ? "true" : "false"), "BOOL"}};
    ArgsCommand ins;
    ins.command = Operation::INSERT;
    ins.tableName = "customers";
    ins.record = r;
    engine.exec(ins);
  }

  //
  // SEARCH product id=3
  //
  ArgsCommand selP;
  selP.command = Operation::SEARCH;
  selP.tableName = "products";
  selP.key = 3;
  engine.exec(selP); // should print Product3 and price 30.0

  //
  // SEARCH customer cid=2
  //
  ArgsCommand selC;
  selC.command = Operation::SEARCH;
  selC.tableName = "customers";
  selC.key = 2;
  engine.exec(selC); // should print Bobson and vip=true

  //
  // UPDATE product 4 price to 99.99
  //
  Record updP = {
      {"id", "4", "INT"},
      {"name", "Product4", "TEXT"},
      {"price", "99.99", "DOUBLE"}};
  ArgsCommand updCmd;
  updCmd.command = Operation::UPDATE;
  updCmd.tableName = "products";
  updCmd.key = 4;
  updCmd.record = updP;
  engine.exec(updCmd);

  // verify in-memory
  selP.key = 4;
  engine.exec(selP); // prints price 99.99

  //
  // REMOVE customer cid=1
  //
  ArgsCommand remC;
  remC.command = Operation::REMOVE;
  remC.tableName = "customers";
  remC.key = 1;
  engine.exec(remC);

  // verify removed
  selC.key = 1;
  engine.exec(selC); // should print not found

  std::cout << "✅ Multi-table Engine test passed!" << std::endl;
  return 0;
}

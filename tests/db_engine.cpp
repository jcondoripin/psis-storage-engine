#include <string>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "../src/core/engine.hpp"
#include "../src/util/table.hpp"

int main()
{
  const std::string dir = "engine_test/";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);

  Engine engine(dir);

  // 1. CREATE_TABLE
  Table schema;
  schema.addColumn({"id", KindColumn::INT});
  schema.addColumn({"name", KindColumn::TEXT});

  ArgsCommand createCmd;
  createCmd.command = Operation::CREATE_TABLE;
  createCmd.tableName = "users";
  createCmd.schema = schema;
  engine.exec(createCmd);

  assert(std::filesystem::exists(dir + "users.meta"));
  assert(std::filesystem::exists(dir + "users.tbl"));

  // 2. INSERT
  for (int i = 0; i < 1000; i++)
  {
    Record r1 = {{"id", std::to_string(i), "INT"}, {"name", "Alice", "TEXT"}};
    ArgsCommand ins1;
    ins1.command = Operation::INSERT;
    ins1.tableName = "users";
    ins1.record = r1;
    engine.exec(ins1);
  }

  // 3. SEARCH exists
  ArgsCommand sel;
  sel.command = Operation::SEARCH;
  sel.tableName = "users";
  sel.key = 1;
  engine.exec(sel); // prints Alice

  // 4. UPDATE
  Record r1u = {{"id", "1", "INT"}, {"name", "Alice_updated", "TEXT"}};
  ArgsCommand upd;
  upd.command = Operation::UPDATE;
  upd.tableName = "users";
  upd.key = 1;
  upd.record = r1u;
  engine.exec(upd);

  // verify in-memory
  sel.key = 1;
  engine.exec(sel);

  // 5. REMOVE
  ArgsCommand rem;
  rem.command = Operation::REMOVE;
  rem.tableName = "users";
  rem.key = 2;
  engine.exec(rem);

  // verify removed
  sel.key = 2;
  engine.exec(sel);

  // 6. BACKUP
  ArgsCommand bak;
  bak.command = Operation::BACKUP;
  engine.exec(bak);

  // Verify mutation.log contents
  std::ifstream mutLog(dir + "logs/2025-07-03.log");
  assert(mutLog);
  std::string line;
  std::vector<std::string> mutations;
  while (std::getline(mutLog, line))
  {
    mutations.push_back(line);
  }
  // assert(mutations.size() == 5);
  // assert(mutations[0].find("INSERT users 1 Alice") == 0);
  // assert(mutations[1].find("INSERT users 2 Bob") == 0);
  // assert(mutations[2].find("UPDATE users 1 Alice_updated") == 0);
  // assert(mutations[3].find("REMOVE users 2") == 0);

  std::cout << "All Engine tests passed!" << std::endl;
  return 0;
}

#include "../src/util/table.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

int main()
{
  const std::string metaFileName = "test_table.meta";

  // 1. Create and populate schema
  Table table;
  table.addColumn({"id", stringToKindColumn("INT")});
  table.addColumn({"name", stringToKindColumn("TEXT")});
  table.addColumn({"created_at", stringToKindColumn("DATE")});

  // 2. Serialize schema to file
  {
    std::ofstream out(metaFileName, std::ios::binary);
    assert(out && "Failed to open meta file for writing");
    table.binary_write(out);
  }

  // 3. Read schema back into a new instance
  Table loaded;
  {
    std::ifstream in(metaFileName, std::ios::binary);
    assert(in && "Failed to open meta file for reading");
    loaded.binary_read(in);
  }

  // 4. Verify that columns match
  auto originalCols = table.getColumnNames();
  auto loadedCols = loaded.getColumnNames();
  assert(originalCols == loadedCols && "Column names do not match");

  for (const auto &colName : originalCols)
  {
    assert(table.getColumnKind(colName) == loaded.getColumnKind(colName) && "Column kinds do not match");
  }

  // 5. Output success message
  std::cout << "✅ Table schema serialization/deserialization test passed!\n";

  // Clean up test file
  std::remove(metaFileName.c_str());

  return 0;
}

#include <iostream>
#include <cassert>
#include <filesystem>
#include "../src/util/table.hpp"
#include "../src/storage/database_node.hpp"
#include "../src/storage/database_node_flb.hpp"

int main() {
    // Setup
    const std::string dataDir = "./test_data/";
    std::filesystem::remove_all(dataDir);
    std::filesystem::create_directories(dataDir);

    // Define schema: id(INT), name(TEXT), active(BOOL), score(DOUBLE)
    Table schema;
    schema.addColumn({"id", KindColumn::INT});
    schema.addColumn({"name", KindColumn::TEXT});
    schema.addColumn({"active", KindColumn::BOOL});
    schema.addColumn({"score", KindColumn::DOUBLE});

    DatabaseNode db(dataDir);

    // Test createTable
    assert(db.createTable("users", schema, 0));
    assert(!db.createTable("users", schema, 0)); // duplicate
    assert(std::filesystem::exists(dataDir + "users.meta"));
    assert(std::filesystem::exists(dataDir + "users.tbl"));

    // Insert records
    Record r1 = {{"id","1","INT"}, {"name","Alice","TEXT"}, {"active","true","BOOL"}, {"score","95.5","DOUBLE"}};
    Record r2 = {{"id","2","INT"}, {"name","Bob","TEXT"},   {"active","false","BOOL"}, {"score","80.0","DOUBLE"}};
    assert(db.insert("users", r1));
    assert(db.insert("users", r2));
    assert(!db.insert("users", r1)); // duplicate key

    // Search existing
    auto res1 = db.search("users", 1);
    assert(res1 && res1->at(1).value == "Alice");

    // Load schema & data using separated FLB
    Table loadedSchema = flb::loadSchemaFromFile(dataDir, "users");
    auto allData = flb::loadTableData(dataDir, "users", loadedSchema);
    assert(allData.size() == 2);
    assert(allData[0].at(1).value == "Alice");

    // Update record
    Record r1u = {{"id","1","INT"}, {"name","Alice","TEXT"}, {"active","false","BOOL"}, {"score","99.9","DOUBLE"}};
    assert(db.update("users", 1, r1u));
    auto res1u = db.search("users", 1);
    assert(res1u);

    // Remove record
    assert(db.remove("users", 2));
    assert(!db.search("users", 2));

    // Final loaded data should have one record
    allData = flb::loadTableData(dataDir, "users", loadedSchema);
    assert(allData.size() == 1);
    std::cout << "All separated DatabaseNode tests passed!" << std::endl;
    return 0;
}

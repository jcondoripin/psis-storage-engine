#include <iostream>
#include <cassert>
#include <string>
#include "../src/util/bplustree_exceptions.hpp"
#include "../src/index/bplustree_node/bplustree_leaf_node.hpp"
#include "../src/index/bplus_vector.hpp"

int main()
{
  std::cout << "Testing BPlusTreeLeafNode..." << std::endl;
  BPlusTreeLeafNode<int, std::string> leafNode;

  // Insert elements
  leafNode.insertValue(5, "Five");
  leafNode.insertValue(3, "Three");
  leafNode.insertValue(8, "Eight");
  leafNode.insertValue(1, "One");

  // Check size
  assert(leafNode.keys.size() == 4);
  assert(leafNode.values.size() == 4);

  // split method
  auto [firstNode, secondNode, promotedKey] = leafNode.split();

  assert(firstNode->keys.size() == 2);
  assert(secondNode->keys.size() == 2);
  assert(promotedKey == 5);

  std::cout << "First Node Keys: ";
  
  for (const auto &key : firstNode->keys.getArray())
  {
    std::cout << key << " ";
  }
  std::cout << "\nSecond Node Keys: ";
  for (const auto &key : secondNode->keys.getArray())
  {
    std::cout << key << " ";
  }
  std::cout << "\nPromoted Key: " << promotedKey << std::endl;

  // Search for elements
  assert(leafNode.getValueByKey(3) == "Three");

  // this throws an exception
  try
  {
    leafNode.getValueByKey(10); // Should throw an exception
  }
  catch (const KeyNotFoundException &e)
  {
    std::cout << "Caught expected exception: " << e.what() << std::endl;
  }

  // Get elements by position
  assert(leafNode.keys.getElementByPosition(0) == 1);
  assert(leafNode.keys.getElementByPosition(2) == 5);

  leafNode.removeValueByKey(1);

  assert(leafNode.keys.size() == 3);

  const std::vector<int> &keysArray = leafNode.keys.getArray();
  const std::vector<std::string> &valuesArray = leafNode.values;

  std::cout << "Keys in Leaf Node: ";
  for (const auto &key : keysArray)
  {
    std::cout << key << " ";
  }

  std::cout << "\nValues in Leaf Node: ";
  for (const auto &value : valuesArray)
  {
    std::cout << value << " ";
  }

  std::cout << "\n✅ All tests passed successfully!" << std::endl;

  return 0;
}
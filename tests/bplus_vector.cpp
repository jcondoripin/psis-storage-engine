#include "../src/index/bplus_vector.hpp"
#include <iostream>
#include <cassert>
#include <string>


int main()
{
  BPlusVector<int> bplusVector;

  // Insert elements
  bplusVector.insertElement(5);
  bplusVector.insertElement(3);
  bplusVector.insertElement(8);
  bplusVector.insertElement(1);

  // Check size
  assert(bplusVector.size() == 4);

  // Search for elements
  assert(bplusVector.searchElement(3) == 1);   // Should return index 1
  assert(bplusVector.searchElement(10) == -1); // Should return -1 (not found)

  // Get elements by position
  assert(bplusVector.getElementByPosition(0) == 1);
  assert(bplusVector.getElementByPosition(2) == 5);

  // print array
  const std::vector<int> &array = bplusVector.getArray();

  std::cout << "Elements in BPlusVector: ";
  for (const auto &elem : array)
  {
    std::cout << elem << " ";
  }
  std::cout << std::endl;
  std::cout << "✅ All tests passed successfully!" << std::endl;

  return 0;
}
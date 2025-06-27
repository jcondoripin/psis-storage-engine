#pragma once

#include <vector>
#include <stdexcept>

/**
 * Performs a binary search on a sorted array.
 *
 * @tparam T The type of elements in the array, must support comparison operators.
 * @param array The sorted array to search.
 * @param search The element to search for.
 * @return The index of the element if found, otherwise -1.
 * @throws std::invalid_argument if the array is empty.
 *
 * This function assumes that the array is sorted in ascending order.
 *
 * Example usage:
 * 
 * ```
 * #include <iostream>
 * #include <vector>
 *
 * int main() {
 *   std::vector<int> arr = {1, 2, 3, 4, 5};
 *   int target = 3;
 *   int index = binary_search(arr, target);
 *
 *   if (index != -1) {
 *     std::cout << "Element found at index: " << index << std::endl;
 *   } else {
 *     std::cout << "Element not found." << std::endl;
 *   }
 *
 *   return 0;
 * }
 * ```
 *
 * This function uses a standard binary search algorithm, which has a time complexity of O(log n).
 * Note: The array must be sorted before calling this function. If the array is not sorted, the result is undefined.
 */
template <typename T>
int binary_search(const std::vector<T> &array, const T &search)
{
  if (array.empty())
  {
    throw std::invalid_argument("Array cannot be empty");
  }

  int left = 0;
  int right = static_cast<int>(array.size()) - 1;

  while (left <= right)
  {
    int mid = left + (right - left) / 2;

    if (array[mid] == search)
    {
      return mid;
    }
    else if (array[mid] < search)
    {
      left = mid + 1;
    }
    else
    {
      right = mid - 1;
    }
  }

  return -1;
}
#pragma once

#include <vector>
#include <stdexcept>

/**
 * Finds the position to insert a value in a sorted array using binary search.
 *
 * @tparam T The type of elements in the array, must support comparison operators.
 * @param array The sorted array to search.
 * @param value The value to insert.
 * @return The index where the value should be inserted to maintain sorted order.
 * @throws std::invalid_argument if the array is empty.
 *
 * Example usage:
 *
 * ```
 * #include <iostream>
 * #include <vector>
 *
 * int main() {
 *   std::vector<int> arr = {1, 2, 4, 5};
 *   int value = 3;
 *   int position = binary_position(arr, value);
 *   std::cout << "Insert " << value << " at index: " << position << std::endl;
 *   return 0;
 * }
 * ```
 *
 * This function assumes that the array is sorted in ascending order.
 */
template <typename T>
int lower_bound(const std::vector<T> &array, const T &value)
{
  if (array.empty())
  {
    return 0;
  }

  int left = 0;
  int right = static_cast<int>(array.size()) - 1;

  while (left <= right)
  {
    int mid = left + (right - left) / 2;

    if (array[mid] < value)
    {
      left = mid + 1;
    }
    else
    {
      right = mid - 1;
    }
  }

  return left;
}
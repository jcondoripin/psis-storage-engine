#pragma once

#include <vector>
#include "../util/algorithm/binary_search.hpp"
#include "../util/algorithm/lower_bound.hpp"
#include "../util/exceptions/general_exceptions.hpp"

/**
 * @brief BPlusVector maintains a sorted vector with fast binary search and insertion.
 *
 * This class wraps a standard std::vector<T> but ensures that all elements
 * remain sorted in ascending order. It uses a lower bound to insert new elements
 * at the correct position and binary search to quickly find elements.
 *
 * @tparam T The type of elements stored, must support comparison operators.
 */
template <typename T>
class BPlusVector
{
private:
  std::vector<T> array;

public:
  /**
   * @brief Inserts an element while maintaining ascending order.
   * Uses lower_bound to find the correct position.
   * @param value Element to insert.
   */
  void insertElement(const T &value)
  {
    int position = lower_bound(this->array, value);
    this->array.insert(this->array.begin() + position, value);
  }

  /**
   * @brief Searches for an element using binary search.
   * @param value Element to search for.
   * @return The index if found, or -1 if not found.
   * @throws EmptyArrayException if the array is empty.
   */
  int searchElement(const T &value) const
  {
    return binary_search(this->array, value);
  }

  /**
   * @brief Search position to insert an element.
   * This uses lower_bound to find the position
   * where the element would fit in the sorted order.
   * @param value Element to search for.
   * @return The index where the element should be inserted.
   */
  int searchPosition(const T &value) const
  {
    return lower_bound(this->array, value);
  }

  /**
   * @brief Gets the element at a specific position.
   * Throws std::out_of_range if the index is invalid.
   * @param position Index of the element.
   * @return The element at the given index.
   * @throws OutOfRangeException if the position is out of range.
   */
  T getElementByPosition(int position) const
  {
    if (position < 0 || position >= static_cast<int>(this->array.size()))
    {
      throw OutOfRangeException("Position out of range");
    }
    return this->array[position];
  }

  /**
   * @brief Removes the element at a specific position.
   * Throws std::out_of_range if the index is invalid.
   * @param position Index of the element to remove.
   * @throws OutOfRangeException if the position is out of range.
   */
  void removeElementByPosition(int position)
  {
    if (position < 0 || position >= static_cast<int>(this->array.size()))
    {
      throw OutOfRangeException("Position out of range");
    }
    this->array.erase(this->array.begin() + position);
  }

  /**
   * @brief Removes an element by value.
   * Uses binary search to find the position and removes it if found.
   * @param value Element to remove.
   * @return true if the element was removed, false if not found.
   * @throws EmptyArrayException if the array is empty.
   */
  bool removeElement(const T &value)
  {
    int position = binary_search(this->array, value);
    if (position != -1)
    {
      this->array.erase(this->array.begin() + position);
      return true;
    }
    return false;
  }

  /**
   * @brief Returns the current number of elements.
   * @return Number of stored elements.
   */
  int size() const
  {
    return static_cast<int>(this->array.size());
  }

  /**
   * @brief Returns if the array is empty
   * @return boolean.
   */
  int empty() const
  {
    return this->array.empty();
  }

  /**
   * @brief Returns a const reference to the internal vector.
   * @return Const reference to the sorted vector.
   */
  std::vector<T> &getArray()
  {
    return this->array;
  }
};

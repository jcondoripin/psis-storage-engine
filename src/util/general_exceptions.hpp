#pragma once

#include <stdexcept>
#include <string>
#include <utility>
#include <iostream>

/**
 * @brief Exception thrown when a key already exists in an array.
 */
class EmptyArrayException : public std::runtime_error
{
public:
    explicit EmptyArrayException(const std::string &message)
        : std::runtime_error("Array is empty: " + message) {}
};

/**
 * @brief Exception thrown when a key is not found in an array.
 */
class OutOfRangeException : public std::runtime_error
{
public:
    explicit OutOfRangeException(const std::string &message)
        : std::runtime_error("Out of range: " + message) {}
};

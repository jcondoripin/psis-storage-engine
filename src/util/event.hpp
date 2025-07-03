#pragma once

#include <iostream>
#include <vector>
#include <functional>

template <typename T>
class Event
{
  std::vector<std::function<void(T)>> listeners;

public:
  void subscribe(const std::function<void(T)> &listener)
  {
    listeners.push_back(listener);
  }

  void emit(T value)
  {
    for (auto &listener : listeners)
    {
      listener(value);
    }
  }
};

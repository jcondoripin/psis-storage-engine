#pragma once

#include <string>
#include <vector>

struct RecordValue
{
  std::string column;
  std::string value;
  std::string kind;
};

struct Record
{
  std::vector<RecordValue> values;

  Record() : values({}) {}
  Record(std::vector<RecordValue> _v) : values(_v) {}

  bool operator==(const Record &other) const
  {
    for (const auto &rv : other.values)
    {
      for (int i = 0; i < values.size(); i++)
      {
        if (values[i].column.compare(rv.column) == 0)
        {
          if (values[i].value != other.values[i].value) {
            return false;
          }
        }
      }
    }
    return true;
  }

  Record &operator=(const Record &other)
  {
    for (const auto &rv : other.values)
    {
      bool found = false;
      for (int i = 0; i < values.size(); i++)
      {
        if (values[i].column.compare(rv.column) == 0)
        {
          values[i].value = rv.value;
          values[i].kind = rv.kind;
          found = true;
          break;
        }
      }
      if (!found)
      {
        values.push_back(rv);
      }
    }
    return *this;
  }
};
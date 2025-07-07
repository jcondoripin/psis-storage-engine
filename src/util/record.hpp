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
#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>  // ✅ necesario para std::istringstream

enum class KindColumn
{
  INT,
  DOUBLE,
  LONG,
  TEXT,
  BOOL,
  DATE
};

std::string kindColumnToString(KindColumn kind)
{
  switch (kind)
  {
  case KindColumn::INT:
    return "INT";
  case KindColumn::DOUBLE:
    return "DOUBLE";
  case KindColumn::LONG:
    return "LONG";
  case KindColumn::TEXT:
    return "TEXT";
  case KindColumn::BOOL:
    return "BOOL";
  case KindColumn::DATE:
    return "DATE";
  default:
    return "Unknown";
  }
}

KindColumn stringToKindColumn(const std::string &str)
{
  if (str == "INT")
    return KindColumn::INT;
  if (str == "DOUBLE")
    return KindColumn::DOUBLE;
  if (str == "LONG")
    return KindColumn::LONG;
  if (str == "TEXT")
    return KindColumn::TEXT;
  if (str == "BOOL")
    return KindColumn::BOOL;
  if (str == "DATE")
    return KindColumn::DATE;
  throw std::invalid_argument("Unknown KindColumn string: " + str);
}

struct Column
{
  std::string name;
  KindColumn kind;

  Column() = default;
  Column(std::string name_, KindColumn kind_)
      : name(std::move(name_)), kind(kind_) {}

  std::string kindToString() const
  {
    return kindColumnToString(kind);
  }

  void binary_write(std::ofstream &out) const
  {
    uint32_t nameLen = static_cast<uint32_t>(name.size());
    out.write(reinterpret_cast<const char *>(&nameLen), sizeof(nameLen));
    out.write(name.data(), nameLen);

    uint8_t kindValue = static_cast<uint8_t>(kind);
    out.write(reinterpret_cast<const char *>(&kindValue), sizeof(kindValue));
  }

  void binary_read(std::ifstream &in)
  {
    uint32_t nameLen;
    in.read(reinterpret_cast<char *>(&nameLen), sizeof(nameLen));
    name.resize(nameLen);
    in.read(&name[0], nameLen);

    uint8_t kindValue;
    in.read(reinterpret_cast<char *>(&kindValue), sizeof(kindValue));
    kind = static_cast<KindColumn>(kindValue);
  }
};

inline void writeByKind(std::ofstream &out, const std::string &kind, const std::string &val)
{
  if (kind == "INT")
  {
    int64_t v = std::stoll(val);
    out.write(reinterpret_cast<const char *>(&v), sizeof(v));
  }
  else if (kind == "LONG")
  {
    long v = std::stol(val);
    out.write(reinterpret_cast<const char *>(&v), sizeof(v));
  }
  else if (kind == "DOUBLE")
  {
    double v = std::stod(val);
    out.write(reinterpret_cast<const char *>(&v), sizeof(v));
  }
  else if (kind == "TEXT")
  {
    uint32_t len = static_cast<uint32_t>(val.size());
    out.write(reinterpret_cast<const char *>(&len), sizeof(len));
    out.write(val.data(), len);
  }
  else if (kind == "BOOL")
  {
    uint8_t b = (val == "true");
    out.write(reinterpret_cast<const char *>(&b), sizeof(b));
  }
  else if (kind == "DATE")
  {
    std::istringstream ss(val);
    std::tm tm = {};
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    int64_t secs = std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
    out.write(reinterpret_cast<const char *>(&secs), sizeof(secs));
  }
  else
  {
    throw std::runtime_error("Unsupported kind: " + kind);
  }
}
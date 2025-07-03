#pragma once

#include "string"
#include "chrono"

std::string get_date()
{
  using namespace std::chrono;
  auto now = system_clock::now();
  std::time_t t = system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&t);

  char buf[32];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
  return buf;
}
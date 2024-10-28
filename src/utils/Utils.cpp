#include "Utils.h"

std::string Utils::strTrim(const std::string& str, char c) {
  if (str.empty())
    return std::string();

  size_t start = 0;
  size_t end = str.length();

  while (start < end && str[start] == c)
    ++start;
  while (end > start && str[end - 1] == c)
    --end;
  return str.substr(start, end - start);
}

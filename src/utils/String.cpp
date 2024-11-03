#include "String.h"
#include <sstream>

std::string String::trim(const std::string& str) {
  if (str.empty())
    return std::string();

  size_t start = 0;
  size_t end = str.length();

  while (start < end && std::isspace(str[start]))
    ++start;
  while (end > start && std::isspace(str[end - 1]))
    --end;
  return str.substr(start, end - start);
}

std::vector<std::string> String::split(const std::string& str, char delimiter) {
  size_t start = 0;
  size_t end = str.find(delimiter);
  std::vector<std::string> tokens;

  while (start < str.length() && end != std::string::npos) {
    tokens.push_back(str.substr(start, end - start));
    start = end + 1;
    end = str.find(delimiter, start);
  }

  tokens.push_back(str.substr(start));
  return tokens;
}

std::string String::fromInt(int i) {
  std::ostringstream oss;
  oss << i;
  return oss.str();
}

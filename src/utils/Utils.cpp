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

std::vector<std::string> Utils::strSplit(const std::string& str, char delimiter) {
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

#pragma once

#include <string>
#include <vector>

namespace Utils {
std::string strTrim(const std::string& str, char c);
std::vector<std::string> strSplit(const std::string& str, char delimiter);
}

#pragma once

#include <string>
#include <vector>

namespace String {
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string fromInt(int i);
}

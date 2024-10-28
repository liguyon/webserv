#include "Utils.h"

#include <cassert>

#include <iostream>
#include <string>
#include <vector>

static bool areStrVecDeeplyEqual(const std::vector<std::string>& vec1, const std::vector<std::string>& vec2) {
  if (vec1.size() != vec2.size())
    return false;

  for (size_t i = 0; i < vec1.size(); ++i) {
    if (vec1[i] != vec2[i])
      return false;
  }

  return true;
}

int main() {
  { // TEST: strTrim
    std::cout << "==== TEST: strTrim ====\n";

    assert(Utils::strTrim("       test         ", ' ') == "test");
    assert(Utils::strTrim("       test         ", '-') == "       test         ");
    assert(Utils::strTrim("                ", ' ') == "");
    assert(Utils::strTrim("", ' ') == "");

    std::cout << "all tests passed\n";
  }

  { // TEST: strSplit
    std::cout << "==== TEST: strSplit ====\n";
    std::vector<std::string> expected;
    expected.push_back("a ");
    expected.push_back(" b");
    assert(areStrVecDeeplyEqual(Utils::strSplit("a = b", '='), expected));

    expected.clear();
    expected.push_back("a = b");
    assert(areStrVecDeeplyEqual(Utils::strSplit("a = b", '-'), expected));

    expected.clear();
    expected.push_back("a ");
    expected.push_back(" b ");
    expected.push_back(" c");
    assert(areStrVecDeeplyEqual(Utils::strSplit("a = b = c", '='), expected));

    std::cout << "all tests passed\n";
  }
  return 0;

}

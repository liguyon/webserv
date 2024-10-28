#include "Utils.h"

#include <cassert>

#include <iostream>
#include <string>

int main() {
  { // TEST: strTrim
    std::cout << "==== TEST: strTrim ====\n";

    assert(Utils::strTrim("       test         ", ' ') == "test");
    assert(Utils::strTrim("       test         ", '-') == "       test         ");
    assert(Utils::strTrim("                ", ' ') == "");
    assert(Utils::strTrim("", ' ') == "");

    std::cout << "all tests passed\n";
  }

  return 0;

}

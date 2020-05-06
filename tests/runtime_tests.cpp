// #define NO_STATIC_TESTS
// uncomment above to build a debuggable program with failing constexpr stuff
#include "vector_tests.hpp"
#include "forward_list_tests.hpp"
#include "list_tests.hpp"
#include "set_tests.hpp"
#include "map_tests.hpp"
#include "string_tests.hpp"
#include "algorithm_tests.hpp"

int main(int argc, char *argv[])
{
  vector_tests();
  forward_list_tests();
  list_tests();
  set_tests();
  map_tests();
  string_tests();
  algorithm_tests();
  return 0;
}

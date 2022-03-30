#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/catch.hpp"
#include "lib/mstats-utils.h"

// start test suite
TEST_CASE("`./mstats tests/samples_exe/sample0` (avoid segfaults)", "[suite=start][weight=1]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample0 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used > 10000);
  system("rm mstats_result.txt");
}

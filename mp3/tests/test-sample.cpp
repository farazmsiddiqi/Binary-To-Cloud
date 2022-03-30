#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/catch.hpp"
#include "lib/mstats-utils.h"

// BASIC MEMORY ALLOCATOR - 10 points
TEST_CASE("`./mstats tests/samples_exe/sample1` test (allocation test)", "[weight=2][part=1]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample1 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 1700);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

TEST_CASE("`./mstats tests/samples_exe/sample2` test (allocation test)", "[weight=2][part=1]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample2 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 10000);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

TEST_CASE("`./mstats tests/samples_exe/sample3` test (allocation test)", "[weight=2][part=1]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample3 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 10000);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

TEST_CASE("`./mstats tests/samples_exe/sample7` test (allocation test with calloc)", "[weight=2][part=1]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample7 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

TEST_CASE("`./mstats tests/samples_exe/sample1` test (allocation test with realloc)", "[weight=2][part=1]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample8 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 6000);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

// BLOCK SPLITTING - 15 points
TEST_CASE("sample1, Block Splitting", "[weight=15][part=2]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample1 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 1200);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

// MEMORY COALESCING - 15 points
TEST_CASE("sample2 - Memory Coalescing", "[weight=5][part=3]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample2 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 7500);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

TEST_CASE("sample3 - Memory Coalescing", "[weight=5][part=3]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample3 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 7500);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

TEST_CASE("sample4 - Memory Coalescing", "[weight=5][part=3]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample4 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 10000);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

// FREE LISTS - 10 points
TEST_CASE("sample6 - Free Lists", "[weight=5][part=4]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample6 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 3000000);
  REQUIRE(result->max_heap_used > 0);
  REQUIRE(result->time_taken < 3);
  system("rm mstats_result.txt");
}

TEST_CASE("sample8 - Free Lists (realloc)", "[weight=5][part=4]") {
  system("make -s");
  system("./mstats tests/samples_exe/sample8 evaluate");
  mstats_result * result = read_mstats_result("mstats_result.txt");
  REQUIRE(result->status == 1);
  REQUIRE(result->max_heap_used < 2000);
  REQUIRE(result->max_heap_used > 0);
  system("rm mstats_result.txt");
}

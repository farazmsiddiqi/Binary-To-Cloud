#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/catch.hpp"

TEST_CASE("test_extract_gif - correct full content", "[weight=8][part=3]") {
  system("make -s");

  system("cp tests/files/natalia.png TEST.png");

  system("./png-extractGIF tests/files/natalia.png TEST.gif");   
  REQUIRE(system("diff TEST.gif tests/files/natalia_test.gif") == 0);

  system("rm -f TEST.png TEST.gif");
}

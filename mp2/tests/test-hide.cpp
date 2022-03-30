#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/catch.hpp"

TEST_CASE("test_hide_gif - correct full content", "[weight=8][part=4]") {
  system("make -s");

  system("cp tests/files/test_hide.gif TEST_2.gif");
  system("cp tests/files/240.png TEST_240.png");

  system("./png-hideGIF TEST_240.png TEST_2.gif");
  system("./png-extractGIF TEST_240.png TEST_EXTRACT.gif");

  REQUIRE(system("diff TEST_2.gif TEST_EXTRACT.gif") == 0);

  system("rm -f TEST_EXTRACT.gif TEST_240.png TEST_2.gif");
}

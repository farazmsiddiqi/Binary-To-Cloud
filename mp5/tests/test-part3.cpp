#include "lib/catch.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


void *launch_server(void *vptr_cmd) {
  char *cmd = (char *)vptr_cmd;
  system(cmd);
}

void kill_server(int port) {
  char cmd[100];
  sprintf(cmd, "kill $(lsof -i:%d -t)", port);
  system(cmd);
}

TEST_CASE("server -- loads `/`", "[weight=2][part=3]") {
  time_t t;
  srand((unsigned) time(&t));
  int port = 10000 + (rand() % 40000);
  char cmd[100];
  sprintf(cmd, "./server %d &", port);

  REQUIRE( system("make -s") == 0 );

  pthread_t server;
  pthread_create(&server, NULL, launch_server, (void *)cmd);
  sleep(2);

  char curl[1000];
  sprintf(curl, "curl --max-time 15 -f http://localhost:%d/ >TEST_out.txt", port);

  CAPTURE(curl);
  REQUIRE( system(curl) == 0 );
  REQUIRE( system("diff TEST_out.txt static/index.html") == 0 );

  system("rm TEST_out.txt");
  kill_server(port);

  pthread_join(server,NULL);
}

TEST_CASE("server -- loads `/240.png`", "[weight=3][part=3]") {
  time_t t;
  srand((unsigned) time(&t));
  int port = 10000 + (rand() % 40000);
  char cmd[100];
  sprintf(cmd, "./server %d &", port);

  REQUIRE( system("make -s") == 0 );

  pthread_t server;
  pthread_create(&server, NULL, launch_server, (void *)cmd);
  sleep(2);

  char curl[1000];
  sprintf(curl, "curl --max-time 15 -f http://localhost:%d/240.png >TEST_out.png", port);

  CAPTURE(curl);
  REQUIRE( system(curl) == 0 );
  REQUIRE( system("diff TEST_out.png static/240.png") == 0 );

  system("rm TEST_out.png");
  kill_server(port);

  pthread_join(server,NULL);
}

TEST_CASE("server -- loads `/getaway.html`", "[weight=3][part=3]") {
  time_t t;
  srand((unsigned) time(&t));
  int port = 10000 + (rand() % 40000);
  char cmd[100];
  sprintf(cmd, "./server %d &", port);

  REQUIRE( system("make -s") == 0 );

  pthread_t server;
  pthread_create(&server, NULL, launch_server, (void *)cmd);
  sleep(2);

  char curl[1000];
  sprintf(curl, "curl --max-time 15 -f http://localhost:%d/getaway.html >TEST_out.txt", port);

  CAPTURE(curl);
  REQUIRE( system(curl) == 0 );
  REQUIRE( system("diff TEST_out.txt static/getaway.html") == 0 );

  system("rm TEST_out.txt");
  kill_server(port);

  pthread_join(server,NULL);
}

TEST_CASE("server -- fails `/not-found.html`", "[weight=2][part=3]") {
  time_t t;
  srand((unsigned) time(&t));
  int port = 10000 + (rand() % 40000);
  char cmd[100];
  sprintf(cmd, "./server %d &", port);

  REQUIRE( system("make -s") == 0 );

  pthread_t server;
  pthread_create(&server, NULL, launch_server, (void *)cmd);
  sleep(2);

  char curl[1000];
  sprintf(curl, "curl --max-time 15 -f http://localhost:%d/not-found.html", port);

  CAPTURE(curl);
  REQUIRE( system(curl) != 0 );

  kill_server(port);

  pthread_join(server,NULL);
}


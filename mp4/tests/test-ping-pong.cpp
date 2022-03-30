#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>

#include "../lib/wallet.h"
#include "lib/catch.hpp"

static bool pingpongtimeup = false;

void *ping(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 3000; i++) {
    if(pingpongtimeup) {
      break;
    }
    wallet_change_resource(wallet, "ping", 1);
    wallet_change_resource(wallet, "pong", -1);
    wallet_change_resource(wallet, "thing", 1);
    fprintf(stderr, "↑");
  }
  return NULL;
}

void *pong(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 3000; i++) {
    if(pingpongtimeup) {
      break;
    }
    wallet_change_resource(wallet, "ping", -1);
    wallet_change_resource(wallet, "pong", 1);
    wallet_change_resource(wallet, "thing", 1);
    fprintf(stderr, "↓");
  }
  return NULL;
}

void *timeout(void *wallet) {
  sleep(2);
  pingpongtimeup = true;
  return NULL;
}

TEST_CASE("ping-pong test", "[weight=10][part=1]") {
  // Create and initialize wallets
  wallet_t wallet;
  wallet_init(&wallet);

  pthread_t tids[3];
  pthread_create(&tids[0], NULL, ping,    &wallet);
  pthread_create(&tids[1], NULL, pong,    &wallet);
  pthread_create(&tids[2], NULL, timeout, &wallet);

  for (int i = 0; i < 3; i++) {
    pthread_join(tids[i], NULL);
  }

  CHECK( wallet_get(&wallet, "ping") == 0 );
  CHECK( wallet_get(&wallet, "pong") == 0 );
  CHECK( wallet_get(&wallet, "thing") == 6000 );

  // Destroy the wallets
  wallet_destroy(&wallet);
}

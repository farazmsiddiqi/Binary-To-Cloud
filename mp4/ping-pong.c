#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#include "lib/wallet.h"


void *ping(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 3000; i++) {
    wallet_change_resource(wallet, "ping", 1);
    wallet_change_resource(wallet, "pong", -1);
    fprintf(stderr, "↑");
  }
  return NULL;
}

void *pong(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 3000; i++) {
    wallet_change_resource(wallet, "ping", -1);
    wallet_change_resource(wallet, "pong", 1);
    fprintf(stderr, "↓");
  }
  return NULL;
}

int main() {
  printf("ping-pong\n");
  
  // Create and initialize wallet
  wallet_t wallet;
  wallet_init(&wallet);
  
  pthread_t tids[2];
  pthread_create(&tids[0], NULL, ping, &wallet);
  pthread_create(&tids[1], NULL, pong, &wallet);

  for (int i = 0; i < 2; i++) {
    pthread_join(tids[i], NULL);
  }
  fprintf(stderr, "\n");

  // Destroy the wallet
  wallet_destroy(&wallet);
}

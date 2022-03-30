#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#include "lib/wallet.h"

void *test_add_hedgehog_food(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 500; i++) {
    wallet_change_resource(wallet, "hedgehog-food", 1);
    fprintf(stderr, "🐛");
    usleep(1);
  }
  return NULL;
}

void *test_add_hedgehogs(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 100; i++) {
    wallet_change_resource(wallet, "hedgehog-food", -3);
    wallet_change_resource(wallet, "hedgehogs", 1);
    fprintf(stderr, "🦔");
  }
  return NULL;
}

int main() {
  printf("Running Test: Test hedgehog food and hedgehog\n");
  
  // Create and initialize wallets
  wallet_t wallet;
  wallet_init(&wallet);
  
  pthread_t tids[2];
  pthread_create(&tids[0], NULL, test_add_hedgehogs, &wallet);
  pthread_create(&tids[1], NULL, test_add_hedgehog_food, &wallet);

  for (int i = 0; i < 2; i++) {
    pthread_join(tids[i], NULL);
  }
  fprintf(stderr, "\n");

  printf("Remaining Hedgehog Food 🐛: %d\n", wallet_get(&wallet, "hedgehog-food"));
  printf("Hedgehogs 🦔: %d\n", wallet_get(&wallet, "hedgehogs"));

  // Destroy the wallets
  wallet_destroy(&wallet);
}

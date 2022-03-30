#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>

#include "lib/wallet.h"


void *generate_primogem(void *wallet) {
  wallet = (wallet_t *) wallet;
  int total = 0;
  // Yup, 14.4k primos to get a 5*.
  while (total < 14400) {
    double r = rand() / (float)RAND_MAX;

    int amount;
    if (r < 0.25)      { amount = 10; }
    else if (r < 0.75) { amount = 20; }
    else               { amount = 50; }

    wallet_change_resource(wallet, "primogem", amount);
    total += amount;

    fprintf(stderr, "✨×%d ", amount);
    usleep(1);
  }
  return NULL;
}

void *fate(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 90; i++) {
    wallet_change_resource(wallet, "primogem", -160);
    wallet_change_resource(wallet, "fate", 1);
    fprintf(stderr, "💫 ");
  }
  return NULL;
}

void *wish(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 90; i++) {
    wallet_change_resource(wallet, "fate", -1);
    wallet_change_resource(wallet, "wish", 1);
    fprintf(stderr, "🌠 ");
  }
  return NULL;
}

int pity_4s = 0;
int pity_5s = 0;

void *gacha(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 90; i++) {
    wallet_change_resource(wallet, "wish", -1);

    int result;
    double r = rand() / (float)RAND_MAX;
    if      (r <= 0.006) { result = 5; }
    else if (r <= 0.057) { result = 4; }
    else                 { result = 3; }             

    // pity:
    if (result < 4 && pity_4s >= 9) { result = 4; }
    if (result < 5 && pity_5s >= 89) { result = 5; }

    if (result != 4) { pity_4s++; }
    if (result != 5) { pity_5s++; }

    // result and pity reset:
    if      (result == 5) {
      pity_5s = 0;
      fprintf(stderr, "🌟🌟🌟🌟🌟\n");
      wallet_change_resource(wallet, "5*", 1);
    } else if (result == 4) {
      pity_4s = 0;
      fprintf(stderr, "⭐⭐⭐⭐\n");
      wallet_change_resource(wallet, "4*", 1);
    } else {
      fprintf(stderr, "★★★\n");
      wallet_change_resource(wallet, "3*", 1);
    }
  }
  return NULL;
}


int main() {
  printf("gacha simulation\n");
  srand(time(NULL));
  
  // Create and initialize wallet
  wallet_t wallet;
  wallet_init(&wallet);
  
  pthread_t tids[4];
  pthread_create(&tids[0], NULL, generate_primogem, &wallet);
  pthread_create(&tids[1], NULL, fate, &wallet);
  pthread_create(&tids[2], NULL, wish, &wallet);
  pthread_create(&tids[3], NULL, gacha, &wallet);

  for (int i = 0; i < 4; i++) {
    pthread_join(tids[i], NULL);
  }
  fprintf(stderr, "\n");

  int five = wallet_get(&wallet, "5*");
  printf("🌟🌟🌟🌟🌟 found: %d\n", five);
  printf("⭐⭐⭐⭐ found: %d\n", wallet_get(&wallet, "4*"));
  printf("★★★ found: %d\n", wallet_get(&wallet, "3*"));
  printf("\n");

  if      (five == 0) { printf("!!: Yikes, your wallet must not be correct.\n"); }
  else if (five == 1) { printf("OK, and average luck.\n"); }
  else if (five == 2) { printf("WOW, you had exceptional luck.\n"); }
  else if (five == 3) { printf("AMAZING, the luck to get this is INSANE!\n"); }
  else                { printf("...I think your wallet is broken. :(\n"); }

  // Destroy the wallet
  wallet_destroy(&wallet);
}

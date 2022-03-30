#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>

#include "../lib/wallet.h"
#include "lib/catch.hpp"

void *test_add_hedgehog_food(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 500; i++) {
      wallet_change_resource(wallet, "hedgehog-food", 1);
  }
  return NULL;
}

void *test_add_hedgehogs(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 100; i++) {
      wallet_change_resource(wallet, "hedgehog-food", -3);
      wallet_change_resource(wallet, "hedgehogs", 1);
  }
  return NULL;
}

TEST_CASE("Test HedgeHog Food and HedgeHogs", "[weight=5][part=1]") {
  clock_t start = clock();
  time_t wall_begin, wall_end;
  time(&wall_begin);

  // Create and initialize wallets
  wallet_t wallet;
  wallet_init(&wallet);

  pthread_t tids[10];
  pthread_create(&tids[0], NULL, test_add_hedgehogs, &wallet);
  pthread_create(&tids[1], NULL, test_add_hedgehogs, &wallet);
  pthread_create(&tids[2], NULL, test_add_hedgehog_food, &wallet);
  pthread_create(&tids[3], NULL, test_add_hedgehog_food, &wallet);
  pthread_create(&tids[4], NULL, test_add_hedgehogs, &wallet);
  pthread_create(&tids[5], NULL, test_add_hedgehogs, &wallet);
  pthread_create(&tids[6], NULL, test_add_hedgehog_food, &wallet);
  pthread_create(&tids[7], NULL, test_add_hedgehog_food, &wallet);

  for (int i = 0; i < 8; i++) {
    pthread_join(tids[i], NULL);
  }

  int num_food = wallet_get(&wallet, "hedgehog-food");
  int num_hedgehogs = wallet_get(&wallet, "hedgehog-food");

  // Destroy the wallets
  wallet_destroy(&wallet);

  clock_t end = clock();
  time(&wall_end);

  time_t wall_time = wall_end - wall_begin;
  printf("Wall Time: %lu second(s)\n",wall_time);
  unsigned long cpu_clocks = end - start;
  printf("CPU Clocks: %lu\n",cpu_clocks);

  REQUIRE(num_food == 800);
  REQUIRE(num_hedgehogs == 800);
}

// Test whether proper conditional variables are being used to wait.
void *test_add_hedgehog_food_mutex(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 100; j++) {
      wallet_change_resource(wallet, "hedgehog-food", 1);
    }
    sleep(5);
  }
  return NULL;
}

void *test_add_hedgehogs_mutex(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 100; i++) {
      wallet_change_resource(wallet, "hedgehog-food", -3);
      wallet_change_resource(wallet, "hedgehogs", 1);
  }
  return NULL;
}

TEST_CASE("Test HedgeHog Food and HedgeHogs - Mutex", "[weight=10][part=2][timeout=30]") {
  // Create and initialize wallets
  clock_t start = clock();
  time_t wall_begin, wall_end;
  time(&wall_begin);

  wallet_t wallet;
  wallet_init(&wallet);

  pthread_t tids[10];
  pthread_create(&tids[0], NULL, test_add_hedgehogs_mutex, &wallet);
  pthread_create(&tids[1], NULL, test_add_hedgehog_food_mutex, &wallet);

  for (int i = 0; i < 2; i++) {
    pthread_join(tids[i], NULL);
  }

  int num_food = wallet_get(&wallet, "hedgehog-food");
  int num_hedgehogs = wallet_get(&wallet, "hedgehogs");

  // Destroy the wallets
  wallet_destroy(&wallet);

  clock_t end = clock();
  time(&wall_end);

  // Require correct values of food and hedgehogs.
  REQUIRE(num_food == 200);
  REQUIRE(num_hedgehogs == 100);

  time_t wall_time = wall_end - wall_begin;
  printf("Wall Time: %lu second(s)\n",wall_time);
  REQUIRE(wall_time <= 26);
  unsigned long cpu_clocks = end - start;
  printf("CPU Clocks: %lu\n",cpu_clocks);
  REQUIRE(cpu_clocks < 10000);
}

// Test whether proper conditional variables are being used to wait.
void *test_add_hedgehog_food_mutex_fast(void *wallet) {
  wallet = (wallet_t *) wallet;
  for (int j = 500; j > 0; j--) {
    wallet_change_resource(wallet, "hedgehog-food", 1);
  }
  return NULL;
}

TEST_CASE("Test HedgeHog Food and HedgeHogs - Broadcast", "[weight=10][part=2]") {
  // Create and initialize wallets
  clock_t start = clock();

  wallet_t wallet;
  wallet_init(&wallet);

  pthread_t tids[10];
  pthread_create(&tids[0], NULL, test_add_hedgehogs_mutex, &wallet);
  pthread_create(&tids[1], NULL, test_add_hedgehog_food_mutex_fast, &wallet);

  for (int i = 0; i < 2; i++) {
    pthread_join(tids[i], NULL);
  }

  int num_food = wallet_get(&wallet, "hedgehog-food");
  int num_hedgehogs = wallet_get(&wallet, "hedgehogs");

  // Destroy the wallets
  wallet_destroy(&wallet);

  clock_t end = clock();

  // Require correct values of food and hedgehogs.
  REQUIRE(num_food == 200);
  REQUIRE(num_hedgehogs == 100);

  unsigned long cpu_clocks = end - start;
  printf("CPU Clocks: %lu\n",cpu_clocks);
  REQUIRE(cpu_clocks < 40000);
}

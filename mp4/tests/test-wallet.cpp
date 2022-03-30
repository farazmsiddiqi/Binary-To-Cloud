#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>

#include "../lib/wallet.h"
#include "lib/catch.hpp"

// Test 1
TEST_CASE("Test Initial Get Resource", "[weight=5][part=1]") {
  // Create and initialize the wallet
  wallet_t wallet;
  wallet_init(&wallet);

  // Get the amount of a resource
  int num_resources = wallet_get(&wallet, "resources");
  REQUIRE(num_resources == 0);

  // Destroy the wallet
  wallet_destroy(&wallet);
  return 0;
}


// Test 2
void * test_add_negative_delta(void * wallet) {
  wallet = (wallet_t *) wallet;
  wallet_change_resource(wallet, "cookies", -10);
  return NULL;
}

void * test_add_positive_delta(void * wallet) {
  wallet = (wallet_t *) wallet;
  wallet_change_resource(wallet, "cookies", 100);
  return NULL;
}

TEST_CASE("Test Initial Negative Delta", "[weight=5][part=1]") {
    // Create and initialize the wallet
    printf("%s", "1\n");
    wallet_t wallet;
    printf("%s", "2\n");
    wallet_init(&wallet);
    printf("%s", "3\n");

    // Test what happens if the first thing your wallet sees is a negative delta
    pthread_t tids[2];
    printf("%s", "4\n");
    pthread_create(&tids[0], NULL, test_add_negative_delta, &wallet);
    printf("%s", "5\n");
    sleep(5);
    printf("%s", "6\n");
    pthread_create(&tids[1], NULL, test_add_positive_delta, &wallet);
    printf("%s", "7\n");
    pthread_join(tids[0], NULL);
    printf("%s", "8\n");
    pthread_join(tids[1], NULL);
    printf("%s", "9\n");

    // Get the final number of the resource
    int num_cookies = wallet_get(&wallet, "cookies");
    printf("%s", "10\n");
    REQUIRE(num_cookies == 90);

    // Destroy the wallet
    wallet_destroy(&wallet);
}


// Test 3
TEST_CASE("Test Multiple Wallets","[weight=10][part=1]") {
  wallet_t wallet_1;
  wallet_t wallet_2;

  wallet_init(&wallet_1);
  wallet_init(&wallet_2);

  REQUIRE(wallet_get(&wallet_1, "cookies") == 0);
  pthread_t tids[4];
  pthread_create(&tids[0], NULL, test_add_positive_delta, &wallet_1);
  pthread_create(&tids[1], NULL, test_add_positive_delta, &wallet_2);
  pthread_create(&tids[2], NULL, test_add_positive_delta, &wallet_1);
  pthread_create(&tids[3], NULL, test_add_positive_delta, &wallet_1);

  for(int i=0;i<4;i++)
    pthread_join(tids[i],NULL);

  REQUIRE(wallet_get(&wallet_1, "cookies") == 300);
  REQUIRE(wallet_get(&wallet_2, "cookies") == 100);

  wallet_destroy(&wallet_1);
  wallet_destroy(&wallet_2);
}


// Test 4
void * test_add_large(void * wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 100000; i++) {
    wallet_change_resource(wallet, "potatoes", 2);
  }
}

void * test_subtract_large(void * wallet) {
  wallet = (wallet_t *) wallet;
  for (int i = 0; i < 100000; i++) {
    wallet_change_resource(wallet, "potatoes", -1);
  }
}

TEST_CASE("Test Large Adds and Subtracts", "[weight=5][part=2]") {
  // Create and initialize the wallet
  wallet_t wallet;
  wallet_init(&wallet);

  int num_threads = 100;
  pthread_t tids[num_threads];

  for (int i = 0; i < (num_threads/2); i++) {
    pthread_create(&tids[i], NULL, test_subtract_large, (void *) &wallet);
  }
  for (int i = (num_threads/2); i < num_threads; i++) {
    pthread_create(&tids[i], NULL, test_add_large, (void *) &wallet);
  }
  for (int i = 0; i < num_threads; i++) {
    pthread_join(tids[i], NULL);
  }

  int num_potatoes = wallet_get(&wallet, "potatoes");
  REQUIRE(num_potatoes == (100000 * (num_threads/2)));

  // Destroy the wallet
  wallet_destroy(&wallet);
}

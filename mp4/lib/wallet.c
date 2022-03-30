#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wallet.h"

/**
 * Initializes an empty wallet.
 */
void wallet_init(wallet_t *wallet) {
  // Implement
  wallet->head = NULL;
  // init lock, argv[1] == NULL specifies default behavior: resource is private to thread with lock
  pthread_mutex_init(&wallet->lock, NULL);
}

wallet_resource* get_helper(wallet_t *wallet, const char *resource) {
  wallet_resource* iter = wallet->head;
  while (iter != NULL && strcmp(iter->resource_name, resource) != 0) {
    iter = iter->next;
  }

  return iter;
}

/**
 * Returns the amount of a given `resource` in the given `wallet`.
 */
int wallet_get(wallet_t *wallet, const char *resource) {

  wallet_resource* iter = get_helper(wallet, resource);
  // resource not found
  if (iter == NULL) {
    return 0;
  }

  return iter->amount;
}

void wallet_add(wallet_t *wallet, wallet_resource *res) {
  if (wallet->head == NULL) {
    wallet->head = res;
    res->next = NULL;
    return;
  }

  wallet_resource* iter = wallet->head;
  while (iter->next != NULL) {
    iter = iter->next;
  }
  // at this point, iter->next is NULL
  iter->next = res;
}

/**
 * Modifies the amount of a given `resource` in a given `wallet` by `delta`.
 * - If `delta` is negative, this function MUST NOT RETURN until the resource can be satisfied.
 *   (Ths function MUST WAIT until the wallet has enough resources to satisfy the request;
 *    there are several ways to accomplish this waiting and it does not have to be fancy.)
 */
void wallet_change_resource(wallet_t *wallet, const char *resource, const int delta) {

  pthread_mutex_lock(&wallet->lock);

  wallet_resource* res = get_helper(wallet, resource);
  if (res == NULL) { // resource doesn't exist --> make resource
    res = malloc(sizeof(wallet_resource));
    // malloc for what resource points to as well
    char* tmp = calloc(strlen(resource) + 1, sizeof(char));
    for (size_t i = 0; i < strlen(resource); i++) {
      tmp[i] = resource[i];
    }
    res->resource_name = tmp;
    // init vals
    res->amount = 0;
    res->next = NULL;
    pthread_cond_init(&res->cond, NULL);
    // add new res to wallet MAKE FN
    wallet_add(wallet, res);
  } else { // resource exists --> pull up current resource
    res = wallet->head;
    while (res != NULL && strcmp(res->resource_name, resource) != 0) {
      res = res->next;
    }
  }

  if (delta < 0) { // widthdrawal
    // blocks threads that are asking for more resources than what is currently in wallet
    while(res->amount < -1 * delta) {
      pthread_cond_wait(&res->cond, &wallet->lock);
    }

    // at this point, we are guaranteed to have enough resource for the widthdrawal to succeed
    res->amount += delta; // delta is negative

    // signal waiting room that resource is ready
    pthread_cond_signal(&res->cond);

    // unlock
    pthread_mutex_unlock(&wallet->lock);
    
  } else { // deposit
    res->amount += delta;
    // signal waiting room that resource is ready
    pthread_cond_signal(&res->cond);

    // unlock
    pthread_mutex_unlock(&wallet->lock);
  }

}

/**
 * Destroys a wallet, freeing all associated memory.
 */
void wallet_destroy(wallet_t *wallet) {
  wallet_resource* iter = wallet->head;
  while (iter != NULL) {
    wallet_resource* tmp = iter;
    iter = iter->next;
    free((char*) tmp->resource_name);
    pthread_cond_destroy(&tmp->cond);
    free(tmp);
  }

  pthread_mutex_destroy(&wallet->lock);
}

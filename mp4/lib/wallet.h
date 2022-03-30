#pragma once
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wallet_resource_ {
  const char *resource_name;
  int amount;
  pthread_cond_t cond;
  struct wallet_resource_ *next;
} wallet_resource;

typedef struct wallet_t_ {
  pthread_mutex_t lock;
  wallet_resource *head;
} wallet_t;

void wallet_init(wallet_t *wallet);
void wallet_add(wallet_t *wallet, wallet_resource *res);
wallet_resource* get_helper(wallet_t *wallet, const char *resource);
int wallet_get(wallet_t *wallet, const char *resource);
void wallet_change_resource(wallet_t *wallet, const char *resource, const int delta);
void wallet_destroy(wallet_t *wallet);

#ifdef __cplusplus
}
#endif
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/times.h>

#include "lib/wallet.h"

void *job_clover_patch(void *wallet) {
  int i = 0;
  for (i = 0; i < 100; i++) {
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "four-leaf-clover", 1); fprintf(stderr, "🍀");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    if (rand() % 10 == 0) {
      wallet_change_resource(wallet, "four-leaf-clover", 1); fprintf(stderr, "🍀");
    }
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
    wallet_change_resource(wallet, "clover", 1); fprintf(stderr, "☘️");
  }

  return NULL;
}

void *job_orchard(void *wallet) {
  int i = 0;
  for (i = 0; i < 110; i++) {
    wallet_change_resource(wallet, "green-apple", 1);
    fprintf(stderr, "🍏");
  }

  return NULL;
}

void *job_workshop(void *wallet) {
  int i = 0;
  for (i = 0; i < 520; i++) {
    wallet_change_resource(wallet, "tools", 1);
    fprintf(stderr, "🧰");

    if (i % 5 == 0) {
      wallet_change_resource(wallet, "gem", 1);
      fprintf(stderr, "💎");
    }
  }

  return NULL;
}

void *job_dna(void *wallet) {
  int i = 0;
  for (i = 0; i < 1750; i++) {
    wallet_change_resource(wallet, "dna", 1);
    fprintf(stderr, "🧬");
  }

  return NULL;
}

void *job_research_green(void *wallet) {
  // 📗 requires 1x🍏 1x🍀 10x☘️ 5x🧬
  int i = 0;
  for (i = 0; i < 100; i++) {
    wallet_change_resource(wallet, "green-apple", -1);
    wallet_change_resource(wallet, "four-leaf-clover", -1);
    wallet_change_resource(wallet, "clover", -10);
    wallet_change_resource(wallet, "dna", -5);
    wallet_change_resource(wallet, "green-book", 1);
    fprintf(stderr, "📗");
  }
  return NULL;  
}

void *job_research_blue(void *wallet) {
  // 📘 requires 10x🧬 1x💎
  int i = 0;
  for (i = 0; i < 100; i++) {
    wallet_change_resource(wallet, "dna", -10);
    wallet_change_resource(wallet, "gem", -1);
    wallet_change_resource(wallet, "blue-book", 1);
    fprintf(stderr, "📘");
  }
  return NULL;  
}

void *job_research_orange(void *wallet) {
  // 📙 requires 5x🧰 2x🧬
  int i = 0;
  for (i = 0; i < 100; i++) {
    wallet_change_resource(wallet, "tools", -5);
    wallet_change_resource(wallet, "dna", -2);
    wallet_change_resource(wallet, "orange-book", 1);
    fprintf(stderr, "📙");
  }
  return NULL;  
}

void *job_combine_research(void *wallet) {
  // 📚 requires 1x📗, 1x📘, 1x📙
  int i = 0;
  for (i = 0; i < 100; i++) {
    wallet_change_resource(wallet, "orange-book", -1);
    wallet_change_resource(wallet, "blue-book", -1);
    wallet_change_resource(wallet, "green-book", -1);
    wallet_change_resource(wallet, "books", 1);
    fprintf(stderr, "📚");
  }
  return NULL;    
}

void *job_graduation(void *wallet) {
  // 🎓 requires 100x 📚 
  wallet_change_resource(wallet, "books", -100);
  wallet_change_resource(wallet, "degree!", 1);
  fprintf(stderr, "🎓");
  return NULL;
}

int main() {
  wallet_t wallet;

  wallet_init(&wallet);
  srand (time(NULL));
	
	pthread_t tids[100];
	fprintf(stderr, "Resources Generated: ");
	pthread_create(&tids[0], NULL, job_graduation, &wallet);
	pthread_create(&tids[1], NULL, job_combine_research, &wallet);
	pthread_create(&tids[2], NULL, job_research_blue, &wallet);
	pthread_create(&tids[3], NULL, job_research_orange, &wallet);
  pthread_create(&tids[4], NULL, job_research_green, &wallet);
  pthread_create(&tids[5], NULL, job_dna, &wallet);
  pthread_create(&tids[6], NULL, job_workshop, &wallet);
  pthread_create(&tids[7], NULL, job_clover_patch, &wallet);
  pthread_create(&tids[8], NULL, job_orchard, &wallet);
  const int totalJobs = 9;

  for (int i = 0; i < totalJobs; i++) {
    pthread_join(tids[i], NULL);
  }
  fprintf(stderr, "\n");
   
  if (wallet_get(&wallet, "degree!") == 1) {
    printf("Your wallet contains a degree! 🎉\n");
    printf("- Extra 🧬: %d\n", wallet_get(&wallet, "dna"));
    printf("- Extra 🍀: %d\n", wallet_get(&wallet, "four-leaf-clover"));
  } else {
    printf("Yikes -- your wallet may not have that degree yet... :(\n");
  }

  wallet_destroy(&wallet);

  return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "cond.c"

int flag = 0;
int pnum;  // number updated when producer runs.
int csum;  // sum computed using pnum when consumer runs.

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


int (*pred)(int); // predicate indicating if pnum is to be consumed

int produceT() {
  scanf("%d",&pnum); // read a number from stdin
  return pnum;
}

void *Produce(void *a) {
  int p;

  p=1;
  while (p) {
    printf("@P-READY\n");
      pthread_mutex_lock(&mutex);
      while (flag==1)
      {
          pthread_cond_wait(&cond, &mutex);
      }
    p = produceT();
      flag = 1;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
    printf("@PRODUCED %d\n",p);
  }
  printf("@P-EXIT\n");
  pthread_exit(NULL);
}


int consumeT() {
  if ( pred(pnum) ) { csum += pnum; }
  return pnum;
}

void *Consume(void *a) {
  int p;

  p=1;
  while (p) {
      printf("@C-READY\n");
      pthread_mutex_lock(&mutex);
      while(flag==1)
      {
          pthread_cond_wait(&cond,&mutex);
      }
    p = consumeT();
      flag = 0;
      pthread_cond_signal(&cond);
      pthread_mutex_unlock(&mutex);
    printf("@CONSUMED %d\n",csum);
  }
  printf("@C-EXIT\n");
  pthread_exit(NULL);
}


int main (int argc, const char * argv[]) {
  // the current number predicate
  static pthread_t prod,cons;
    long rc;

  pred = &cond1;
  if (argc>1) {
    if      (!strncmp(argv[1],"2",10)) { pred = &cond2; }
    else if (!strncmp(argv[1],"3",10)) { pred = &cond3; }
  }


  pnum = 999;
  csum=0;
  srand(time(0));

  printf("@P-CREATE\n");
     rc = pthread_create(&prod,NULL,Produce,(void *)0);
    if (rc) {
            printf("@P-ERROR %ld\n",rc);
            exit(-1);
        }
  printf("@C-CREATE\n");
     rc = pthread_create(&cons,NULL,Consume,(void *)0);
    if (rc) {
            printf("@C-ERROR %ld\n",rc);
            exit(-1);
        }

  printf("@P-JOIN\n");
  pthread_join( prod, NULL);
  printf("@C-JOIN\n");
  pthread_join( cons, NULL);


  printf("@CSUM=%d.\n",csum);

  return 0;
}

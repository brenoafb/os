#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8
#define K 1000
#define M 1000
#define N 1000
#define P 10000

int mat1[K][M];
int mat2[M][N];
int mat3[K][N];
int idx = 0;
int init = 0;

pthread_mutex_t idx_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t init_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void populate_matrix(int m, int n, int mat[m][n]) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      mat[i][j] = (rand() % P) - (P/2);
    }
  }
}

void print_matrix(int m, int n, int mat[m][n]) {
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      printf("%d ", mat[i][j]);
    }
    printf("\n");
  }
}

void *work(void *args) {
  int index = *((int *) args);
  printf("Thread %d: started\n", index);

  while(!init) {}
  // pthread_cond_wait(&cond, &init_mutex);

  while (idx < K*N) {

    pthread_mutex_lock(&idx_mutex);
    int curr_idx = idx;
    idx++;
    pthread_mutex_unlock(&idx_mutex);

    int i = curr_idx % K;
    int j = curr_idx / K;
    // printf("Thread %d: computing entry i=%d, j=%d\n (idx=%d)\n",
    //     index, i, j, curr_idx);
    mat3[i][j] = 0;
    for (int k = 0; k < M; k++) {
      mat3[i][j] += mat1[i][k] + mat2[k][j];
    }
  }
  printf("Thread %d: finished\n", index);
  return NULL;
}

int main(void) {
  pthread_t threads[NUM_THREADS];
  int thread_args[NUM_THREADS];


  srand(time(NULL));

  populate_matrix(K, M, mat1);
  populate_matrix(M, N, mat2);

  for (int i = 0; i < NUM_THREADS; i++) {
    printf("main: creating thread %d\n", i);
    thread_args[i] = i;
    assert(!pthread_create(&threads[i], NULL, work, &thread_args[i]));
  }


  printf("main: threads created\n");
  init = 1;
  // pthread_cond_signal(&cond);

  for (int i = 0; i < NUM_THREADS; i++) {
    assert(!pthread_join(threads[i], NULL));
    printf("main: thread %d joined\n", i);
  }

  pthread_mutex_destroy(&idx_mutex);
  // pthread_mutex_destroy(&init_mutex);

  // print_matrix(K, M, mat1);
  // printf("\n");
  // print_matrix(M, N, mat2);
  // printf("\n");
  // print_matrix(K, N, mat3);
  // printf("\n");

  printf("main: finalizing\n");
  return 0;
}

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

const int multiplier = 1;
int main() { 
  //int false_shared[32];
  int *false_shared =  (int*) malloc(sizeof(int) * 1024 * 1024);
  
  unsigned int N = 1 << 30;
  int* num_to_add = (int*) malloc(sizeof(int) * N); 
  for(int i = 0; i < N; i++) num_to_add[i] = 1;

  printf("starting experiments\n");
  for(int t = 1; t <= 32; t <<= 1) {
    memset(false_shared, 0, sizeof(int) * 1024 * 1024);      
    double st = omp_get_wtime();
#pragma omp parallel num_threads(t) 
    {
      int tid = omp_get_thread_num();
#pragma omp for 
      for(int i = 0; i < N; i++) {
	false_shared[tid * multiplier] += num_to_add[i];
	//false_shared[tid * multiplier + t] += num_to_add[i];
      }
    }
    double tm =  omp_get_wtime() - st;
    int sum = 0;
    for(int i = 0; i < t; i++) {
      sum += false_shared[i * multiplier];
    }
    printf("%d threads - %d sum - in %f seconds\n", t, sum, tm);
  }
}

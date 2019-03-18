#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

#define NBUCKETS 100
#define NVALS 10000000

int main() {  
  int i, ival;
  omp_lock_t hist_locks[NBUCKETS];
  int hist[NBUCKETS];
  int arr[NVALS];

  for(i = 0; i < NVALS; i++) {
    arr[i] = rand() % NBUCKETS;
  }

#pragma omp parallel for 
  for(i=0;i<NBUCKETS; i++){
    omp_init_lock(&hist_locks[i]);
    hist[i] = 0;
  }
  
  double t1 = omp_get_wtime();
#pragma omp parallel for private(ival)
  for(i=0;i<NVALS;i++){
    ival = arr[i];
#ifdef LOCK
    omp_set_lock(&hist_locks[ival]);
    hist[ival]++;
    omp_unset_lock(&hist_locks[ival]);
#else
   #pragma omp atomic
    hist[ival]++;
#endif
  }
  double t2 = omp_get_wtime();

  for(i=0;i<NBUCKETS; i++) {
   omp_destroy_lock(&hist_locks[i]);
  }

  int sum = 0;
  for(int i = 0; i < NBUCKETS; i++) { sum += hist[i];}
  cout << sum << " in " << t2 - t1 << " seconds" << endl;
}

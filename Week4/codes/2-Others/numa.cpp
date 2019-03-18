#include <iostream>
#include <vector>
#include <algorithm>  
#include "omp.h"

using namespace std;

#define CHUNK (1 << 25)
#define N (32 * CHUNK)

int main() {
  int* dummy = new int[N];
  //also try with the loop below
  /*for(int i = 0; i < N; i++) {
    dummy[i] = 1;
    }*/

  cout << "dummy generated " << endl;
#pragma omp parallel num_threads(32)
  {
    int no_threads = omp_get_num_threads();

    if(no_threads == 32) {
      int start = (no_threads - omp_get_thread_num() - 1) * CHUNK; //try with this
      //int start = omp_get_thread_num() * CHUNK; // and this
      for(int i = 0; i < CHUNK; i++) {
	dummy[start + i] = 1;
      }
    } else {
#pragma omp master
      cout << "thread count is not 32" << endl;
    }
  }
  
  cout << "process started " << endl;
  double sum = 0;
  double t1 = omp_get_wtime();
#pragma omp parallel num_threads(32)
  {
   int no_threads = omp_get_num_threads();
   if(no_threads == 32) {
#pragma omp for schedule(static, CHUNK) reduction(+:sum)
     for(int i = 0; i < N; i++) {
       sum += dummy[i];
     }
   } else {
#pragma omp master
     cout << "thread count is not 32" << endl;
   }
  }
  double t2 = omp_get_wtime();
  cout << sum << " in " << t2 - t1 << " seconds " << endl;
    
  return 0;
}

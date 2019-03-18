#include <iostream>
#include "omp.h"

using namespace std;

int main() {
  cout << "In sequential:" << endl;;
  cout << "\tthread_num: " << omp_get_thread_num() << endl;
  cout << "\tnum_threads: " << omp_get_num_threads() << endl;
  cout << "\tmax_threads: " << omp_get_max_threads() << endl;
  cout << "\tin_parallel: " << omp_in_parallel() << endl;  

  omp_set_num_threads(8);
  
#pragma omp parallel num_threads(4)
  {
    #pragma omp single
    {
      cout << "In sequential:" << endl;;
      cout << "\tthread_num: " << omp_get_thread_num() << endl;
      cout << "\tnum_threads: " << omp_get_num_threads() << endl;
      cout << "\tmax_threads: " << omp_get_max_threads() << endl;
      cout << "\tin_parallel: " << omp_in_parallel() << endl;  
    }

    #pragma omp critical
    {
      cout << "\t\tHello from thread " << omp_get_thread_num() << endl;
    }
  }
}

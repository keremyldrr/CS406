#include <iostream>
#include "omp.h"
using namespace std;

int fibo(int n) {
  if(n == 1 || n == 0) return 1;
  else return fibo(n-1) + fibo(n-2);
}

int main() {
#pragma omp parallel for num_threads(8) //collapse(1)
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 2; k++) {
	int result = fibo(40);
	int id = omp_get_thread_num();
	#pragma omp critical
	cout << omp_get_thread_num() << " out of " << omp_get_num_threads() 
	     << " - result: " << result << " " << i << " " << j << " " << k << endl;
      }
    }
  }
}

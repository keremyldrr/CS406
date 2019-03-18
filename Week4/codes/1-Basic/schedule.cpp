#include <iostream>
#include <cmath>
#include <omp.h>

using namespace std;

int N = 1024 * 1024 * 16;
double task(int t, int option) {
  int loop_size = 0;
  if(option == 0) {
    loop_size = N;
  } else if(option == 1) {
    loop_size = rand() % N;
  } else if(option == 2) {
    loop_size = t/8 * N;
  }
  double val = 1;
  for(int i = 0; i < loop_size; i++) {
    val *= i * i - i + (i+5) * (i-2);
  }
  return val;
}

void loop(int sch, int option) {
  int L = 256;
  double sum;
  double st = omp_get_wtime();
  if(sch == 0) {
#pragma omp parallel for schedule(static) reduction(+:sum) 
    for(int i = 0; i < L; i++) {
      sum += task(i, option);
    }
  } else if(sch == 1) {
#pragma omp parallel for schedule(dynamic) reduction(+:sum)
    for(int i = 0; i < L; i++) {
      sum += task(i, option);
    }
  } else if(sch == 2) {
#pragma omp parallel for schedule(guided) reduction(+:sum)
    for(int i = 0; i < L; i++) {
      sum += task(i, option);
    }
  } 
  cout << "time is " << omp_get_wtime() - st << " -- " << sum << endl;
}

int main() {
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      cout << i << " " << j << " - ";
      loop(j,i);
    }
    cout << endl;
  }
  return 1;
}

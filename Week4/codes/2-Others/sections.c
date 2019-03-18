#include <stdio.h>
#include <omp.h>

int main(int argc, char* argv[]) {
#pragma omp parallel sections
  {
#pragma omp section
    { 
      printf ("id = %d, \n", omp_get_thread_num());
    }
    
#pragma omp section
    { 
      printf ("id = %d, \n", omp_get_thread_num());
    }
  }
  return 1;
}

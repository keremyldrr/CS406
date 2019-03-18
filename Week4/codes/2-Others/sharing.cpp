#include <iostream>
#include "omp.h"
using namespace std;

int NT = 32;
int main ()
{
  int shared = -1;
  int priv = -1;
#pragma omp parallel num_threads(NT) private(priv)
  {
    priv = omp_get_thread_num();
    shared = omp_get_thread_num();
  }
  cout << priv << " " << shared << endl;
}

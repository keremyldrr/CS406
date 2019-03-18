#include <iostream>
#include "omp.h"

using namespace std;

class A {
private:
  int size;
  int* mem;
public:
  A(int size = 5) : size(size), mem(new int[size]) {}
  void set(const int& val) {
    for(int i = 0; i < size; i++) {
      mem[i] = val;
    }
  }

  A(const A& cp) {
    this->size = cp.size;
    mem = new int[size];
  }
  
  void print() const {
    cout << size << ": ";
    for(int i = 0; i < size; i++) {
      cout << mem[i] << " ";
    }
    cout << endl;
  }
};

int main() {
  A a(10);
#pragma omp parallel num_threads(32) private(a)
  {
#pragma omp single 
    {
      cout << "Hello from the parallel region " << endl;
    }
    a.set(omp_get_thread_num());		
  }
  a.print();
}

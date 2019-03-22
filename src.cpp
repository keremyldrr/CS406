#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <stdlib.h>
#include <cmath>
#include<cstring>
using namespace std;


void usage()
{
  cout << "USAGE: ./exec <filename>" << endl;
  exit(0);
}


int* get_last_col(int **M,int N){
  int *col = new int[N];
  for(int i=0;i<N;i++){
    col[i] = M[i][N-1];
  }
  return col ;
}

double *sum_column_getx(int **M,int N){
  double *column_sum = new double[N];
  for(int i=0;i<N;i++){
    for(int j=0; j<N;j++)
      column_sum[i]+=(double)M[i][j]/2;
  }
 
  int *last_col = get_last_col(M,N);
  for(int i=0;i<N;i++){
    column_sum[i] = (last_col[i] - column_sum[i]);
  }
 
  return column_sum;
}

int main(int argc, const char** argv)
{

  if(argc != 2)
    usage();

  string line;

  const char* filename = argv[1];
  ifstream input (filename);
  if(input.fail())
    return 0;


  int N;
  int **M;
  getline(input,line);
  N = atoi(line.c_str());

  M = new int*[N];
  for(int i = 0; i < N; i ++)
    M[i] = new int[N];


  int linectr = 0;
  while(getline(input,line)){
    stringstream ss(line);
    int temp;
    int ctr = 0;
    while(ss >> temp)
      M[linectr][ctr++] = temp;

    linectr++;
  }

  double start, end;
  for(int t = 1; t <=16; t*=2) { //t is the number of threads
    start = omp_get_wtime();
    
    ////YOUR CODE GOES HERE
    double *x = sum_column_getx(M,N);
    long long lim = (1 << N - 1) +1;
    int chunkSize = lim/t;
    double **lookup = new double *[t];
    for(int i = 0;i<t;i++)
      {
	lookup[i] = new int[N];
	memcpy(lookup[i], x, sizeof(double) * N);
	int ind = i*chunkSize;
	int y  = ind ^ (ind >> 1);
	int cnt = 0;
	int  mask = 1;
	while (y > 0) {

	  
	  int bit = mask & y;
	  
	  if (bit != 0) {
	    
	    for (int q = 0; q < N; q++) {
	      
	      lookup[i][q] = M[q][cnt];
	    }
	  }
	  cnt++;
	  y = y >> 1;
	  

	}
      
	cout << "t is  = "<< t << "and index is " << ind << endl;
	for(int z=0;z<N;z++){
	  cout << lookup[i][z] << endl;;
	  
	}
	cout << "-------------------------------------"  << endl;
      }
  }
    

    
    
    // best of luck :)
  
    //    double *x_0 = new double[N];




    
  // omp_set_num_threads(t);
    //parallel region start here

    /*
      int tid = omp_get_thread_num();
    
    int s = tid *chunkSize;
    int e = start + chunkSize-1;
    
    double result = 0;
    long double p = 1;
    for (int q = 0; q < N; q++) {
      p *= x[q];
    };
    for (int i = 1; i < lim; i++) {
      //memcpy(x, x_0, sizeof(double) * N);
      // copy(x,(x+N),x_0);
      unsigned int y = i ^(i >> 1);
      int j = i + 1;
      int z = log2(y ^ (j ^ (j >> 1)));
      int mask = 1 << z;
      int s = (y & mask) ? 1 : -1;
      int prodSign;
      if (1 & i)
	prodSign = -1;
      else
	prodSign = 1;

      double prodX = 1;


      
     
      for (int q = 0; q < N; q++) {
	prodX *= x[q];
      }

      p += prodX * prodSign;

      }

    result = (4 * (N % 2) - 2) * p;
    //// YOUR CODE ENDS HERE

    
    end = omp_get_wtime();

    cout << "Threads: " << t << "\tResult:" << result << "\tTime:" << end - start << " s" << endl;
    }*/

  for( int i=0;i<N;i++)
    delete [] M[i];
  delete M;
 
  return 0;
  
}

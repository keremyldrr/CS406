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

  for(int t =1; t <=16; t*=2) { //t is the number of threads
    start = omp_get_wtime();
    
    ////YOUR CODE GOES HERE
    long long lim = (1 << (N - 1)) ;
     omp_set_num_threads(t);
     //parallel region start here
     double *column_sum = new double[N];
     // double *x =new double[N];    
     double p = 1;
     for(int i=0;i<N;i++)
       {
	 column_sum[i] = 0;
	 //	 x[i] = 0;
	 for(int j=0;j<N;j++)
	   {
	     column_sum[i]+=M[i][j];
	   }

	 p *= M[i][N-1] - column_sum[i]/2;
	 
       }
     int chunkSize = (lim/(t));
     double result = 0;
#pragma omp parallel 
     { 
	int tid = omp_get_thread_num();
	double *x_new = new double[N];
	for(int i=0;i<N;i++)
	  x_new[i] = (M[i][N-1] - double(column_sum[i]/2));
	int r = (tid)*chunkSize;
	int y = r ^ (r>>1);
	int numSetBits= __builtin_popcount(y);
	for(int f=0;f<numSetBits;f++)//pre calculation of initial x values for threads
	  {
	    int bit = __builtin_ffs(y);
	    if(bit !=0);
	    {
	      for (int q = 0; q < N; q++)
		{
		  x_new[q] += M[q][bit-1];
		  
		}
	      y = y^(1 << (bit-1));
	    }
	  }
#pragma omp for  schedule(static) reduction(+:p)
	for (int i = 1; i < lim; i++)
	  {
	   int y = i ^ (i >> 1);
	   int j = i - 1;
	   int z = log2(y ^ (j ^ (j >> 1))) +1;
	   int mask = 1 << z-1;
	   int getBit= (y & mask) ;
	   int s;
	   if(getBit)
	     s=1;
	   else
	     s=-1;
	   int prodSign;
	   if(i & 1)
	     prodSign = -1;
	   else
	     prodSign = 1;
	   double prodX = 1;
	   for(int q=0;q<N;q++)
	     {
	       x_new[q]+=s*M[q][z-1];
	       prodX *= x_new[q];
	     }
	   p+= prodX * prodSign;

	 }
	delete [] x_new;
     }
      result = (4 * (N % 2) - 2) * p;
      delete [] column_sum;
      //// YOUR CODE ENDS HERE
          
    end = omp_get_wtime();

    cout << "Threads: " << t << "\tResult:" << result << "\tTime:" << end - start << " s" << endl;
    


  }
  
  for( int i=0;i<N;i++)
    delete [] M[i];
  delete M;

  return 0;
    
}

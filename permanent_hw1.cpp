#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <stdlib.h>
#include <cmath>
#include<cstring>
#include <immintrin.h>

using namespace std;


void usage()
{
  cout << "USAGE: ./exec <filename>" << endl;
  exit(0);
}


int myLog2(int v)
{
  int r = 0;
  if(v &  0xFFFF0000)
    {
      v >>=16;
      r = 16;
    }
  if(v &  0xFF00)
    {
      v >>=8;
      r |= 8;
    }
  if(v &  0xF0)
    {
      v >>=4;
      r |= 4;
    }
  if(v &  0xC)
    {
      v >>=2;
      r |= 2;
    }
  if(v &  0x2)
    {
      v >>=1;
      r |= 1;
    }
  return r;
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
      M[ctr++][linectr] = temp;

    linectr++;
  }

  double start, end;

  for(int t =1; t <=16; t*=2) { //t is the number of threads
    start = omp_get_wtime();
    ////YOUR CODE GOES HERE

    long int  lim = (1 << (N - 1)) ;
     omp_set_num_threads(t);
     //parallel region start here
     double *column_sum = new double[N];
     double p = 1;
     
#pragma omp simd reduction(*:p)
     for(int i=0;i<N;i++)
       {
	 column_sum[i] = 0;

	 for(int j=0;j<N;j++)
	   {
	     column_sum[i]+=M[j][i];
	   }

	 p *= M[N-1][i] - column_sum[i]/2;
	 
       }
     int chunkSize = (lim/(t));
     double result = 0;
     
#pragma omp parallel proc_bind(spread)
     {
       int **M_2  = new int*[N];
       for(int i=0;i<N;i++)
	 {
	   M_2[i] = new int[N];
	   for(int j=0;j<N;j++)
	     M_2[i][j] = M[i][j];
	 }
       int tid = omp_get_thread_num();
	double *x_new = new double[N];
	for(int i=0;i<N;i++)
	  x_new[i] = (M_2[N-1][i] - double(column_sum[i]/2));
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
		  x_new[q] += M_2[bit-1][q];
		}
	      y = y^(1 << (bit-1));
	    }
	  }
#pragma omp for  schedule(static) reduction(+:p)
	for (int i = 1; i < lim; i++)
	  {
	   int y = i ^ (i >> 1);
	   int j = i - 1;
	   int z = myLog2(y ^ (j ^ (j >> 1))) +1;
	   
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
	   #pragma omp simd reduction(*:prodX)
	   for(int q=0;q<N;q++)//vector add
	     {
	       x_new[q]+=s*M_2[z-1][q];
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

    //cout << "Threads: " << t << "\tResult:" << result << "\tTime:" << end - start << " s" << endl;
     cout << t << "," << result << "," << end - start <<",\n";


  }
  return 0;
    
}

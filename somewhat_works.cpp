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

  for(int t =1; t <=16; t*=2) { //t is the number of threads
    start = omp_get_wtime();
    
    ////YOUR CODE GOES HERE
   
    
    long long lim = (1 << N - 1)  - 1 ;
     omp_set_num_threads(t);
    //parallel region start here
    
     double chunkSize = ceil(lim/t);
     
     //double *x_new = sum_column_getx(M,N);
     long double p = 1;
     double result = 0;
      double *x =sum_column_getx(M,N) ;
      for (int q = 0; q < N; q++) {
	p *= x[q];
      }
      bool init =false;
#pragma omp parallel  
      {
	int tid = omp_get_thread_num();
	double *x_new =sum_column_getx(M,N) ;
	int r = (tid)*chunkSize + 1;
	int y = r ^ (r>>1);
	//printf("Thread %d  iteration %d ,\n ",tid,r);
	int numSetBits= __builtin_popcount(y);
	for(int f=0;f<numSetBits && tid!=0;f++)
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


#pragma omp for firstprivate(init)  schedule(static) reduction(+:p)
       for (int i = 1; i <= lim; i++)
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
	    if(!init)
	      {
	      init =true;

	      //	      printf( "OLUM %d %d \n",tid,i);

	       }
	   //deneme

	   for(int q=0;q<N;q++)
	     {
	       x_new[q]+=s*M[q][z-1];
	       prodX *= x_new[q];
	     }
	   
	   p += prodX * prodSign;
	   
		

	 }
      }
      result = (4 * (N % 2) - 2) * p;
      //// YOUR CODE ENDS HERE
      
    
    end = omp_get_wtime();

    cout << "Threads: " << t << "\tResult:" << result << "\tTime:" << end - start << " s" << endl;

      
  }
  for( int i=0;i<N;i++)
    delete [] M[i];
  delete M;
 
  return 0;
    
}

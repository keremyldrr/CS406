#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include "cuda_error_handle.h"
using namespace std;
typedef long long int ll;
__global__ void preprocess(ll *M,double *allLocalX,int N,int chunkSize)
{

  ll  tid = threadIdx.x + blockIdx.x * blockDim.x;
  
  ll  r = (tid)*chunkSize; // each chunk in X array is of size N;
  
  ll  y = r ^ (r>>1);
  ll  numSetBits= __popcll(y);
  for( int f=0;f<numSetBits;f++)
    {
      ll bit = __ffsll(y);
      if(bit !=0);
      {

	for (  int  q = 0; q < N; q++)
	  {
	    allLocalX[tid*N + q] += M[q*N + bit-1];

	  }
	y = y^(1LL << (bit-1));
      }
    }    
      
}
__global__ void permanent(ll *M,double *X,ll N,double *d_p,ll  chunkSize,ll limit)
{
  
  ll tid = threadIdx.x + blockIdx.x * blockDim.x;
  //  chunkSize+=1;  
  ll start = tid*chunkSize+1;
  ll end = start+chunkSize;
  ll prodSign = start & 1LL;

  if(prodSign)
    prodSign=-1;
   else
     prodSign = 1;
   for( ll ind=start;ind<end && ind < limit;ind++)
	{
	  
	  ll y = (ind ^ (ind >> 1LL));
	  ll j = ind-1;
	  ll z = __ffsll(y ^ (j ^ (j >> 1LL)));
	  ll mask = (1LL << (z-1));
	  ll s = y & mask;
	  
	  if(!s)
	    s=-1;
	  else
	    s=1;
	  

	  double prodX = 1;
	  for( ll i=0;i<N;i++)
	    {
	      X[tid*N + i] += s*M[i*N + z-1];
	      prodX *= X[tid*N + i];
	    }
	  
	  double res =  prodSign * prodX;
	  //errortesting[ind] = prodX*prodSign;//M[N+z-1];
	  atomicAdd(d_p,res);
	  prodSign*=-1;
	}

  
  
}                  

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
  ll **M;
  getline(input,line);
  N = atoi(line.c_str());

  M = new ll*[N];
  for(int i = 0; i < N; i ++)
    M[i] = new ll[N];


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
  cudaSetDevice(0);  
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);
  ll threads = prop.maxThreadsPerBlock;



  int numBlocks = 2048;
  ll totalNumThreads = threads * numBlocks;
  
  ll shift =1; 
  ll lim = (1LL<< (N-1));
  ll chunkSize = lim/(totalNumThreads);
  
  if(chunkSize == 0)
    chunkSize = 1;
  ll noChunks = lim/chunkSize ;


  double *local_x_values = new double [noChunks * N];
  ll *new_m = new ll[N * N];
  double *column_sum = new double[N];



  double *d_local;
  ll *d_m;
  double *d_p;
  //  double *errortest = new double [lim];
  //ll errorSize = sizeof(double )*(1 + lim);
  //double *d_errortest;
  cout << "Chunksize is : " << chunkSize <<  " lim is " <<lim << " and there are "<<noChunks << " chunks"<<  endl;  
  CudaSafeCall(cudaMalloc((void **)&d_local,sizeof(double)*(noChunks * N)));
  CudaSafeCall(cudaMalloc((void **)&d_m,sizeof(ll)*(N * N)));

  CudaSafeCall(cudaMalloc((void **)&d_p,sizeof(double)));
  //CudaSafeCall(cudaMalloc((void **)&d_errortest,errorSize));
  

  for(int i = 0;i<N;i++)
    {
      for(int j = 0;j<N;j++)
	{
	  new_m[i*N + j] = M[i][j];
	  //cout << new_m[i*N + j] << endl;
	}
    }

  
  ////YOUR CODE GOES HERE

  double p = 1;
  
  for(int i=0;i<N;i++)
    {
      column_sum[i] = 0;

      for(int j=0;j<N;j++)
	{
	  column_sum[i]+=M[i][j];
	}

      p *= (M[i][N-1] - column_sum[i]/2);

    }
  cout << "INITIAL P " << p << endl;
  for(int i=0;i<noChunks;i++)
    {
      for(  int j=0;j<N;j++)
	{
	  local_x_values[i*N + j] = (M[j][N-1] - double(column_sum[j]/2));
	  
	}
      
    }

  
  CudaSafeCall(cudaMemcpy(d_local,local_x_values,sizeof(double)*(noChunks * N),cudaMemcpyHostToDevice) );
  CudaSafeCall(cudaMemcpy(d_m,new_m,sizeof(ll)*N*N,cudaMemcpyHostToDevice));
  CudaSafeCall(cudaMemcpy(d_p,&p,sizeof(double),cudaMemcpyHostToDevice));
  // CudaSafeCall(cudaMemcpy(d_errortest,errortest,errorSize,cudaMemcpyHostToDevice));
  

  start = omp_get_wtime();      
  preprocess<<<numBlocks,threads >>>(d_m,d_local,N,chunkSize);
  CudaCheckError();
  cudaDeviceSynchronize();
 
  permanent<<<numBlocks,threads >>>(d_m,d_local,N,d_p,chunkSize,lim);
  CudaCheckError();
  end = omp_get_wtime();

  CudaSafeCall(cudaMemcpy(&p,d_p,sizeof(double),cudaMemcpyDeviceToHost));
  //CudaSafeCall(cudaMemcpy(errortest,d_errortest,errorSize,cudaMemcpyDeviceToHost));
  
  
  cout << "FINAL P "<<p << endl;
  double result = (4 * (N % 2) - 2) * p;
  //cout << result << endl;
  //// YOUR CODE ENDS HERE
  

    
    cout << "GPU \tResult:" << result << "\tTime:" << end - start << " s" << endl;
    //cout << t << "," << result << "," << end - start <<",\n";
    
    
  
  return 0;
    
}

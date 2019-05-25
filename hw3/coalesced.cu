#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include "cuda_error_handle.h"
using namespace std;
typedef long long int ll;

__global__ void permanent(ll *M,float *X,ll N,float *d_p,ll  chunkSize,int noChunks,ll lim)
{
  ll tid = threadIdx.x + blockIdx.x * blockDim.x;
  if(tid < lim){

    ll  r = (tid)*chunkSize; // each chunk in X array is of size N;
  
  ll  y = r ^ (r>>1LL);
  ll  numSetBits= __popcll(y);
  for( int f=0;f<numSetBits;f++)
    {
      ll bit = __ffsll(y);
      
      for (int  q = 0; q < N; q++)
	{
	    X[q*noChunks + tid] += M[q + N*(bit-1)];
	    
	}
      y = y^(1LL << (bit-1));
      
    } ///value initialization


  ////////////////////////////////////////////
  //  chunkSize+=1;  
  ll start = tid*chunkSize+1;
  ll end = start+chunkSize;
  ll prodSign = start & 1LL;
  ll limiter = lim;//(1LL << (N-1));
  double res = 0;
  if(prodSign)
    prodSign=-1LL;
  else
    prodSign = 1LL;
  for( ll ind=start;ind<end && ind < limiter;ind++)
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
      
      
      float prodX = 1;
     
      for( ll i=0;i<N;i++)
	{
	  X[i*noChunks + tid] += s*M[i + (z-1)*N];
	  prodX *= X[i*noChunks + tid];
	}
      
      res +=  prodSign * prodX;

      
      
      prodSign*=-1;
    }
  atomicAdd(d_p,res);
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
  
  float start, end;
  cudaSetDevice(0);  
  cudaDeviceProp prop;
  cudaGetDeviceProperties(&prop, 0);
  ll threads = prop.maxThreadsPerBlock;
  ll max_blocks = prop.maxGridSize[0];
  ll mem = prop.sharedMemPerBlock; //bytes
  ll float_size = sizeof(float);
  cout << "we can fit  " << mem/(N*float_size) <<  " "<< "gridsize is "<< prop.maxGridSize[0] << endl;

  cout << "Available shared memory is : " << mem << endl;
  int numBlocks = 2048;//max_blocks/N;
  //  threads=1;
  ll totalNumThreads = threads * numBlocks;
  
  //  ll shift =1; 
  ll lim = (1LL<< (N-1));
  ll chunkSize = lim/(totalNumThreads);
  
  if(chunkSize == 0)
    chunkSize = 1;
  ll noChunks = totalNumThreads;//lim/chunkSize ;
  cout << "Chunksize is : " << chunkSize <<  " lim is " <<lim << " and there are "<<noChunks << " chunks and "<<totalNumThreads << " threads "<<  endl;  

  float *local_x_values = new float [noChunks * N];
  ll *new_m = new ll[N * N];
  float *column_sum = new float[N];



  float *d_local;
  ll *d_m;
  float *d_p;
  float *errortest = new float [totalNumThreads];
  ll errorSize = sizeof(float )*(totalNumThreads);
  float *d_errortest;

  CudaSafeCall(cudaMalloc((void **)&d_local,sizeof(float)*(noChunks * N)));
  CudaSafeCall(cudaMalloc((void **)&d_m,sizeof(ll)*(N * N)));

  CudaSafeCall(cudaMalloc((void **)&d_p,sizeof(float)));
  //  CudaSafeCall(cudaMalloc((void **)&d_errortest,errorSize));
  

  for(int i = 0;i<N;i++)
    {
      for(int j = 0;j<N;j++)
	{
	  new_m[i + j*N] = M[i][j];
	  //cout << new_m[i*N + j] << endl;
	}
    }

  
  ////YOUR CODE GOES HERE

  float p = 1;
  
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
  for(int i=0;i<N;i++)
    {
      for(int j=0;j<noChunks;j++)
	{
	  local_x_values[i*noChunks + j] = (M[i][N-1] - float(column_sum[i]/2));
	}
    }

  CudaSafeCall(cudaMemcpy(d_local,local_x_values,sizeof(float)*(noChunks * N),cudaMemcpyHostToDevice) );
  CudaSafeCall(cudaMemcpy(d_m,new_m,sizeof(ll)*N*N,cudaMemcpyHostToDevice));
  CudaSafeCall(cudaMemcpy(d_p,&p,sizeof(float),cudaMemcpyHostToDevice));
  //  CudaSafeCall(cudaMemcpy(d_errortest,errortest,errorSize,cudaMemcpyHostToDevice));
  

  start = omp_get_wtime();      

 
  permanent<<<numBlocks,threads >>>(d_m,d_local,N,d_p,chunkSize,noChunks,lim);
  CudaCheckError();
  end = omp_get_wtime();
  
  CudaSafeCall(cudaMemcpy(&p,d_p,sizeof(float),cudaMemcpyDeviceToHost));
  //CudaSafeCall(cudaMemcpy(errortest,d_errortest,errorSize,cudaMemcpyDeviceToHost));
  
  //for(int i=0;i<totalNumThreads;i++)
  //p+=errortest[i];
  cout << "FINAL P "<<p << endl;
  float result = (4 * (N % 2) - 2) * p;
  //cout << result << endl;
  //// YOUR CODE ENDS HERE
  

    
    cout << "GPU \tResult:" << result << "\tTime:" << end - start << " s" << endl;
    //cout << t << "," << result << "," << end - start <<",\n";
    
    
  
  return 0;
    
}

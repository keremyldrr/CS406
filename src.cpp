#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <stdlib.h>
#include <cmath>
#include<bitset>
using namespace std;


void binary(int i) {
    std::bitset<8*sizeof(int)> b = i;
    std::string s = b.to_string<char>();
    printf("\n%s",s.c_str());
}

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
long double product(double *x,int N){

  long double p = 1;
  for(int i=0;i<N;i++){
    p*=x[i];

  }
  
  return p;
}
long long *generate_gray_coding_sequence(int N){

  long long upperLimit = (1 << (N-1));
  long long *y=new long long[upperLimit];
  for(int i=0;i<upperLimit;i++){
    int temp = i >> 1;
    y[i] =  i ^ temp;
   }

  return y;
}
int* find_changing_bits(long long *y,int N){
  long long lim = (1 << N-1) + 1;
    int j=0;
    int *bitnums =new int[lim];
  
    for(int i=0;i<lim;i++)
      {
	j=i+1;
	long long temp = y[j] ^ y[i];
	bitnums[i] =log2(temp);

      }
    return bitnums;
 
}
int *find_signs(long long *y,int *bitnums,int N,int t){

  int lim = 1<<(N-1);
  int *signs = new int[lim];
  
  
  

      for(int i=0;i<lim;i++){
	int mask = 1 << (bitnums[i]);
	int temp = y[i] & mask;
	if(temp)
	  signs[i] = -1;
	else
	  signs[i] = 1;
      }
  return signs;
}
void updateP(long long *y,double *x,long  double &p,int *bitnums,int *signs,int N,int **M){

  int lim = (1 << N-1)-1;

  
  for(int i=0;i<lim; i++)
    {
      int firstbit = 1 & (i+1);
      int _sign = pow(-1,firstbit);
      
      int col_ind = bitnums[i];

      for(int k = 0;k<N;k++)
	{
	  x[k]+=signs[i]*M[k][col_ind];

	}
      
      p+=  product(x,N) *_sign;
    }
}
double *sum_column_getx(int **M,int N,int num_threads){
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

    double result =0;
    // best of luck :)
    double *x = sum_column_getx(M,N,t);
   
    long double p = product(x,N);
    long long *y =  generate_gray_coding_sequence(N);
    int *bitnums = find_changing_bits(y,N);
    int *signs = find_signs(y,bitnums,N,t);
    updateP(y,x,p,bitnums,signs,N,M);
    result = (4*(N % 2)-2)*p;
    //// YOUR CODE ENDS HERE

    delete [] y;
    delete [] bitnums;
    delete []signs;
    end = omp_get_wtime();

    cout << "Threads: " << t << "\tResult:" << result << "\tTime:" << end - start << " s" << endl;
  }
  for( int i=0;i<N;i++)
    delete [] M[i];
  delete M;
 
  return 0;
  
}

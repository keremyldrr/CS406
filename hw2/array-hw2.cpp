#include <iostream>
#include <omp.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;
void checkGraph(int *row,int *col,int *colors,int N)
{
  int cnt = 0;
  for(int i=0;i<N;i++)
    {
      int ind = i;
      int currColor = colors[ind];
      int start = row[ind];
      int end = row[ind+1];

      for(int n = start;n<end;n++)
	{
	  if(colors[col[n]] == currColor)
	    {
	      cnt++;
	    }
	}
    }
  
  if(cnt > 0)
    {
      cout  << cnt <<  " ERRORS OCCURRED  ";
      return;
    }
  //  cout << "ALL IS WELL ";
  

}

int main(int argc,const char **argv)
{
  if(argc != 2) {
          cout << "No input specified" << endl;
          return 0;
      }
      const char *filename = argv[1];

      //string filepath = "./data/" + filename;

      ifstream file;

      file.open(filename);
      if(file.fail()) {
          cout << "Cannot find given file" << endl;
          return 0;
      }

      string line;
      getline(file,line);

      stringstream s(line);
      bool symmetric = false;
      string word;
      while(s >> word) // check for symmetric
          if(word == "symmetric")
              symmetric = true;

      while(file.peek() == '%')
          file.ignore(2048, '\n');

      int N, M, edge;
      file >> N >> M >> edge;



      cout << "Graph has " << N << " nodes and " << (1+symmetric*1)*edge << " edges and symmetric " << symmetric << endl;
      cout<<"Preprocessing started.."<<endl;

      int n;
      bool based0 = false;
      while(file >> n) {
          if (n == 0) {
              based0 = true;
              break;
          }
      }
      file.close();
     

      file.open(filename);
      while(file.peek() == '%')
          file.ignore(2048, '\n');
      file >> n >> n >> n; //tmp

      // by default all the graphs are 0 based;

      int i,j;
      vector<vector<int>> v(N,vector<int>(0));
      int e = 0;
      while(!file.eof()) {
          file >> i >> j;
          if(!based0) {
              i--;
              j--;
          }
          if(i==j)
              continue;
          v[i].push_back(j);
          v[j].push_back(i);
      }

      for(int i = 0; i < N; i++) {
          if(!v[i].empty()) {
	    sort(v[i].begin(), v[i].end(), greater<int>());
              int j = 0;
              while (j < v[i].size() - 1) {
                  if (v[i][j] == v[i][j + 1]) {
                      v[i].erase(v[i].begin() + j);
                  } else
                      j++;
              }
          }
      }
      file.close();

      for(int i = 0; i < v.size(); i++)
          e += v[i].size();


      edge = e;
      // switch to CRS
      // all non zeros are 1

      int *row = new int[N+1];
      int *col = new int[edge];

      int *colors = new int[N];

      
      row[0] = 0;
      int index = 0;
      for(int i = 0; i < N; i++) {
          for(int j = 0; j < v[i].size(); j++) {
              col[index] = v[i][j];
              index++;
          }
          row[i+1] = index;
      }
      cout << "Preprocessing complete" << endl;
      for(int t = 2; t<=16;t*=2)
	{

	  int *myArr = new int[N];
	  int **arr = new int *[t];
	  int *places = new int [t];
	  omp_set_num_threads(t);
	  
	  for(int i=0;i<t;i++)
	    {
	      arr[i] = new int[N];
	      places[i] = 0;
	    }
	  

#pragma omp parallel proc_bind(spread) //initialization
	  {
	    int tid = omp_get_thread_num();
#pragma omp for schedule(guided)
	  for(int i =0;i<N;i++)
	    {
	      colors[i] = 0;
	      //cout <<tid << " " <<  places[tid] << endl;
	      arr[tid][places[tid]] = i;
	      places[tid]++;
	    }
	  }
	  double begin = omp_get_wtime();	   
	  bool gulag = true;
	  
	  while(gulag)
	    {
#pragma omp parallel proc_bind(spread)
	      {

		int tid = omp_get_thread_num();
		int *forbidden = new int[N]{0};
		int lim = places[tid];//cout << tid <<places[tid] << endl;
		//#pragma omp for schedule(static)
		for(int v = 0; v <lim;v++)
		  {
		    //		    cout << v << endl;
		    int ind = arr[tid][v];
		    int start = row[ind];
		    int end = row[ind+1];
		    
		    for(int i = start;i<end;i++)
		      {
			int dd = col[i];
			forbidden[colors[dd]]= ind;
		      }
		    
		    for(int t = 0;t<N;t++)
		      {
			if(forbidden[t] != ind)
			  {
			    colors[ind] = t;
			    break;
			  }
		      }
		  }
		delete [] forbidden;
	    }
	      
	      if(t>1)
		  {
	  omp_set_num_threads(t);
#pragma omp parallel  proc_bind(spread)
		    {
		      int tid = omp_get_thread_num();
		      //int *hatalar = new int[N];
		      //int bb = 0;
		      int lim = places[tid];
		      places[tid] = 0;
		      //#pragma omp for schedule(static)
		      for(int i=0;i<lim;i++)
			{
			  int ind = arr[tid][i];
			  int currColor = colors[ind];
			  int start = row[ind];
			  int end = row[ind+1];
			  for(int n = start;n<end;n++)
			    {
			      if(col[n] > ind && colors[col[n]] == currColor)
				{
				  arr[tid][places[tid]] = ind;
				  places[tid]++;//erroneus vertice
				  
				}
			      
			    }
			  
			}
		      
		      
		    }
		    int cnt = 0;
		    for(int p = 0;p<t;p++)
		      {
			if(places[p] != 0)
			  {

			    ++cnt;
			  }
			
		      }
		    if(cnt == 0)
		      gulag = false; 
		    //		    else
		    //cout << "errors : " << cnt << endl;
		  }
	      else{
		gulag = false;
	      }
	    }
		  
	  double ends= omp_get_wtime();
	  cout<<"Graph: "<<filename << " Threads: "<<t << " Time: " << ends - begin << " Number of colors: ";
	  checkGraph(row,col,colors,N);
	  int maxy = 0;
	  for(int q = 0;q<N;q++)
	    {
	      if(colors[q] > maxy)
		maxy = colors[q];
	    }
	  
	  cout << maxy<<endl;
	}
      
      
      
      return 0;
}

#include <iostream>
#include <omp.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <set>
using namespace std;


void checkGraph(int *row,int *col,int *colors,int N)
{
  for(int i=0;i<N;i++)
    {
      int ind = col[i];
      int currColor = colors[ind];
      int start = row[ind];
      int end = row[ind+1];

      for(int n = start;n<end;n++)
	{
	  if(colors[col[n]] == currColor)
	    { 
	      cout <<ind << " " << col[n] <<  "ERROR"<<endl;
	      return;
	    }
	}
    }

  cout << "ALL IS WELL" << endl;


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

      int n;
      bool based0 = false;
      while(file >> n) {
          if (n == 0) {
              based0 = true;
              break;
          }
      }
      file.close();
      if(based0)
          cout << "Graph is 0 based" << endl;
      else
          cout << "Graph is 1 based and is being turned in to 0 base." << endl;

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
      int *yassak = new int[N]{0};
      int *colors = new int[N];
      for(int i =0;i<N;i++)
	colors[i]  = -1;

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



      double begin = omp_get_wtime();
    for(int v = 0; v < N;v++)
      {
	int ind = col[v];
	int start = row[ind];
	int end = row[ind+1];
	//	int currColor = colors[ind];
	int cnt = 0;
	for(int i = start;i<end;i++)
	  {
	    if(colors[col[i]] != -1)
	      {
		yassak[colors[col[i]]]= v;
		//	cnt++;
	      }
	  }
	//cout << "Saved " << end-start - cnt  << endl;
	for(int t = 0;t<N;t++)
	  {
	    if(yassak[t] != v)
	      {
		colors[ind] = t;
		break;
	      }
	  }
      }
    double end = omp_get_wtime();

    cout<< "Execution time is: " << end - begin << "seconds" << endl;
    
    checkGraph(row,col,colors,N);
    int maxy = 0;
    for(int q = 0;q<N;q++)
      {
	if(colors[q] > maxy)
	  maxy = colors[q];
	
      }

    cout << "There are " << maxy <<  " colors" << endl;
    return 0;
}

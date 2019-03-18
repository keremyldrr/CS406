#include <stdlib.h>
#include <stdio.h>
#include "omp.h"

#define N 32
#define FS 8

struct node {
   int data;
   int fibdata;
   struct node* next;
};

int fib(int n) {
   int x, y;
   if (n < 2) {
      return (n);
   } else {
      x = fib(n - 1);
      y = fib(n - 2);
	  return (x + y);
   }
}

void processwork(struct node* p) 
{
   int n;
   n = p->data;
   p->fibdata = fib(n);
}

struct node* init_list(struct node* p) {
    int i;
    struct node* head = NULL;
    struct node* temp = NULL;
    
    head = malloc(sizeof(struct node));
    p = head;
    p->data = FS;
    p->fibdata = 0;
    for (i=0; i< N; i++) {
       temp  = malloc(sizeof(struct node));
       p->next = temp;
       p = temp;
       p->data = FS + i + 1;
       p->fibdata = i+1;
    }
    p->next = NULL;
    return head;
}

int main(int argc, char *argv[]) {
  double start, end;
  struct node *p=NULL;
  struct node *temp=NULL;
  struct node *head=NULL;
  struct node **parr = (struct node**)malloc(sizeof(struct node*) * N); 
  int i, count = 0;
  
  printf("Process linked list\n");
  printf("  Each linked list node will be processed by function 'processwork()'\n");
  printf("  Each ll node will compute %d fibonacci numbers beginning with %d\n",N,FS);      
  
  p = init_list(p);
  head = p;
    
  start = omp_get_wtime();
  {
    while (p != NULL) {
      processwork(p);
      p = p->next;
    }
  }
  
  end = omp_get_wtime();
  
  printf("serial Compute Time: %f seconds\n", end - start);
  
  int t;
  for(t = 1; t <= 8; t*=2) {
    omp_set_num_threads(t);
    start = omp_get_wtime();
    {
      // count number of items in the list.  Strictly speaking this isn't 
      // needed since we know there are N elements in the list.  But in 
      // most cases you don't know this and need to count nodes. 
  
      count = 0;
      p = head;
      while (p != NULL) {
	p = p->next;
	count++;
      }
      
      // traverse the list and collect pointers into an array.
      p = head;
      for(i=0; i<count; i++) {
	parr[i] = p;
	p = p->next;
      }
      
      // do the work in parallel 
#pragma omp parallel 
      {
#pragma omp for schedule(static,1)
	for(i=0; i < count; i++) {
	  processwork(parr[i]);
	} 
      }
    }
    end = omp_get_wtime();
   
    //#define DEBUG
#ifdef DEBUG
    p = head;
    while (p != NULL) {
      printf("%d : %d\n",p->data, p->fibdata);
      p = p->next;
    }      
#endif
    printf("thread: %d time: %f seconds\n", t, end - start);
  }

  p = head;
  while (p != NULL) {
    temp = p->next;
    free (p);
    p = temp;
  }  
  free (p);
  

  return 0;
}


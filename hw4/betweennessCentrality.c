#include "defs.h"
#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>

#define MAX_THREADS 320

typedef struct
{
    double *values;
} parallelBCValues;

double betweennessCentrality_parallel(graph* G, double* BC)
{
  double elapsed_time;
  int i;
  // Get number of vertices and edges
  int n = G->nv;
  int m = G->ne;
  // Magic number to determine chunk sizes
  int p = 10;
  int chunkSize = n / p;
  int leftover = n % p;
  // Allocate and initialize the parallel BC structure array
  parallelBCValues *parallelBC = (parallelBCValues *) calloc(n, sizeof(parallelBCValues));
  for (i = 0; i < n; i++)
  {
      parallelBC[i].values = (double *) calloc(p, sizeof(double));
  }
  /* Start timing code from here */
  elapsed_time = get_seconds();
  cilk_for (i = 0; i < p; i++)
  {   
      // Holds current vertex number
      int s;
      // Stack of explored vertices
      int *stack = (int *) malloc(n*sizeof(int));
      // Predecessor list
      plist *P = (plist *) calloc(n, sizeof(plist));
      // Alloc predecessor list lists with lots of magic and hope it works
      int *in_degree = (int *) calloc(n+1, sizeof(int));
      int *numEdges = (int *) malloc((n+1)*sizeof(int));
      int index;
      for (index = 0; index < m; index++)
      {
        int vertex = G->nbr[index];
        in_degree[vertex]++;
      }
      prefix_sums(in_degree, numEdges, n);
      int *pListMem = (int *) malloc(m*sizeof(int));
      for (index = 0; index < n; index++)
      {
        P[index].list = pListMem + numEdges[index];
      }
      free(numEdges);
      // Queue Q
      int *queue = (int *) malloc(n*sizeof(int));
      // Sigma is the number of shortest paths through this node
      double *sigma = (double *) calloc(n, sizeof(double));
      // Dist is the distance of each node from starting
      int *dist = (int *) malloc(n*sizeof(int));
      // Last processor will have different chunk size
      int mySize = chunkSize;
      // Last processor gets leftover nodes
      if (i == p-1)
      {
          mySize += leftover;
      }
      //
      int numV = 100;
      int numTraversals = 0;
      // Main Loop
      for (s = i*chunkSize; s < (i*chunkSize + mySize); s++)
      {
          if (++numTraversals > numV)
	      break;
          // Fresh starting vertex so reset stack and queue
          int topOfStack = 0;
          int frontOfQueue = 0;
          int endOfQueue = 0;
          // Also need to set all Plist entries to empty with the magic from above
          for (index = 0; index < n; index++)
          {
              P[index].degree = in_degree[index];
              P[index].count = 0;
          }
          // Initialize shortest paths to 0
          memset(sigma, 0.0, n*sizeof(double));
          // Shortest path to ourself
          sigma[s] = 1.0;
          // Initialize distances to be -1
          memset(dist, -1, n*sizeof(int));
          // Except ours
          dist[s] = 0;
          // Enqueue ourself
          queue[endOfQueue] = s;
          endOfQueue = (endOfQueue + 1) % n;
          while (endOfQueue != frontOfQueue)
          {
              // Dequeue v from queue
              int v = queue[frontOfQueue];
              frontOfQueue = (frontOfQueue + 1) % n;
              // Push v to stack
              stack[topOfStack] = v;
              topOfStack++;
              // For every neighbor of v
              int j;
              for ( j=G->firstnbr[v]; j<G->firstnbr[v+1]; j++ )
              {
                  // w is the neighbor
                  int w = G->nbr[j];
                  if (dist[w] < 0)
                  {
                      // Enqueue w to queue
                      queue[endOfQueue] = w;
                      endOfQueue = (endOfQueue + 1) % n;
                      // Update distance
                      dist[w] = dist[v] + 1;
                  }
                  if (dist[w] == dist[v] + 1)
                  {
                      // Update shortest paths count
                      sigma[w] += sigma[v];
                      // Append v to predecessor list of w
                      P[w].list[P[w].count++] = v;
                  }
              }
          }
          // Set all deltas to zero
          double *delta = (double *) calloc(n, sizeof(double));
          while (topOfStack > 0)
          {
              // Pop w from stack
              int w = stack[--topOfStack];
              // Loop through every predecessor of w
              for (index = 0; index < P[w].count; index++)
              {
                  // v is a predecessor of w
                  int v = P[w].list[index];
                  delta[v] = delta[v] + (sigma[v] / sigma[w])*(1 + delta[w]);
              }
              // If this isnt the starting vertex, then update BC score
              if (w != s)
              {
                  // Add to this processors copy of BC[w]
                  parallelBC[w].values[i] += delta[w];
                  // BC[w] += delta[w];
              }
          }
          free(delta);
      }
      free(stack);
      free(queue);
      free(P);
      free(sigma);
      free(dist);
      free(pListMem);
      free(in_degree);
  }
  // Sum up BC scores in parallel
  cilk_for (i = 0; i < n; i++)
  {
      int k;
      for (k = 0; k < p; k++)
      {
          BC[i] += parallelBC[i].values[k];
      }
      free(parallelBC[i].values);
  }
  free(parallelBC);
  elapsed_time = get_seconds() - elapsed_time;
  return elapsed_time;
}

/*
 * Serial Version
 *
 */
double betweennessCentrality_serial(graph* G, double* BC) {
  int *S; 	/* stack of vertices in order of distance from s. Also, implicitly, the BFS queue */
  plist* P;  	/* predecessors of vertex v on shortest paths from s */
  double* sig; 	/* No. of shortest paths */
  int* d; 	/* Length of the shortest path between every pair */
  double* del; 	/* dependency of vertices */
  int *in_degree, *numEdges;
  int *pListMem;	
  int* Srcs; 
  int *start, *end;
  int seed = 2387;
  double elapsed_time;
  int i, j, k, p, count, myCount;
  int v, w, vert;
  int numV, num_traversals, n, m, phase_num;

  /* numV: no. of vertices to run BFS from = 2^K4approx */
  //numV = 1<<K4approx;
  n = G->nv;
  m = G->ne;
  numV = n;

  /* Permute vertices */
  Srcs = (int *) malloc(n*sizeof(int));
  for (i=0; i<n; i++) {
    Srcs[i] = i;
  }

  /* Start timing code from here */
  elapsed_time = get_seconds();

  /* Initialize predecessor lists */
  /* Number of predecessors of a vertex is at most its in-degree. */
  P = (plist *) calloc(n, sizeof(plist));
  in_degree = (int *) calloc(n+1, sizeof(int));
  numEdges = (int *) malloc((n+1)*sizeof(int));
  for (i=0; i<m; i++) {
    v = G->nbr[i];
    in_degree[v]++;
  }
  prefix_sums(in_degree, numEdges, n);
  pListMem = (int *) malloc(m*sizeof(int));
  for (i=0; i<n; i++) {
    P[i].list = pListMem + numEdges[i];
    P[i].degree = in_degree[i];
    P[i].count = 0;
  }
  free(in_degree);
  free(numEdges);
	
  /* Allocate shared memory */ 
  S   = (int *) malloc(n*sizeof(int));
  sig = (double *) malloc(n*sizeof(double));
  d   = (int *) malloc(n*sizeof(int));
  del = (double *) calloc(n, sizeof(double));
	
  start = (int *) malloc(n*sizeof(int));
  end = (int *) malloc(n*sizeof(int));

  num_traversals = 0;
  myCount = 0;

  for (i=0; i<n; i++) {
    d[i] = -1;
  }
	
  /***********************************/
  /*** MAIN LOOP *********************/
  /***********************************/
  for (p=0; p<n; p++) {

		i = Srcs[p];
		if (G->firstnbr[i+1] - G->firstnbr[i] == 0) {
			continue;
		} else {
			num_traversals++;
		}

		if (num_traversals == numV + 1) {
			break;
		}
		
		sig[i] = 1;
		d[i] = 0;
		S[0] = i;
		start[0] = 0;
		end[0] = 1;
		
		count = 1;
		phase_num = 0;

		while (end[phase_num] - start[phase_num] > 0) {
				myCount = 0;
				// BFS to destination, calculate distances, 
				int vert;
				for ( vert = start[phase_num]; vert < end[phase_num]; vert++ ) {
					v = S[vert];
					int j;
					for ( j=G->firstnbr[v]; j<G->firstnbr[v+1]; j++ ) {
						w = G->nbr[j];
						if (v != w) {

							/* w found for the first time? */ 
							if (d[w] == -1) {
								//printf("n=%d, j=%d, start=%d, end=%d, count=%d, vert=%d, w=%d, v=%d\n",n,j,start[phase_num],end[phase_num],myCount,vert,w,v);
								S[end[phase_num] + myCount] = w;
								myCount++;
								d[w] = d[v] + 1; 
								sig[w] = sig[v]; 
								P[w].list[P[w].count++] = v;
							} else if (d[w] == d[v] + 1) {
								sig[w] += sig[v]; 
								P[w].list[P[w].count++] = v;
							}
						
						}
					}
	 			}
			
				/* Merge all local stacks for next iteration */
				phase_num++; 
				
				start[phase_num] = end[phase_num-1];
				end[phase_num] = start[phase_num] + myCount;
			
				count = end[phase_num];
		}
 	
		phase_num--;

		while (phase_num > 0) {
			for (j=start[phase_num]; j<end[phase_num]; j++) {
				w = S[j];
				for (k = 0; k < P[w].count; k++) {
					v = P[w].list[k];
					del[v] = del[v] + sig[v]*(1+del[w])/sig[w];
				}
				BC[w] += del[w];
			}

			phase_num--;
		}
		
		for (j=0; j<count; j++) {
			w = S[j];
			d[w] = -1;
			del[w] = 0;
			P[w].count = 0;
		}
  }
  /***********************************/
  /*** END OF MAIN LOOP **************/
  /***********************************/
 

	
  free(S);
  free(pListMem);
  free(P);
  free(sig);
  free(d);
  free(del);
  free(start);
  free(end);
  elapsed_time = get_seconds() - elapsed_time;
  free(Srcs);

  return elapsed_time;
}

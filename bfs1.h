#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"

using namespace std;

#include <iostream>

using namespace std;


 int scan_up(int* A, int* LS, int n)
 {
     int k = n;
     int Curr[n], CLS[n];
     for (int i = 0; i < n; i++) Curr[i] = A[i];
     for (int i = 0; i < n; i++) CLS[i] = LS[i];
     if (n == 1) return A[0];
     int m = n/2;
     int l, r;
     l = scan_up(A, LS, m);
     r = scan_up(A+m, LS+m, n-m);
     LS[m-1] = l;
     return l+r;
 }
 void scan_down(int* A, int* B, int* LS, int n, int offset)
 {
     if (n == 1) {B[0] = A[0] + offset; return; }
     int m = n/2;
     
     scan_down(A, B, LS, m, offset);
     scan_down(A+m, B+m, LS+m, n-m, offset+LS[m-1]);
     return;
 }
 int* inclusive_scan(int* A, int n){
    int* LS = new int[n];
    int* B = new int[n];
    scan_up(A, LS, n);
    scan_down(A, B, LS, n, 0);
    return B;
}

int* exclusive_scan(int* A, int n)
{
    int* LS = new int[n];
    int* B = new int[n];
    B[0] = 0;
    scan_up(A, LS, n);
    scan_down(A, B+1, LS, n, 0);
    return B;
}

 int* pack_flatten(int* arrLen, int** A, int n)
 {
     int S[n];
     int* B = new int[6];
     for (int i = 0; i < n; i++) S[i] = arrLen[i];
     int* offset = exclusive_scan(S, n);
     for (int i = 0; i < n; i++)
     {
         int off = offset[i];
         for (int j = 0; j < S[i]; j++)
         {
             B[off+j] = A[i][j];
         }
    }
    return B;
 }
 
int* filter(int* ngh, int* flag, int n)
{
    
    int* prefix_sum_of_flags = inclusive_scan(flag, n);
    // for (int i = 0; i < n; i++) C[i] = prefix_sum_of_flags[i];
    int* B = new int[prefix_sum_of_flags[n-1]];
    for (int i = 0; i<n; i++)
    {
        if(flag[i])
        {
            int k = ngh[i];
            int h = prefix_sum_of_flags[i];
            B[prefix_sum_of_flags[i]-1] = ngh[i];
        }
    }
    return B;
}

void bfs(int n, int m, int* offset, int* E, int s, int* dist)
{
    // for (int i = 0; i < n; i++) dist[i] = -1;
    int frontierSize = 1;
    int* frontier = new int[frontierSize];
    int curr_dist = 1;
    dist[s] = 0;
    while(frontierSize != 0) //continue until there are no more vertexs to add to the frontier
    {
        int** effective_nghs = new int*[frontierSize];
        int* ngh_len = new int[frontierSize];
        for (int i = 0; i < frontierSize; i++) //loop through every vertex of the frontier 
        {
            int curr_v = frontier[i]; //this is the curr vertex of the frontier we are visiting 
            int k = offset[curr_v+1] - offset[curr_v]; //this is how many neighbors this curr vertex has. 
            // int* temp_arr = new int[k];
            int* flag = new int[k]; //flag array for which of these neightbors will be added to the effective neightbors list, need to change to delayed seq.
            for(int j = 0; j < k; j++) //for every neighbor of the current vertex 
            {
                // temp_arr[i] = E[offset[curr_v]+j];
                if (dist[E[offset[curr_v]+j]] == -1) //the offset of our curr_v could be 3 and then we increment for everyneighbor and check if that vertex has
                //already been visited i.e. is the dist != -1 
                {
                    
                    dist[E[offset[curr_v]+j]] = curr_dist;
                    flag[j] = 1;
                    //add to frontier
                    
                }
            } 
            ngh_len[i] = inclusive_scan(flag, k)[k-1];
            effective_nghs[i] = filter(E+offset[curr_v], flag, k);//getting the effective neighbors for the curr_node
        }
        frontier = pack_flatten(ngh_len, effective_nghs, frontierSize);
        frontierSize =  inclusive_scan(ngh_len, frontierSize)[frontierSize-1]; 
        curr_dist++;
    }
    delete [] frontier;
}

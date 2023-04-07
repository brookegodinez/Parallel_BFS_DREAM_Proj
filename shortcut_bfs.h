#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <vector>
#include "bfs_utils.h"
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
// #include "get_time.h"

using namespace std;



int pre_processing_BFS(int n, int m, int* offset, int* E, int s, int* dist, int p, int**  new_neighbors, int** weights)
{
    int* new_edge_flag = new int[n];  ///different
    int* tmp_weights = new int[n]; ///different
    cilk_for (int i = 0; i < n; i++) //different
    {
        new_edge_flag[i] = 0;
        tmp_weights[i] = 0;
        dist[i] = -1;

    }
    int frontierSize = 1;
    int* frontier = new int[frontierSize]; 
    int curr_dist = 1;
    dist[s] = 0;
    frontier[0] = s; 
    int frontier_num = 0;

    while(frontierSize != 0 && frontier_num < p) //continue until there are no more vertexs to add to the frontier
    {	
        int** effective_nghs = new int*[frontierSize];
        int* ngh_len = new int[frontierSize];
       
        for (int i = 0; i < frontierSize; i++) //loop through every vertex of the frontier  // this is parallel
        {    
	
            int curr_v = frontier[i]; //this is the curr vertex of the frontier we are visiting 
           
            int k = offset[curr_v+1] - offset[curr_v];
	   //this is how many neighbors this curr vertex has.
	  //if (k > 0){ 
            int* flag = new int[k]; //flag array for which of these neightbors will be added to the effective neightbors list, need to change to delayed seq.
	 
            cilk_for(int j = 0; j < k; j++) //for every neighbor of the current vertex 
            //for(int j = 0; j < k; j++)
	        {

                if (dist[E[offset[curr_v]+j]] == -1 && __sync_bool_compare_and_swap(&dist[E[offset[curr_v]+j]], -1, curr_dist)) //the offset of our curr_v could be 3 and then we increment for everyneighbor and check if that vertex has
                //already been visited i.e. is the dist != -1 
                {
                 
                    dist[E[offset[curr_v]+j]] = curr_dist;
                    new_edge_flag[E[offset[curr_v]+j]] =  1; //different
                    tmp_weights[E[offset[curr_v]+j]] = curr_dist; //different
                    flag[j] = 1;
                    //add to frontier
                }
                else 
                {
                    flag[j] = 0;
                }
            }

            int* tmp  = inclusive_scan(flag, k); 
            ngh_len[i] = tmp[k-1];
            delete [] tmp;
            effective_nghs[i] = filter(E+offset[curr_v], flag, k);//getting the effective neighbors for the curr_node
            delete [] flag;
        /*
	} 
	else
	{
		ngh_len[i] = 0;
		
	} */
      }
        frontier_num++; //different
	int* tmp = inclusive_scan(ngh_len, frontierSize);
        int newFrontierSize = tmp[frontierSize-1];
	delete [] tmp;
	delete [] frontier;
	//This is tricky because in preprocessing there is a chance a node has no neighbors so this could result in 
	//memory leaks beacuse these assume something is in these arrays?
        frontier = pack_flatten(ngh_len, effective_nghs, frontierSize, newFrontierSize);
        for (int i = 0; i < frontierSize; i++) delete [] effective_nghs[i];
        delete [] effective_nghs;
        frontierSize = newFrontierSize;
        delete [] ngh_len;
        curr_dist++;
           
    }
    int* temp = inclusive_scan(new_edge_flag, n); //get the size of the new neighbors. 
    int new_offset = temp[n-1]; // save in new offset.
    delete [] temp;
    
   
    new_neighbors[s] = filter_for_dense(new_edge_flag, n);
    weights[s] = filter(tmp_weights, new_edge_flag, n);
    
	delete [] frontier; //only thing not new
    delete [] new_edge_flag;
    delete [] tmp_weights; 
   return new_offset;
}

void new_BFS(int n, int m, int* offset, int* E, int s, int* dist)
{
  
    int** new_neighbors = new int*[n];
    int* temp_new_offset = new int[n];
    //int* new_offset = new int[n+1];
    int** weights = new int*[n]; 
    //int test_v = 12;
    //for (int i = offset[test_v]; i < offset[test_v+1]; i++) cout << E[i] << " " << endl;

    
    int p = 2; 
    
    for(int i = 0; i < n; i++)
    { 
	if (offset[i+1] > offset[i] ) 
	{
		
		temp_new_offset[i] = pre_processing_BFS(n, m, offset, E, i, dist, p, new_neighbors, weights);
	
	}
	else 
	{
		temp_new_offset[i] = 0;
		new_neighbors[i] = new int[0];
		weights[i] = new int[0];
	}
     }
    
    int* new_offset = exclusive_scan(temp_new_offset, n);
   
    int* new_E = pack_flatten(temp_new_offset, new_neighbors, n, new_offset[n]);
    int* new_weights = pack_flatten(temp_new_offset, weights, n, new_offset[n]);
    
    

    
    for (int i = 0; i < n; i++)
    {
            
        delete [] weights[i];
        delete [] new_neighbors[i];
       
    }
    
    delete [] weights;
    delete [] new_neighbors;

    /*
    for(int i = 0; i < n+1; i++) cout << new_offset[i] << " ";
    cout << endl;
    for(int i = 0; i < new_offset[n]; i++){ 
	cout << new_E[i] << " "; 
	if (i % 5 == 0 && i > 0) cout << endl; }
    cout << endl;
    for(int i = 0; i < new_offset[n]; i++) { 
	cout << new_weights[i] << " ";  
	if (i % 5 == 0 && i > 0) cout << endl;}
    */
    shortcut_BFS(n, new_offset[n], new_offset, new_E, s, dist, new_weights);
    delete [] temp_new_offset;
    delete [] new_offset;
    delete [] new_E;
    delete [] new_weights;
    //delete [] tmp;
    //for(int i = 0; i < n; i++) cout << dist[i] << " ";
}


/*
int main()
{
    int n = 7;
    int m = 18;
    int offset[8] = {0,3,6,9,11,14,16,18};
    int E[18] = {1,2,3,0,2,4,0,1,4,0,5,1,2,6,3,6,4,5};
    int* dist = new int[n];
    // BFS(n, m, offset, E, 0, dist);
    new_BFS(n,m,offset,E,0,dist);
    return 0;
} */

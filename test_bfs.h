#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
//#include "get_time.h"

using namespace std;


 int scan_up(int* A, int* LS, int n)
 {
     
     if (n == 1) return A[0];
     int m = n/2;
     int l, r;
    l = cilk_spawn scan_up(A, LS, m);
    //l = scan_up(A, LS, m);
    r = scan_up(A+m, LS+m, n-m);
    cilk_sync;
     LS[m-1] = l;
     return l+r;
 }
 void scan_down(int* A, int* B, int* LS, int n, int offset)
 {
     
     if(n <= 200) 
     {
     	B[0] = offset + A[0];
	for (int i = 1; i < n; i++) B[i] = B[i-1] + A[i];
	return;
     }
     int m = n/2;
    
    cilk_spawn scan_down(A, B, LS, m, offset);
    //scan_down(A, B, LS, m, offset);
    scan_down(A+m, B+m, LS+m, n-m, offset+LS[m-1]);
    cilk_sync;
     return;
 }
 int* inclusive_scan(int* A, int n){
    int* LS = new int[n];
    int* B = new int[n];
   
    if (n < 200)
    {
    	B[0] = A[0];
    	for (int i = 1; i < n; i++)
    	{
    		B[i] = B[i-1] + A[i];
    	}
    }
    else
    {
    	scan_up(A, LS, n);
    	scan_down(A, B, LS, n, 0);
    }
    
    delete [] LS;
    return B;
}

int* exclusive_scan(int* A, int n)
{
    
    int* LS = new int[n];
    int* B = new int[n+1];
    B[0] = 0;
    
    if (n < 100)
    {
    	for(int i = 1; i < n; i++)
    	{
    		B[i] = B[i-1] + A[i-1];
    	}

    }
    else 
    {
        scan_up(A, LS, n);
        scan_down(A, B+1, LS, n, 0);
    }
    
    delete [] LS;
    return B;
}

 int* pack_flatten(int* arrLen, int** A, int n, int m)
 {
     
     int* B = new int[m]; 
   
     int* offset = exclusive_scan(arrLen, n);//new int[n]; 
     
     if (n < 200)
     {
    	for (int i = 0; i < n; i++)
    	{
    		int off = offset[i];
    		for(int j = 0; j < arrLen[i]; j++)
    		{
    			B[off+j] = A[i][j];
    		}
    	}



     }
     else
     {
         //for (int i = 0; i < n; i++)
         cilk_for (int i = 0; i < n; i++)
         {
             int off = offset[i];
             for (int j = 0; j < arrLen[i]; j++)
             {
                 B[off+j] = A[i][j];
             }
          }
     }
    delete [] offset;
    return B;
 }
 
int* filter(int* ngh, int* flag, int n)
{
    if (n > 0){
    	int* prefix_sum_of_flags = inclusive_scan(flag, n);
    
    	int* B = new int[prefix_sum_of_flags[n-1]];
    	if (n < 200)
		{
    		for(int i = 0; i < n; i++)
    		{
    			if(flag[i] == 1)
    			{
    				B[prefix_sum_of_flags[i]-1] = ngh[i];
    			}
    		}

		}
    	else
    	{
        //for (int i = 0; i<n; i++)
        	cilk_for (int i = 0; i<n; i++)
        	{
            		if(flag[i] == 1)
            		{
                		B[prefix_sum_of_flags[i]-1] = ngh[i];
            		}
        	}
    	}
    	delete [] prefix_sum_of_flags;
    	return B;
	}
   else 
   {
	int* tmp = new int[0];
	return tmp;

   }

}

int* filter_for_dense(int* flag, int n)
{
    int* prefix_sum_of_flags = inclusive_scan(flag, n);
    
    int* B = new int[prefix_sum_of_flags[n-1]];
    if (n < 200)
        {
        for(int i = 0; i < n; i++)
        {
                if(flag[i] == 1)
                {
                        B[prefix_sum_of_flags[i]-1] = i;
                }
        }

        }
    else
    {
        //for (int i = 0; i<n; i++)
        cilk_for (int i = 0; i<n; i++)
        {
            if(flag[i] == 1)
            {
                
                B[prefix_sum_of_flags[i]-1] = i;
            }
        }
    }
    delete [] prefix_sum_of_flags;
    return B;
}

void BFS(int n, int m, int* offset, int* E, int s, int* dist, int* weights)
{
    int q = s;
    for (int i = 0; i < n; i++) dist[i] = -1;
    int frontierSize = 1;
    int* frontier = new int[frontierSize];
    int curr_dist = 1;
    dist[s] = 0;
    frontier[0] = s; 
    int* dense_frontier = new int[n];
    int t = n / 2;
    cout << "in BFS";
    while(frontierSize != 0) //continue until there are no more vertexs to add to the frontier
    {
	
    	if (frontierSize <= t){
    	
        	while (frontierSize <= t && frontierSize > 0)
        	{
        	
                int** effective_nghs = new int*[frontierSize];
        	
                int* ngh_len = new int[frontierSize];
                
                for (int i = 0; i < frontierSize; i++) //loop through every vertex of the frontier 
                {
        	
                    int curr_v = frontier[i]; //this is the curr vertex of the frontier we are visiting 
                    
                    int k = offset[curr_v+1] - offset[curr_v]; //this is how many neighbors this curr vertex has. 
                    
                    int* flag = new int[k]; //flag array for which of these neightbors will be added to the effective neightbors list, need to change to delayed seq.
                    cilk_for(int j = 0; j < k; j++) //for every neighbor of the current vertex 
                    //for(int j = 0; j < k; j++)
        	        {
                        
                        if (dist[E[offset[curr_v]+j]] == -1 && __sync_bool_compare_and_swap(&dist[E[offset[curr_v]+j]], -1, curr_dist)) //the offset of our curr_v could be 3 and then we increment for everyneighbor and check if that vertex has
                        //already been visited i.e. is the dist != -1 
                        {
                            
                            dist[E[offset[curr_v]+j]] = dist[curr_v] + weights[offset[curr_v]+j];
                            flag[j] = 1;
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
                }
        	int* tmp = inclusive_scan(ngh_len, frontierSize);
            int newFrontierSize = tmp[frontierSize-1];
        	delete [] tmp;
        	delete [] frontier; //double free???
            frontier = pack_flatten(ngh_len, effective_nghs, frontierSize, newFrontierSize);
            
            for (int i = 0; i < frontierSize; i++) delete [] effective_nghs[i];
            delete [] effective_nghs;
            
            frontierSize = newFrontierSize;
            delete [] ngh_len;
            curr_dist++;
        	}
    	cilk_for(int i = 0; i < n; i++) dense_frontier[i] = 0;
    	//for(int i = 0; i < n; i++) dense_frontier[i] = 0;
    	//for(int i = 0; i < frontierSize; i++) dense_frontier[frontier[i]] = 1;
    	cilk_for(int i = 0; i < frontierSize; i++) dense_frontier[frontier[i]] = 1;
        }
    
    	else if(frontierSize > t)
    	{
    
        	while(frontierSize > t)
        	{
        	
        		int* new_dense_frontier = new int[n];
        		cilk_for(int i = 0; i < n; i++) new_dense_frontier[i] = 0;
        		//for(int i = 0; i < n; i++) new_dense_frontier[i] = 0;
        		//for (int i = 0; i < n; i++)
        		cilk_for(int i = 0; i < n; i++)
        		{
        			if(dist[i] == -1)
        			{
        				int k = offset[i+1] - offset[i];
        				for (int j = 0; j < k; j++)
        				{
        					
        					if(dense_frontier[E[offset[i]+j]] == 1 && __sync_bool_compare_and_swap(&dist[i], -1, curr_dist))
        					{
        						dist[i] = dist[E[offset[i]+j]] + weights[offset[i]+j];
        						new_dense_frontier[i] = 1;
        						
        						break;
        					}
        				}
        			}
        		}
			                         
        		curr_dist++;

        		delete [] dense_frontier;
        		dense_frontier = new_dense_frontier;        		
        		
			int* tmp = inclusive_scan(dense_frontier, n);
        		frontierSize = tmp[n-1];

			delete [] tmp;
        	}
    	
    	delete [] frontier;
    	frontier = filter_for_dense(dense_frontier, n);
        }
    }
	delete [] dense_frontier;
	delete [] frontier;
}

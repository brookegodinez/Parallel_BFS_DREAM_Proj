#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
//#include "get_time.h"
#include "bfs_utils.h"
using namespace std;
/*

 int scan_up(int* A, int* LS, int n)
 {
     //int k = n;
     //int Curr[n], CLS[n];
     //for (int i = 0; i < n; i++) Curr[i] = A[i];
     //for (int i = 0; i < n; i++) CLS[i] = LS[i];
  
   if (n < 50)
     {
     	int k = 0;
	for (int i = 0; i < n; i++)
	{
		if (i % 2 ==0) LS[i] = A[i];
	        else {
			LS[i] = 0;
			for (int j = 0; j < i; j++) LS[i] += A[j];
		}	
	}
     }

     //cout << n << "  ";
     //if (n == 0) return 1;
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
     //if (n == 1) {B[0] = A[0] + offset; return; }
     if(n <= 200) 
     {
     	B[0] = offset + A[0];
	for (int i = 1; i < n; i++) B[i] = B[i-1] + A[i];
	return;
     }
     int m = n/2;
     //cout << "scan down m: " << m << endl;
     cilk_spawn scan_down(A, B, LS, m, offset);
     //scan_down(A, B, LS, m, offset);
     scan_down(A+m, B+m, LS+m, n-m, offset+LS[m-1]);
     cilk_sync;
     return;
 }
 int* inclusive_scan(int* A, int n){
    int* LS = new int[n];
    int* B = new int[n];
    //cilk_spawn scan_up(A, LS, n);
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
    //cilk_sync;
    delete [] LS;
    return B;
}

int* exclusive_scan(int* A, int n)
{
    //cout << "exclusive_scan n: "<< n << " ";
    int* LS = new int[n];
    int* B = new int[n+1];
    B[0] = 0;
    //cilk_spawn scan_up(A, LS, n);
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
    //cilk_sync;
    delete [] LS;
    return B;
}

 int* pack_flatten(int* arrLen, int** A, int n, int m)
 {
     //int S[n];
     int* B = new int[m]; ///
     //for (int i = 0; i < n; i++) S[i] = arrLen[i];
     int* offset = exclusive_scan(arrLen, n);//new int[n]; 
     //int* tmp = exclusive_scan(arrLen, n);
     //if(tmp) offset = tmp;
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
     else{
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
    
    int* prefix_sum_of_flags = inclusive_scan(flag, n);
    // int C[n];
    //cout << "flag array: ";
    //for (int i = 0; i < n; i++) cout << flag[i] << " ";
    //cout << "n: "<< n << " " << "num in the next array: " << prefix_sum_of_flags[n-1] << " ";
    int* B = new int[prefix_sum_of_flags[n-1]];
    if (n < 200)
	{
	for(int i = 0; i < n; i++){
		if(flag[i] == 1)
		{
			B[prefix_sum_of_flags[i]-1] = ngh[i];
		}
	}

	}
    else{
    //for (int i = 0; i<n; i++)
    cilk_for (int i = 0; i<n; i++)
    {
        if(flag[i] == 1)
        {
            // int k = ngh[i];
            // int h = prefix_sum_of_flags[i];
            B[prefix_sum_of_flags[i]-1] = ngh[i];
        }
    }
}
    delete [] prefix_sum_of_flags;
    return B;
}

int* filter_for_dense(int* flag, int n)
{
    int* prefix_sum_of_flags = inclusive_scan(flag, n);
    // int C[n];
    //cout << "flag array: ";
    //for (int i = 0; i < n; i++) cout << flag[i] << " ";
    //cout << "n: "<< n << " " << "num in the next array: " << prefix_sum_of_flags[n-1] << " ";
    int* B = new int[prefix_sum_of_flags[n-1]];
    if (n < 200)
        {
        for(int i = 0; i < n; i++){
                if(flag[i] == 1)
                {
                        B[prefix_sum_of_flags[i]-1] = i;
                }
        }

        }
    else{
    //for (int i = 0; i<n; i++)
    cilk_for (int i = 0; i<n; i++)
    {
        if(flag[i] == 1)
        {
            // int k = ngh[i];
            // int h = prefix_sum_of_flags[i];
            B[prefix_sum_of_flags[i]-1] = i;
        }
    }
}
    delete [] prefix_sum_of_flags;
    return B;
}
*/
void BFS(int n, int m, int* offset, int* E, int s, int* dist)
{
    for (int i = 0; i < n; i++) dist[i] = -1;
    int frontierSize = 1;
    int* frontier = new int[frontierSize];
    int curr_dist = 1;
    dist[s] = 0;
    frontier[0] = s; 
    int* dense_frontier = new int[n];
    int t = sqrt(n);

    while(frontierSize != 0) //continue until there are no more vertexs to add to the frontier
    {
	//cout << "frontier size: " << frontierSize << endl;
	if (frontierSize <= t){
	//cout << "in frontier<n/10 ";	
	while (frontierSize <= t && frontierSize > 0)
	{
	//cout << "frontier size: " << frontierSize << endl;
        int** effective_nghs = new int*[frontierSize];
	//cout << "I still exist here, after effective_ngh";
        int* ngh_len = new int[frontierSize];
        //cout << "I still exist here, after ngh_len";
        // int C[frontierSize];

        // for (int m = 0; m < frontierSize; m++) C[m] = frontier[m];
        //added for dense backward, this is technically a sparse mode
        for (int i = 0; i < frontierSize; i++) //loop through every vertex of the frontier 
        {
	//cout << "I still exist here, in the ffor loop";
            int curr_v = frontier[i]; //this is the curr vertex of the frontier we are visiting 
            // int D[frontierSize];
            // for (int m = 0; m < frontierSize; m++) C[i] = frontier[i];
            int k = offset[curr_v+1] - offset[curr_v]; //this is how many neighbors this curr vertex has. 
            // int* temp_arr = new int[k];
            int* flag = new int[k]; //flag array for which of these neightbors will be added to the effective neightbors list, need to change to delayed seq.
	    //cilk_for(int i = 0; i < k; i++) flag[i] = 0;
            //cout << "I still exist here";
            cilk_for(int j = 0; j < k; j++) //for every neighbor of the current vertex 
            //for(int j = 0; j < k; j++)
	    {
                //int what = E[offset[curr_v]+j];
                // temp_arr[i] = E[offset[curr_v]+j];
                if (dist[E[offset[curr_v]+j]] == -1 && __sync_bool_compare_and_swap(&dist[E[offset[curr_v]+j]], -1, curr_dist)) //the offset of our curr_v could be 3 and then we increment for everyneighbor and check if that vertex has
                //already been visited i.e. is the dist != -1 
                {
                    // int what = dist[E[offset[curr_v]+j]];
                    dist[E[offset[curr_v]+j]] = curr_dist;
                    flag[j] = 1;
		    //cout << "node: "<<E[offset[curr_v]+j] << " dist:" << dist[E[offset[curr_v]+j]] << " ";
                    //add to frontier
                    
                }
                else 
                {
                    flag[j] = 0;
                }
            }
	    //cout << "value of k: " << k << endl;
            //int* tmp  = inclusive_scan(flag, k); 
	    ngh_len[i] = reduce(flag, k);
	    //delete [] tmp;
	    effective_nghs[i] = filter(E+offset[curr_v], flag, k);//getting the effective neighbors for the curr_node
            delete [] flag;
	    //cout << "I still exist here part 2";
        }
	//cout << "frontier size: " << frontierSize; 	
	//int* tmp = inclusive_scan(ngh_len, frontierSize);
        int newFrontierSize = reduce(ngh_len, frontierSize);//tmp[frontierSize-1];
	//delete [] tmp;
        
	delete [] frontier;
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

	else if(frontierSize > t){
	
	while(frontierSize > t)
	{
		
		int* new_dense_frontier = new int[n];
		cilk_for(int i = 0; i < n; i++) new_dense_frontier[i] = 0;
		//for(int i = 0; i < n; i++) new_dense_frontier[i] = 0;
		//for (int i =0; i < n; i++)
		cilk_for(int i = 0; i < n; i++)
		{
			if(dist[i] == -1)
			{
				int k = offset[i+1] - offset[i];
				for (int j = 0; j < k; j++)
				{
					//cout << "found non visited node ";
					if(dense_frontier[E[offset[i]+j]] == 1 && __sync_bool_compare_and_swap(&dist[i], -1, curr_dist))
					{
						dist[i] = curr_dist;
						new_dense_frontier[i] = 1;
						//dense_frontier[offset[i]+j] = 0;
						break;
					}
					else 
					{
						//dense_frontier[offset[i]+j] = 0;
					}
				}
			}
		} 
		curr_dist++;
		
		delete [] dense_frontier;
		dense_frontier = new_dense_frontier;
		
		
		frontierSize = reduce(dense_frontier, n);
		//delete [] tmp;
	}
	
	delete [] frontier;
	frontier = filter_for_dense(dense_frontier, n);
	
    }
           
    }
	
	delete [] frontier;
}


// int main()
// {
//     int* A = new int[8];
//     int* LS = new int[8];
//     int* B = new int[8];
//     for (int i = 1; i < 9; i++)
//     {
//         A[i-1] = i;
//     }
//     scan_up(A, LS, 8);
//     scan_down(A, B+1, LS, 8, 0);
//     for(int i = 0; i < 8; i++){
//         cout << LS[i] << " " ;
//     }
//     B[0] = 0;//exclusive scan
//     cout << "    ";
//     for(int i = 0; i < 8; i++){
//         cout << B[i] << " " ;
//     }
    
//     int R[3][3] = {{2,3,4},{5,6,7},{8,9,10}};
//     cout << "     ";
//     // cout << R[1][5];
//     int ngh[10] = {1,2,3,4,5,6,7,8,9,10};
//     int flag[10] = {0,1,1,0,1,0,0,0,1,1};
//     int* tryin = filter(ngh, flag, 10);
//     for (int i = 0; i < 5; i++)
//     {
//         cout << tryin[i] << " ";
//     }
//     int arr_len[3] = {1,3,2};
//     int t1[1] = {1};
//     int t2[3] = {2,3,4};
//     int t3[2] = {5,6};
//     int* t4[3] = {t1, t2, t3};
//     // cout << t4[1][2];
//     // int* pack = pack_flatten(arr_len, t4, 3);
//     cout << "      ";
//     // for (int i = 0; i < 6; i++)
//     // {
//     //     cout << pack[i];
//     // }
//     int* dist = new int[15];
//     int offset[16] = {0, 1, 8, 11, 13, 14, 16, 18, 23, 27, 30, 33, 34, 40, 43,48 };
//     int E[48] = {6, 7, 7, 8, 9, 12, 13, 13, 5, 7, 13, 9, 12, 5, 2, 4, 0, 14, 1, 1, 2, 10, 14, 1, 10, 11, 14, 1, 3, 12, 7, 8, 12, 8, 1, 3, 9, 10, 14, 14, 1, 1, 2, 6, 7, 8, 12, 12 };
//     for (int i = 0; i<15; i++) dist[i] = -1;
//     bfs(15, 48, offset, E, 0, dist);
//     for (int i = 0; i < 15; i++) cout << dist[i] << "   ";
//     cout << endl;
//     return 0;
// }

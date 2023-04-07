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
void pack_flatten(int* flag, int* A, int n, int* newFrontier);
void scan(int* degrees, int* LS, int n, int* copy);
void scan_down(int* degrees, int* LS, int n, int offset);
int scan_up(int* degrees, int* LS, int n);
int filter(int i);

void BFS(int n, int m, int* offset, int* E, int s, int* dist) {
	
	int* parent = new int[n]; 
	//int* frontierNext = new int[n]; 
	//int* degrees = new int[n];
	int* copy = new int[n];
	//int* LS   = new int[n];A
	int* frontier = new int[n];
	cilk_for(int i = 0;i<n;i++) parent[i] = -1; //delayed deq potential.
	frontier[0] = s; 
	int frontierSize = 1;
	parent[s] = s;
	
	while(frontierSize > 0){
		int* degrees = new int[frontierSize];
		//int* copy = new int[frontierSize];
		int* LS = new int[frontierSize];
		//cout << "frontier list: ";
		cilk_for(int i = 0; i < frontierSize; i++){
			degrees[i] = offset[frontier[i]+1] - offset[frontier[i]];
			//cout << frontier[i] << " ";
			copy[i] = degrees[i];
		}
		//cout << endl;
		//perform prefix sum on degrees array?
     		
		scan(degrees, LS, frontierSize, copy);
		//for (int i  = 0; i < frontierSize; i++) cout << "degrees: " << degrees[i] << " ";//added for test
		//cout << endl; 	
		int k = offset[frontier[frontierSize-1]+1] - offset[frontier[frontierSize-1]];
		//cout << "K: " << k; 
		int* frontierNext =  new int[degrees[frontierSize-1]+ k];
		cilk_for(int i = 0; i<frontierSize; i++){ //index = degrees[i]
			int v = frontier[i], index = degrees[i], d = offset[v+1]-offset[v];
			for (int j = 0; j < d; j++){
				int ngh = E[offset[v]+j];
				if (parent[ngh] == -1 && __sync_bool_compare_and_swap(&parent[ngh], -1, v)){
					frontierNext[index+j] = ngh;
					dist[ngh] = dist[v] + 1;
					//cout << "in here";
				
				}
				else {
					frontierNext[index+j] = -1;
							}
			//cout << frontierNext[index+j] << " ";
			}
			
		}
	//int w = degrees[frontierSize-1]+k;
	//for (int i = 0; i < degrees[frontierSize-1]; i++) cout << frontierNext[i] << " ";
	int* flag = new int[degrees[frontierSize-1]+k];
	int* copy2 = new int[degrees[frontierSize-1]+k];
	//run filter and scan on new 
	for (int i = 0; i < degrees[frontierSize-1]+k; i++){
		flag[i] = filter(frontierNext[i]);
		//cout << "flag" << flag[i] << " ";
		copy2[i] = flag[i];
	}
	scan(flag, LS, degrees[frontierSize-1]+k, copy2);
	//for (int i = 0; i < degrees[frontierSize-1]+k; i++) cout << flag[i] << " ";
	//int* newFrontier = new int[degrees[flag[frontierSize-1]-1]];
	pack_flatten(flag, frontierNext, degrees[frontierSize-1]+k, frontier);
	//cout << "frontier";
	//for (int i = 0; i < degrees[frontierSize-1]+k; i++) cout << frontier[i] << " ";
	//cout << "flag: " << flag[n-1];
	
	//cout << degrees[0] +k;
	frontierSize = flag[(degrees[frontierSize-1]+k)-1];
	//cout << "size: " << frontierSize;
	//delete [] frontier;
 
	//delete [] degrees;
	//delete [] copy;
	//delete [] LS;
	}
	//delete [] parent;
	//dense_backward()		

}
void pack_flatten(int* flag, int* A, int n, int* newFrontier){
	cilk_for(int i = 0; i < n; i++){
		if(filter(A[i])) {
			newFrontier[flag[i]] = A[i];
			cout << A[i] << " ";
		}		
	}

} //frontierNext 
void scan(int* degrees, int* LS, int n, int* copy){
	//if (n == 1) return;
	if (n <= 50) {
		degrees[0] = 0;
		for (int i = 1; i < n; i++){
			degrees[i] = degrees[i-1] + copy[i-1];
}
		return;
} 	
	scan_up(degrees, LS, n);
	degrees[n-1] = 0;
	scan_down(degrees, LS, n, 0);

}

void scan_down(int* degrees, int* LS, int n, int offset){
/*
	if (n < 100){
		degrees[0] = offset + degrees[0];
		for (int i = 0; i < n; i++){
			degrees[i] = degrees[i-1] + degrees[i];
		}
	}*/
	if (n==0) 
	int m = n / 2;
	cilk_spawn scan_down(degrees, LS, m, offset);
	scan_down(degrees+m, LS+m, n-m, offset+LS[m-1]);
	cilk_sync;
}
int scan_up(int* degrees, int* LS, int n){
	if (n <= 1) return degrees[0];
	int l, r;
	int m = n-n/2;
	l = cilk_spawn scan_up(degrees, LS, m);
	r = scan_up(degrees+m, LS+m, n-m);
	LS[m-1] = l;
	cilk_sync;
	return l + r;
}











int filter(int i){
	if (i == -1) return 0;
	else return 1; 

}


/*
void dense_backward(int* frontier, int* new_frontier, int n, int* E){
	cilk_for (int i = 0; i < n; i++) 
	{
		if (visited[i] == 1 && frontier[i] == 1) continue; //skip if the node is already visited or is in the froniter.

		for (int j = i; j < offset[frontier[i+1]]; i++)
		{
			if (fronier[E[j]] == 1){ //found a neighbor in the previous frontier
				new_frontier[i] = 1; //include the current element in the new froniter. 
			
			}
		
		}
	}


}

void next_frontier();

void pack_and_flatten(int* A, int n)
{   ///need to include pack in this as well 
	//this creates a similar representation to the CSR
	int* S = new int[n];
	cilk_for(int i = 0; i < n; i++) S[i] = A[i];
	offset = scan(S, n); //will be scan_enclusive. 
	cilk_for(int i = 0; i < n; i++)
	{
		off = offset[i];
		cilk_for (int j = 0; j < S[i]; j++) B[off+j] = A[i][j];
	
	}
	

}
*/

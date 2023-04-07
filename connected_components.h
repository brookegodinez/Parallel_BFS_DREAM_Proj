#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>


using namespace std;
int BFS(int n, int m, int* offset, int* E, int s, bool* visited)
{
    // for (int i = 0; i < n; i++) dist[i] = -1;
    visited[s] = true;
// 	dist[s] = 0;
        //cout << s << " ";
	int* q = new int[n];
	q[0] = s;
	int start = 0, end = 1;
	while (start != end) {
		int cur_node = q[start];
// 		int cur_dis = dist[cur_node];
		
		for (int i = offset[cur_node]; i < offset[cur_node+1]; i++) {
			int v = E[i];
			if (visited[v] == false) {
				// dist[v] = cur_dis + 1;
				visited[v] = true;
				//cout << v << " ";
				q[end++] = v;
				
			}
		}
		start++;
	}
    delete [] q;
    return end;

}


int connected_components(int n, int m, int* offset, int* E)
{
    int* cc_size = new int[n];
    for (int i = 0; i < n; i++) cc_size[i] = 0;
    int* dist = new int[n];
    bool* visited = new bool[n];
    for (int i = 0; i < n; i++) visited[i] = false;
    cout << "Connected Components: " << endl;
    for (int i = 0; i < n; i++)
    {
        if(visited[i] == false)
        {
            cc_size[i] = BFS(n, m, offset, E, i, visited);
        }
    }
	int largest = -1;
	int most_connected_v;
	for (int i = 0; i < n; i++) 
	{
		if (cc_size[i] > largest)
		{
			largest = cc_size[i];
			most_connected_v = i;
		}
	}
	delete [] cc_size;
	delete [] dist;
	delete [] visited;
	return most_connected_v;
}



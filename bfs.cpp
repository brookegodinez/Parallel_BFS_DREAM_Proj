#include <iostream>
#include <cstdio>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "get_time.h"
#include "parse_command_line.h"
//#include "bfs.h"
#include "bfs3.h"
//#include "bfs2.h"
#include "connected_components.h"
//#include "pre_processing_k1.h"
//#include "shortcut_bfs.h"
//#include "bfs_utils.h"
using namespace std;

#define NUM_ROUNDS 10

void seq_BFS(int n, int m, int* offset, int* E, int s, int* dist) {
	for (int i = 0; i < n; i++) dist[i] = -1;
	
	dist[s] = 0;
	int* q = new int[n];
	q[0] = s;
	int start = 0, end = 1;
	while (start != end) {
		int cur_node = q[start];
		int cur_dis = dist[cur_node];
		//cout << cur_node << endl;		
		for (int i = offset[cur_node]; i < offset[cur_node+1]; i++) {
			int v = E[i];
			if (dist[v] == -1) {
				dist[v] = cur_dis + 1;
				q[end++] = v;
			
			}

		}
		//cout << cur_node << " ";
		start++;
	}
	//cout << start << " ";
	cout << "traversed: " << end << " vertices" << endl;
	delete [] q;
}
	
int main(int argc, char** argv) {
	commandLine P(argc, argv,
		"[-g graph] [-s source]");
	//string graphname = P.getOptionValue("-g", "RoadUSA_sym_wgh.adj");
	//string graphname = P.getOptionValue("-g", "com-orkut_sym.adj");//"soc-LiveJournal_sym.adj");
	string graphname = P.getOptionValue("-g", "soc-LiveJournal_sym.adj");
	//string graphname = P.getOptionValue("-g", "graph6");
	//string graphname = P.getOptionValue("-g", "medium_test_graph");


	string dir = "/usr/local/cs214/";
	//string dir = "/home/csgrads/bgodi007/project2-brookegodinez/";
	string filename = dir+graphname;
	freopen(filename.c_str(), "r", stdin);
	
	int n, m;
	string graphtype; 
	cin >> graphtype;
	cin >> n;
	cin >> m;
	int* offset = new int[n+1];
	int* E = new int[m];
	cout << n << " " << m;
	cout << "reading graph" << endl;
	for (int i  = 0; i < n; i++) cin >> offset[i];
	for (int i  = 0; i < m; i++) cin >> E[i];
	offset[n] = m;
	cout << "finish reading graph" << endl;
	int most_connected_component = connected_components(n, m, offset, E);
        int s = P.getOptionIntValue("-s", most_connected_component);//214A

	//for(int i = 0; i < n; i++) cout << offset[i] << " ";
	//cout << endl;
	//for (int i = 0; i < m; i++) cout << E[i] << " ";
	//cout << endl;
	/*
	int v = connected_components(n, m, offset, E, s);
	cout << v << " " << endl;
	return 0;
	*/


	//correctness check, when you test performance, you can comment this out
	{
	int* dist_check = new int[n];
	timer tx; tx.start();
	

	seq_BFS(n, m, offset, E, s, dist_check);

	double seq_t = tx.stop();
	
	cout << "sequential time: " << seq_t << endl;
	//cout << n << " " << m << endl;

	//for (int i = 0; i < n+1; i++) cout << offset[i] << " ";
	//cout << endl;
	//for (int i = 0; i < m; i++) cout << E[i] << " ";
	//cout << endl;
	int* dist = new int[n];
	//cout << "Running parallel BFS: " << endl;
	BFS(n, m, offset, E, s, dist);

	timer ty; ty.start();


        BFS(n, m, offset, E, s, dist);

        double parallel_t = ty.stop();

        cout << "parallel time: " << parallel_t << endl;



	//cout << "Running wide diameter BFS: ";
	//cout << endl;
	/*This is for wide diameter graphs, where m approaches n. 
	 *
	 * */
	
	//new_BFS(n, m, offset, E, s, dist);
	cout << endl;
	for (int i = 0; i < n; i++) {
		//cout << i << endl;
		//cout << dist[i] << " " << dist_check[i] << endl;
		if (dist[i] != dist_check[i]) {
			cout << i << endl;
			cout << dist[i] << " " << dist_check[i] << endl;
			cout << "wrong answer" << endl;
			//exit(1);
		}
	}

	delete[] dist;
	delete[] dist_check;
	}
	
	{
	
	
	int* dist = new int[n];
	BFS(n, m, offset, E, s, dist);
	delete[] dist;
	}
	//cout << "here after second call";
	double tot_time = 0.0;
	for (int round = 0; round < NUM_ROUNDS; round++) {
		int* dist = new int[n];
		//cout << "here in round " << round << en
		timer t; t.start();
		BFS(n, m, offset, E, s, dist);
		//cout << "here after round" << round << endl;
		t.stop();
		double tm = t.get_total();
		

		cout << "BFS, Round " << round << ", time: " << tm << endl;
		tot_time += tm;

		delete[] dist;
	}
	cout << "BFS, Average time: " << tot_time/NUM_ROUNDS << endl;	
	delete [] offset;
	delete [] E;
	return 0;
}

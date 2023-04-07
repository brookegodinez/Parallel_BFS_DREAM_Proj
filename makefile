CC = g++
CFLAGS = -mcx16 -march=native -DCILK -fcilkplus -std=c++17 -g -O3

all:	bfs

bfs:	bfs.cpp
	$(CC) $(CFLAGS) -DNDEBUG  bfs.cpp -o bfs
	
graph:  smol_graph.cpp
	$(CC)  -DNDEBUG  smol_graph.cpp -o smol_graph 	
clean:
	rm -f bfs

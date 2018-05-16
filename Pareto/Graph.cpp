/*
 * Graph.cpp
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#include "Graph.h"

Graph::Graph(int nodes, int edges, int* num_neighbours) : num_nodes(nodes), num_edges(edges) {
	adjacency_list = alloc_nodes<int*>();
	degrees = alloc_nodes<int>();
	for(int i=0; i < nodes; i++) {
		adjacency_list[i] = new int[num_neighbours[i]];
		degrees[i] = num_neighbours[i]; 	//=0?
	}
}

Graph::~Graph() {
	delete[] adjacency_list;
	delete[] degrees;
	edges.clear();
	edge_list.clear();
	delete edges;
	delete edge_list;
}

template<class T> T*
Graph::alloc_nodes(void)
{
  return new T[get_num_nodes()];	//new int[num_edges]
}

template<class T> T*
Graph::alloc_edges(void)
{
  return new T[get_num_edges()];	//new int[num_edges]
  //return new T[MAX_EDGES];
}

void Graph::add_edge(int i, int j){
	assert(i>j);
	if( !is_edge(i,j)) {
		adjacency_list[i][degrees[i]++]=j;
		adjacency_list[j][degrees[j]++]=i;

		edges.insert(eidx(i,j));
		edge_list.push_back(pair<int,int>(i,j));
		num_edges++;
	}
}

void Graph::del_edge(int i, int j) {
	assert(i>j);
	if( is_edge(i,j) ){
		int pos;
		for(pos=0; pos<degrees[i] && adjacency_list[i][pos]!= j; pos++)
			adjacency_list[i][pos] = adjacency_list[i][degrees[--i]];
	}
}


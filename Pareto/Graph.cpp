/*
 * Graph.cpp
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#include "Graph.h"
#include <algorithm>

Graph::Graph(int nodes, int edges, int* num_neighbours) : num_nodes(nodes), num_edges(edges) {
	adj = new vector<int>[num_nodes];
	//adj = new list<int>[nodes];
	degrees = new int[nodes];

	for(int i=0; i < nodes; i++) {
		adj[i] = new int[num_neighbours[i]];
		degrees[i] = num_neighbours[i]; 	//=0?
	}
}

Graph::~Graph() {
	delete[] adj;
	delete[] degrees;
	//edges.clear();
	//edge_list.clear();
	//delete edges;
	//delete edge_list;
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

//
//void Graph::add_edge(int i, int j){
//	assert(i>j);
//	if( !is_edge(i,j)) {
//		adjacency_list[i][degrees[i]++]=j;
//		adjacency_list[j][degrees[j]++]=i;
//
//		edges.insert(eidx(i,j));
//		edge_list.push_back(pair<int,int>(i,j));
//		num_edges++;
//	}
//}

void Graph::add_edge(int i, int j) {
	assert(i>j);
	adj[i].push_back(j);
	adj[j].push_back(i);
}

//void Graph::del_edge(int i, int j) {
//	assert(i>j);
//	if( is_edge(i,j) ){
//		int pos;
//		for(pos=0; pos<degrees[i] && adjacency_list[i][pos]!= j; pos++)
//			adjacency_list[i][pos] = adjacency_list[i][degrees[--i]];
//	}
//}

void Graph::del_edge(int i, int j) {
	adj[i].erase( (adj[i].begin(), adj[i].end(), j), adj[i].end() );
}
void Graph::load_graph(ifstream *file, string *name) {
	string file_string;
	char* tkn;

	file(name, ifstream::in);
	file_string << file;

	char* buf_file = file_string;
	file->close();

	tkn = strtok(buf_file, "\n");
	while(tkn != nullptr) {
		char* bufline = strtok(tkn, " ;");
		int i = toDigit(bufline);

		while(bufline != nullptr) {
			add_edge(i, toDigit(bufline));
			tkn = (strtok(NULL, " ;"));
		}

		tkn = (strtok(NULL, "\n"));
	}
}

vector< vector<int> > Graph::connected_component_list() {
	bool* visited = new bool[num_nodes];	//array che conta i nodi visitati
	vector< vector<int> > component_list;
	int i=0;

	for(int n=0; n < num_nodes; n++) {
		visited[n] = false;
	}

	for(int n=0; n < num_nodes; n++) {
		if(visited[n]==false) {
			depth_search(n, visited, &component_list[i]);
			i++;
		}
	}
	return component_list;
}

void Graph::depth_search(int n, bool* visited, vector<int>* vect) {
	visited[n] = true;	//il nodo attuale viene segnato come visitato
	vect->push_back(n);

	for(const auto& node : vect) {
		if(!visited[*node])
			depth_search(*node, visited, vect);
	}
}

int Graph::pairwise(vector< vector<int> > component_list) {
	int pairwise = 0;

	for(const auto& component : component_list) {	//itero tutte le componenti connesse
		pairwise += component.size() * (component.size()-1 );
	}
	return pairwise;
}

int toDigit(char *c) {
	return c - '0';
}



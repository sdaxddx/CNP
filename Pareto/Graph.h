/*
 * Graph.h
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#ifndef PARETO_GRAPH_H_
#define PARETO_GRAPH_H_
#include <vector>
#include <set>
#include <cassert>

using namespace std;

class Graph {
protected:
	int num_nodes;
	int num_edges;
	int* degrees;

	int** adjacency_list;
	set<int> edges;

	vector< pair<int,int> > edge_list;

public:
	Graph(int nodes, int edges, int num_neighbours);
	virtual ~Graph();

	template<class T> T *alloc_nodes(void);
	template<class T> T *alloc_edges(void);

	int get_num_nodes() const {return num_nodes; };
	int get_num_edges() const {return num_edges; };

	int get_degree(int i) const
	  {
	    assert(0<=i && i<num_nodes);
	    return degrees[i];
	  }

	inline int eidx(int i, int j) const 	//edge index; assegna un ID a ciascun edge tramite una funzione con parametri i e j
	  {
	    assert(i>j);
	    return ((i*(i-1))>>1) + j; // i * (i-1) diviso due + j..//
	  }

	int* /*const*/ get_adgacency(int i) const {
		assert(0<=i && i<num_nodes);
		    return adjacency_list[i];
		}

	vector< pair<int,int> > get_edge_list() const {
		return edge_list;
		}

	bool is_edge(int i, int j) const //i deve essre maggiore di j
	  {
		return edges.count(eidx(i,j))>0;
	  }

	bool has_edge(int i, int j) const
	  {
	    if( i==j ) return false;
	    return (i>j) ? is_edge(i,j):is_edge(j,i); //se i>j ritorna is_edge(i,j), altrimenti is_edge(j,i)//
	  }


	void add_edge(int i, int j);
	void del_edge(int i, int j);

	int pairwise();

};

#endif /* PARETO_GRAPH_H_ */

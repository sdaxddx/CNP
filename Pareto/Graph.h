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
#include <fstream>
#include <cstring>
#include <list>
#include <bitset>
//#include <unordered_map>

using namespace std;

struct Node {
	unsigned int id;
	unsigned int degree;
	vector<int> neighbours;
};

/*
struct Edge {
	unsigned int id;
	Node* id_from;
	Node* id_to;
	//int cost;
};
*/

class Graph {
protected:
	unsigned int num_nodes;
	unsigned int num_edges;
	int* degrees;

	vector<int> *adj;//array of vectors (nodes are in array, adjacency list in vectors)
	//unordered_map< int, set<int> > adj;

	set<int> edges;	//set of edges
	/*
	 * int** adjacency_list;
	 *
	 *vector< pair<int,int> > edge_list;
	 */

	/****************************************
	 ***** 		 Diverso sistema 		*****
	 ***************************************/
	//vector<Node> node_vector;

public:
	Graph(int nodes, int edges, int *num_neighbours);
	Graph(FILE* f);
	Graph(Graph* g);
	virtual ~Graph();

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

	vector<int>* /*const*/ get_adgacency(int i) const {
		assert(0<=i && i<num_nodes);
		    return &adj[i];
		}
//
//	vector< pair<int,int> > get_edge_list() const {
//		return edge_list;
//		}

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
	void del_node(int i);

	int get_max_degree();
	int gen_vertex_cover(vector<int>& res);

	void update_Graph(std::bitset* removed_nodes);

	void load_graph(ifstream *file, string *name);

	vector<vector<int>> connected_component_list();
	int components(bitset& del_set, int *components, int *comp_size); //passing a deletion bitset, it returns the number of components and assign each node to a component

	void depth_search(int n, bool* visited, vector<int>* vect);

	int pairwise();
};

//class Reduced_Graph: public Graph {
//public:
//	Reduced_Graph(Graph original, set<int> deleted_vertices) : Graph(int nodes, int edges, int* num_neighbours);
//	virtual ~Reduced_Graph();
//};

#endif /* PARETO_GRAPH_H_ */

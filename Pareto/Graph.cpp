/*
 * Graph.cpp
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#include "Graph.h"
#include <algorithm>
#include <queue>

Graph::Graph(int nodes, int edges, int* num_neighbours) : num_nodes(nodes), num_edges(edges) {
	adj = new vector<int>[nodes];   //creo l'array di vettori
	//adj = new list<int>[nodes];
	degrees = new int[nodes];

	for(int i=0; i < nodes; i++) {
		adj[i] = vector<int>(num_neighbours[i]);    //ogni vettore i contiene i vicini di i
		degrees[i] = num_neighbours[i]; 	// = adj[i].size();
	}
}

//TODO
Graph::Graph(FILE *f) {

}

Graph::Graph(Graph* g) :num_nodes(g->num_nodes), num_edges(g->num_edges), adj(g->adj), degrees(g->degrees) {
}

Graph::~Graph() {
	delete[] adj; // cancel vectors?
	delete[] degrees;
	//edges.clear();
	//edge_list.clear();
	//delete edges;
	//delete edge_list;
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

void Graph::add_edge(int i, int j) {    //TODO ottimizzare inserimento
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

void Graph::del_edge(int i, int j) {    //TODO ottimizzare eliminazione
    assert(i>j);
    unsigned size_i = adj[i].size();
    unsigned size_j = adj[j].size();

    for (int n = 0; n < size_i; ++n) {
        if (n == j) {
            adj[i].erase(adj[i].begin() + n); //se n == j cancello n dal vettore di adiacenza
            break; //perché grafo semplice senza loop;
        }
    }

    for (int m = 0; m < size_j ; ++m) {
        if(m == i) {
            adj[j].erase(adj[j].begin()+m);
            break;
        }
    }
}

void Graph::del_node(int i) {
	num_nodes--;
	for(auto const& node : adj[i]) {	//cancel all adjacency node of i
		del_edge(i, node);
		num_edges--;
	}
	adj.erase( (adj.begin(), adj.end(), adj[i]), adj.end() );	//cancel i from the adjacency list

}

int Graph::get_max_degree() {
	int max = 0;
	for(int i = 0; i<num_nodes; i++) {
		if (degrees[i] > max)
			max = degrees[i];
	}
	return max;
}

int Graph::gen_vertex_cover(vector<int>& res) {
	vector<int> deg(num_nodes);
	std::bitset<num_nodes> C;
	int edges = 0, cur = 0;


	for(int i=0; i<num_nodes; i++) {
		deg[i] = this->degrees[i];
		C[i] = 0;
	}

	  do {
		  int maxi = 0;
		  for(int i=0; i<num_nodes; i++) {
		      if(deg[i] > maxi && C[i] == 0)
		    	  maxi = deg[i];
		      	  cur = i;
		    }

	      C[cur] = 1;
	    //cout<<"cur==-1 => "<<cur<<endl;

	    //cout<<"Qui, cur="<<cur<<endl;

	    for(int j=0; j<this->degrees[cur]; ++j) { //per tutti i vicini del nodo corrente
	    	if(C[adj[cur][j]] == 0) {
	    		deg[adj[cur][j]]--;
	    		edges++;
	    	}
	    }
	  } while(edges<num_edges);

	  res.clear();
	  for(int i=0; i<num_nodes; ++i) {
	    if( C[i] == 1 ) {
	      res.push_back(i);
	    }
	  }

	  return C.size();

}

void Graph::update_Graph(std::bitset* removed_nodes) {
	for(std::size_t i = 0; i < removed_nodes->size(); ++i) {	//for all nodes passed delete them from graph
		if (removed_nodes[i] == 1)
			del_node(i);
	}
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

	for(int n=0; n < num_nodes; n++) {	//inizializzo tutti i nodi come NON visitati
		visited[n] = false;
	}

	for(int n=0; n < num_nodes; n++) {	//itero tutti i nodi guardando se sono visitati o meno
		if(visited[n]==false) {	//se non sono visitati faccio una DEPTH SEARCH
			depth_search(n, visited, &component_list[i]);
			i++;	//poich� ho trovato tutti i nodi di una componente passo alla successiva
		}
	}
	return component_list;
}
int Graph::components(bitset &del_set, int *components, int *comp_size) {
	int count=0, current_size; //number of components and size of each one

	queue<int> S;	//coda con nodi da esaminare

	for (int i = 0; i < num_nodes; ++i) {
		components[i] = -1;
	}

	for (int i = 0; i <num_nodes ; ++i) {
		if(!del_set[i] && components[i]<0) {	//se il nodo non è nei cancellati e non appartiene già a una componente
			S.push(i);
			components[i] = count;	//assegno il nodo i alla componente attuale
			current_size = 1;	//inizializzo il numero di nodi nella componente

			while (!S.empty()) {	//finché esistono nodi da esaminare (adiacenti a i e a cascata)
				int k = S.front();	//seleziono il primo elemento
				S.pop();	//e lo elimino poiché lo sto esaminando

				for (int j = 0; j < degrees[k] ; ++j) {
					const int adj_node = adj[k][j];	//per tutti i nodi adiacenti a quello esaminato

					if(!del_set[adj_node] && components[adj_node]<0) {	//se il nodo non è nei cancellati e non appartiene a una componente
						components[adj_node] = count;	//lo inserisco nella componente attuale
						current_size++;	//aumento la dimensione della componente
						S.push(adj_node);	//e inserisco il nodo in quelli da esaminare
					}
				}
			}
			if(comp_size)
				comp_size[count] = current_size; //assegno a ogni componente la sua dimensione
			count++; //passo alla componente successiva
		}
	}
	return ++count;
}


void Graph::depth_search(int n, bool* visited, vector<int>* vect) {
	visited[n] = true;	//il nodo attuale viene segnato come visitato
	vect->push_back(n);	//aggiungo il nodo alla componente attuale

	for(const auto& node : adj[n]) {	//itero per tutti i nodi adiacenti
		if(!visited[node])	//se il nodo non � visitato applica una DEPTH SEARCH
			depth_search(node, visited, vect);
	}
}

int Graph::pairwise() {
	int pairwise = 0;
	vector< vector<int> > component_list = connected_component_list();
	for(const auto& component : component_list) {	//itero tutte le componenti connesse
		pairwise += component.size() * (component.size()-1 );
	}
	return pairwise;
}

int toDigit(char *c) {
	return (int) c - '0';
}

//Reduced_Graph::Reduced_Graph(Graph original, set<int> deleted_vertices) : Graph(int nodes, int edges, int* num_neighbours) {
//	adj = new vector<int>[num_nodes];
//	//adj = new list<int>[nodes];
//	degrees = new int[nodes];
//
//	for(int i=0; i < nodes; i++) {
//		adj[i] = new int[num_neighbours[i]];
//		degrees[i] = num_neighbours[i]; 	//=0?
//	}
//}
//
//Reduced_Graph::~Reduced_Graph() {

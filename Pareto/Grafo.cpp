//
// Created by Alessio on 26/09/2018.
//

#include <iostream>
#include <sstream>
#include <algorithm>
#include "Grafo.h"

using namespace Graph;


std::vector<std::string> split_adj(const std::string& s, char delimiter);


Grafo::Grafo(unsigned nodes, unsigned edges, int *num_neighbours) : num_vertices(nodes), num_edges (edges), degrees(num_neighbours)  {
    adj_list = new std::vector<int>[nodes];   //creo l'array di vettori
    //adj = new list<int>[nodes];

    for(int i=0; i < nodes; i++) {
        adj_list[i] = std::vector<int>(num_neighbours[i]);    //ogni vettore i contiene i vicini di i
        degrees[i] = num_neighbours[i]; 	// = adj[i].size();
    }
}

Grafo::Grafo(std::ifstream& file) {
    std::string line;
    //std::ifstream file("C:\\Users\\Alessio\\CLionProjects\\CNP\\Pareto\\Graph_Example.txt");


    if(file.is_open())  {
        std::vector<std::string> temp_adj;  //oggetto temporaneo per il passaggio dei dati
        getline(file, line);    //prendo la prima riga, ovvero il numero di nodi totali

        int nodes = std::stoi(line);
        this->num_edges = nodes;

        degrees = new int[nodes]();   //creo l'array dei gradi e la lista di adiacenza
        adj_list = new std::vector<int>[nodes];
        std::cout<<nodes<< "\n";

        int curr_node = 0;

        while(getline(file, line)) {    //per ogni nodo aggiungo i nodi adiacenti al suo vettore di adiacenza
            temp_adj = split_adj(line.substr(line.find(':')+1), ' ');   //elimino il primo numero che indica il nodo e i due punti, e divido la stringa per spazi

            for (const std::string& a : temp_adj) { //finché ho vicini li aggiungo alla lista di adiacenza del nodo
                if (!a.empty()) {
                    int i = std::stoi(a, nullptr, 10);
                    adj_list[curr_node].push_back(i);
                    degrees[curr_node]++;

                    std::cout << i << " ";
                }
            }
            curr_node++;    //passo al nodo successivo

            std::cout<< "\n";
        }
        file.close();   //chiudo il file

    }

    else
        std::cout<< "File not open\n";

}

Grafo::Grafo(const Graph::Grafo& grafo): num_vertices(grafo.num_vertices), num_edges(grafo.num_edges){
    this->adj_list= grafo.adj_list; //vector = copia il vettore
    degrees = new int[num_vertices];    //creo array dei gradi
    for (int i = 0; i < num_vertices ; ++i) {
        this->degrees[i] = grafo.degrees[i];
    }
}

Grafo::~Grafo() {
    delete degrees;
    delete[] adj_list;
}

int Grafo::get_max_degree() {
    int c = adj_list[0].size();
    for (int i = 1; i < adj_list->size() ; ++i) {
        if(adj_list[i].size() > c)
            c = adj_list[i].size();
    }
    return c;
}

void Grafo::del_edge(int i, int j) {    //TODO ottimizzare eliminazione //se i<j scambiare i dati
    assert(i>j);
    unsigned size_i = adj_list[i].size();
    unsigned size_j = adj_list[j].size();

    for (int n = 0; n < size_i; ++n) {
        if (n == j) {
            adj_list[i].erase(adj_list[i].begin() + n); //se n == j cancello n dal vettore di adiacenza
            break; //perché grafo semplice senza loop;
        }
    }

    for (int m = 0; m < size_j ; ++m) {
        if(m == i) {
            adj_list[j].erase(adj_list[j].begin()+m);
            break;
        }
    }
}

void Grafo::del_node(int i) {
	num_vertices--;
	for(auto const& node : adj_list[i]) {	//cancel all adjacency node of i
		del_edge(i, node);
		num_edges--;
	}
	adj_list[i].clear();	//erase all elements from adjacency list of i
}

void Grafo::update_Graph(std::bitset<500>* removed_nodes) {
    for(std::size_t i = 0; i < removed_nodes->size(); ++i) {	//for all nodes passed delete them from graph
        if (removed_nodes[i] == 1)
            del_node(i);
    }
}

int Grafo::gen_vertex_cover(std::vector<int>& res) {
    std::vector<int> deg(num_vertices); //vettore dei gradi
    std::bitset<500> C;    //vettore nodo eliminato
    int edges = 0, cur = 0;


    for(int i=0; i<num_vertices; i++) {
        deg[i] = this->degrees[i];
        C[i] = 0;
    }

    do {
        int maxi = 0;
        for(int i=0; i<num_vertices; i++) {
            if(deg[i] > maxi && C[i] == 0)
                maxi = deg[i];
            cur = i;
        }

        C[cur] = 1;
        //cout<<"cur==-1 => "<<cur<<endl;

        //cout<<"Qui, cur="<<cur<<endl;

        for(int j=0; j<this->degrees[cur]; ++j) { //per tutti i vicini del nodo corrente
            if(C[adj_list[cur][j]] == 0) {
                deg[adj_list[cur][j]]--;
                edges++;
            }
        }
    } while(edges<num_edges);

    res.clear();
    for(int i=0; i<num_vertices; ++i) {
        if( C[i] == 1 ) {
            res.push_back(i);
        }
    }

    return C.size();

}

std::vector< std::vector<int> > Grafo::connected_component_list() {
    auto* visited = new bool[num_vertices]();	//array che conta i nodi visitati, inizializzati a FALSE
    std::vector< std::vector<int> > component_list (0);
    int curr_comp=0;

    /*
     * for(int n=0; n < num_vertices; n++) {	//inizializzo tutti i nodi come NON visitati    //INUTILE
     * visited[n] = false;
     *  }
     */

    for(int n=0; n < num_vertices; n++) {	//itero tutti i nodi guardando se sono visitati o meno
        if(!visited[n]) {	//se non sono visitati faccio una DEPTH SEARCH
            std::cout << "controllo nodo:" << n<< " \n";
            component_list.emplace_back( std::vector<int>());
            depth_search(n, visited, component_list[curr_comp]);
            curr_comp++;	//poich� ho trovato tutti i nodi di una componente passo alla successiva
        }
    }
    return component_list;
}
//TODO CHECK INTEGRITY
void Grafo::depth_search(int n, bool *visited, std::vector<int>& vect) {
    visited[n] = true;	//il nodo attuale viene segnato come visitato
    vect.emplace_back(n);	//aggiungo il nodo alla componente attuale

    std::cout << "visita n: " << n<< " e valore attuale: "<<vect.size()<< "\n";

    for(const int& node : adj_list[n]) {	//itero per tutti i nodi adiacenti
        if(!visited[node])	//se il nodo non � visitato applica una DEPTH SEARCH
            depth_search(node, visited, vect);
    }
}

int Grafo::pairwise() {
    int pairwise = 0;
    std::vector< std::vector<int> > component_list = connected_component_list();
    for(const auto& component : component_list) {	//itero tutte le componenti connesse
        pairwise += component.size() * (component.size()-1 );
    }
    return pairwise;
}

std::vector<std::string > split_adj(const std::string& s, char delimiter)    //divide "s" in tante stringhe delimitate da "delimiter", e mi ritorna un vettore contenente queste
{
    std::vector<std::string > tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    std::ifstream file("C:\\Users\\Alessio\\CLionProjects\\CNP\\Graph_Example.txt");
    Grafo prova(file);

    prova.del_node(11);

    std::cout<< "\n \n \n \n \nResult :"<< prova.get_adj_list()[36].size() ;

    return 0;
}


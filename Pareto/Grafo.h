//
// Created by Alessio on 26/09/2018.
//

#ifndef PROJECT_GRAFO_H
#define PROJECT_GRAFO_H

#include <vector>
#include <set>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <bitset>

namespace Graph {

    class Grafo {
    private:
        std::vector<int>* adj_list; //array di nodi, con ogni nodo un vettore di vertici adiacenti
        unsigned num_vertices, num_edges;   //num vertici e archi
        std::set<int> edges; //insieme di archi
        int* degrees;   //grado di ogni vertice; (size num_vertices)

    public:
        Grafo(unsigned nodes, unsigned edges, int *num_neighbours);
        explicit Grafo(const Grafo&);
        explicit Grafo(std::ifstream& file);
        virtual  ~Grafo();

        void addEdge(int source, int target);
        void del_edge(int i, int j);
        void del_node(int i);

        inline std::vector<int>& operator[] (int vertex) { assert(0<=vertex && vertex<num_vertices);    return adj_list[vertex];}    //subscription mi ritorna la lista di adiacenza del vertice passato
        inline std::vector<int> operator[] (int vertex) const {assert(0<=vertex && vertex<num_vertices);    return adj_list[vertex]; }   //mi ritorna la copia della lista di adiacenza del vertice passato
        bool is_edge(int source, int target);

        int get_num_nodes() const {return num_vertices; };  //getters numeri vertici e archi
        int get_num_edges() const {return num_edges; };
        inline std::vector<int >* get_adj_list() const {
            return adj_list;
        };

        int get_degree(int vertex) const
        {
            assert(0<=vertex && vertex<num_vertices);
            return degrees[vertex];
        }

        /**
         *
         * @param source vertex from (must be greater than target)
         * @param target vertex to  (must be smaller than source)
         * @return index associated to edge
         */
        inline int edge_index(int source, int target) const 	//edge index; assegna un ID a ciascun edge tramite una funzione con parametri source e target
        {
            assert(source>target);
            return ((source*(source-1))>>1) + target; // source * (source-1) diviso due + target..//
        }

        bool is_edge(int i, int j) const //i deve essre maggiore di j
        {
            return edges.count(edge_index(i,j))>0; //=1, poichè presente una volta sola, essendo una funzione suriettiva? (e set)
        }

        bool has_edge(int i, int j) const
        {
            if( i==j ) return false;
            return (i>j) ? is_edge(i,j):is_edge(j,i); // ritorna falso se vertici uguali, altrimenti ritorna la funzione is_edge con i parametri nel giusto ordine
        }

        int get_max_degree();
        int gen_vertex_cover(std::vector<int>& res);
        void update_Graph(std::bitset<500>* removed_nodes);
        void open_file(std::ifstream& ifs, std::string* name);


        std::vector<std::vector<int>> connected_component_list();
        void depth_search(int n, bool* visited, std::vector<int>& vect);
        //int components(std::bitset& del_set, int *components, int *comp_size); //passing a deletion bitset, it returs the number of components and assign each node to a component


        int pairwise();


    };
}

#endif //PROJECT_GRAFO_H

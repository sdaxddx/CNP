
#ifndef __DEFNS_HPP__  //perche questo if iniziale???//

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <climits>

#include <fstream>

#include <vector>
#include <set>
#include <string>

//#include <ilcplex/cplex.h>
using namespace std;

#define __DEFNS_HPP__

const char *errmsg(const char *fmt, ...);

#define MAX_NODES 30000
#define MAX_EDGES ((MAX_NODES*(MAX_NODES-1))>>1)    //diviso due

//#define IL_STD

typedef unsigned int BitCell;

class BitSet {
protected:
  static const size_t CellSize;
  int Nr_Cells;
  BitCell *buf;

  void init(int n);
  inline int Cell_idx(int x) const { return (x/CellSize); } //inline = macro in c++
  inline int Cell_ofs(int x) const { return x%CellSize; }
  inline BitCell mask(int x) const { return ((BitCell)1)<<Cell_ofs(x); } //<<moltiplicazione pr 2 o multipli
public:
  int size;
  BitSet(int maxsize);
  BitSet(const BitSet& S);
  ~BitSet();

  inline int max_size(void) const { return Nr_Cells*CellSize; }
  inline void add(int x) {if(!member(x)) size++; buf[Cell_idx(x)]|=mask(x);}
  inline void del(int x) {if(member(x)) size--; buf[Cell_idx(x)]&= ~mask(x);}
  inline bool member(int x) const { return (buf[Cell_idx(x)]&mask(x))!=0; }
  inline int cell_card(BitCell x) const {
    int res=0;
    for(res=0; x; x>>=1) {
      if( x&((BitCell)1) )
	++res;
      x>>=1;
    }
    return res;
  }
  void merge(const BitSet& S);
  void intersect(const BitSet& S);
  void copy(const BitSet& S);
  void clear(void);
  bool equal(const BitSet& S);

  int get_hamming_distance_from(const BitSet& S) const;

  void dump(void);

};

ostream& operator<<(ostream& s, const BitSet& S); //cosa sgnifica??//

template<class T> void Alloc(T*&x, int n)
{
  x=new T[n];
  assert(x!=NULL);
}


template<class T>
class Queue {
protected:
  T *buf;
  int buflen;
  int head, tail;
  int cnt;

public:
  Queue(int maxsize)
    : buf(NULL), head(0), tail(0), cnt(0) 
  {
    Alloc<T>(buf, maxsize); // uso del template, crea un puntatore di tipo T a un vettore di nome buf, tipo T e dimensione maxsize
    buflen=maxsize;
  }
  ~Queue() { if( buf ) delete[] buf; }

  bool is_empty(void) const 
  {
    return cnt==0;
  }

  bool is_full(void) const 
  {
    return cnt==buflen;
  }

  int enqueue(int x)
  {
    assert( !is_full() );
    buf[tail++]=x; 
	tail%=buflen;
	return ++cnt;
  }

  int dequeue(void)
  {
    assert(!is_empty());
    int res=buf[head++];
	cnt--; 
	head%=buflen;
	return res;
  }
};


template<class T>
struct SortByWeight {
  vector<T> buf;
  SortByWeight(const vector<T>& x) 
    : buf(x){}
  bool operator()(int a, int b) {
    return buf[a]<buf[b];
  }
};

class individual {
protected:
  int dim;

public:
  int obj, *nodes;
  BitSet *B;
  int dimB;
  int point;
  int pointed;

  individual(int dimpb, int NumNodes);
  ~individual();
  void clear();
  void copy(individual *ind);
  //double weight(double gamma, individual best, int K);

};

class Graph {
protected:
  int Nr_nodes;
  int Nr_edges;

  set<int> edge_set;

  int **adj_list;
  int *deg;

  vector<pair<int, int> > edge_list;
  //set<pair<int, int> > edge_set;

public:

  template<class T> T *alloc_nodes(void);
  template<class T> T *alloc_edges(void);
  int numnode;

  Graph(int n, int m, int *sizeneigh);
  Graph(const char *fname); // alternatively load directly from file name (easier)
  ~Graph();

  inline int NumNodes(void) const { return Nr_nodes; }//ok, const indica che la funzione non cambia i valori degli attributi di istanze della classe graph
  inline int NumEdges(void) const { return Nr_edges; }


  inline int eidx(int i, int j) const 
  {
    assert(i>j);
    return ((i*(i-1))>>1) + j; // i * (i-1) diviso due + j..//
  }

    i) const
    {
        assert(0<=i && i<Nr_nodes);
        return deg[i];
        inline int degree(int
  }

  inline const int *adj(int i) const 
  {
    assert(0<=i && i<Nr_nodes); 
    return adj_list[i];
  }

  inline const vector<pair<int, int> >& EdgeList(void) const 
  {
    return edge_list;
  }

  inline bool is_edge(int i, int j) const 
  { 
    //pair<int, int> tmp(i,j);
    //return edge_set.count(tmp);
    return edge_set.count(eidx(i,j))>0; //count funzione di set, conta il numero di container con quel valore, puo ssere o 0 o 1, perch� n set solo valori diversi//
  }

  inline bool has_edge(int i, int j) const 
  {
    if( i==j ) return false;
    return (i>j) ? is_edge(i,j):is_edge(j,i); //se i>j ritorna is_edge(i,j), altrimenti is_edge(j,i)//
  }

  void add_edge(int i, int j, int *sizeneigh);
  void del_edge(int i, int j);

  int evaluate_obj(const BitSet& delSet) const;
  int components(const BitSet& DelSet, int *label, int *size=NULL) const;
  int evaluate_node(const BitSet& DelSet, int i) const;
  void increase_density(int Kmax, int *sizeneigh);

  void update_components_after_add_node(BitSet& DelSet, int i, 
					int *label, int *size, int *Nr_comp);
  int update_components_after_add_node_CC(BitSet& DelSet, int i, 
					int *label, int *size, int *Nr_comp);
  int evaluate_add_node(const BitSet& DelSet, int i, int *label, int *size);
  int evaluate_add_node_CC(const BitSet& DelSet, int i, int *label, int *size);

  void gen_random_vertex_cover(vector<int>& res);
  void gen_random_vertex_cover_sub(vector<int>& res, BitSet& sub);
  void gen_random_vertex_cover(vector<int>& res, const vector<int>& w);
 
  void gen_random_vertex_cover_maxcomp(vector<int>& res, BitSet& S);

  void read_from_file(const char *fname, int *sizeneigh);
  void printgraph_vc(BitSet& S);
  int components_single(const BitSet& DelSet, int *label, int *size, int node, int num_comp) const ;
  int components_del_node(const BitSet&, int, int*, int*, int) const;

  void eval_obj_general(int C[], const BitSet& DelSet);
  int update_components_after_add_node_general(BitSet& DelSet, int i, int *label, int *size, int *Nr_comp);
  void update_components_after_del_node_general(int C[4], int i, int *label, int *size, int *Nr_comp);
  int heur_add_general(int K, vector<int>& res, int n, char x, int partial, BitSet *S);
  vector<int> evaluate_add_node_general(int n, const BitSet& DelSet, int i, int *label, int *size);
  int heur_del_avoid_general(int n, char x, int K, vector<int>& resplus, vector<int>& resminus, BitSet& DelSet, int version, BitSet& nodes_avoid);
};



#endif


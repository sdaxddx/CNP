#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>
#include <set>
#include <list>

#include <iterator>
#include <sys/time.h>

#include <algorithm>

#include <math.h>

#include "defns.hpp" 

#define __debug_vertex_cover__ 0
#define __debug_add_one_node__ 0
#define __debug_heur_add__ 0
#define __debug_evaluate_obj__ 0

using namespace std;


const size_t BitSet::CellSize = sizeof(BitCell)*8;

void
BitSet::clear(void)
{
  memset(buf, (BitCell)0, Nr_Cells*sizeof(BitCell));
  size = 0;
  /*
  for(int i=0; i<Nr_Cells; i++) 
    buf[i]=(BitCell)0;
  */
}

void
BitSet::init(int n)
{
  Nr_Cells= 1+Cell_idx(n-1);
  buf=new BitCell[Nr_Cells];
}

BitSet::BitSet(int maxsize)
{
  init(maxsize);
  clear();
}

BitSet::BitSet(const BitSet& S)
{
  init(S.max_size());
  copy(S);
}

BitSet::~BitSet()
{
  delete[] buf;
}


void 
BitSet::merge(const BitSet& S)
{
  assert(S.Nr_Cells==Nr_Cells);
  for(int i=0; i<Nr_Cells; i++)
    buf[i]|=S.buf[i];
}


void 
BitSet::intersect(const BitSet& S)
{
  assert(S.Nr_Cells==Nr_Cells);
  for(int i=0; i<Nr_Cells; i++)
    buf[i]&=S.buf[i];
}


bool 
BitSet::equal(const BitSet& S)
{
  assert(S.Nr_Cells==Nr_Cells);
  bool equal=true;  
for(int i=0; i<Nr_Cells; i++){
    if(buf[i]!=S.buf[i])
       {equal=false;}
}
return equal;
}

void
BitSet::copy(const BitSet& S)
{
  assert(S.Nr_Cells==Nr_Cells);
  /*  
  for(int i=0; i<Nr_Cells; i++)
    buf[i]=S.buf[i];
  */
  memcpy(buf, S.buf, Nr_Cells*sizeof(BitCell));
  size = S.size;
}


int 
BitSet::get_hamming_distance_from(const BitSet& S) const
{
  assert(S.Nr_Cells==Nr_Cells);

  int tmp, res=0;
  for(int i=0; i<Nr_Cells; i++)
    res+=cell_card(buf[i]^S.buf[i]);

  return res;
}

ostream& operator<<(ostream& s, const BitSet& S) 
{
  s<<"{";
  for(int i=0; i<S.max_size(); i++) {
    if( S.member(i) )
      s<<i<<" ";
  }
  s<<"}";
  return s;
}


void 
BitSet::dump(void)
{
  char msg[16];
  for(int i=0; i<Nr_Cells; i++) {
    sprintf( msg, "%u", buf[i] );
    cout<<msg<<" ";
  }
}

template<class T> T*
Graph::alloc_nodes(void)
{
  return new T[NumNodes()];
}

template<class T> T*
Graph::alloc_edges(void)
{
  return new T[Nr_edges];
  //return new T[MAX_EDGES];
}



Graph::Graph(int n, int m, int *sizeneigh)
  :Nr_nodes(n), Nr_edges(m)
{
  adj_list=alloc_nodes<int*>();
  deg=alloc_nodes<int>();
  for(int i=0; i<Nr_nodes; i++) {
    adj_list[i]=new int[sizeneigh[i]];
    deg[i]=0;
  }
}

Graph::Graph(const char *fname)
{
  stringstream ss(stringstream::out);
  ifstream is(fname);

  is>>Nr_nodes;
  //cout<<NrNodes<<endl;
  Nr_edges = 0;

  int sizeneigh[Nr_nodes];
  for(int i=0; i<Nr_nodes; ++i) {
    int idx, idx1;
    is>>idx;
    sizeneigh[i] = 0;

    int c;
    for(c=is.get(); c!=':' && isspace(c); c=is.get());
    bool next_line=false;

    while( !is.eof() && !next_line) {
      while( isspace(is.peek()) )
        next_line = (is.get()=='\n');
    
      if( !next_line )
      {
        is>>idx1;
        Nr_edges++;
        sizeneigh[i]++;
      }
    }
  }

  adj_list=alloc_nodes<int*>();
  deg=alloc_nodes<int>();
  for(int i=0; i<Nr_nodes; i++) {
    adj_list[i]=new int[sizeneigh[i]];
    deg[i]=0;
  }
}

Graph::~Graph()
{
  delete[] adj_list;
  delete[] deg;
}

void 
Graph::add_edge(int i, int j, int *sizeneigh) 
{
  int scan;
if(i<=j) fprintf(stderr,"Problem i = %d, j = %d\n",i,j);
  assert(i>j);
  if( !is_edge(i, j) ) {
//fprintf(stderr,"i=%d add j=%d\n",i,j);
/*if(deg[i] >= sizeneigh[i] || deg[j] >= sizeneigh[j])
{
  fprintf(stderr,"deg[%d]=%d, sizeneigh[%d]=%d; deg[%d]=%d, sizeneigh[%d]=%d\n",i,deg[i],i,sizeneigh[i],j,deg[j],j,sizeneigh[j]);
  fscanf(stdin,"%d",&scan);
}*/
    adj_list[i][deg[i]++]=j;
    adj_list[j][deg[j]++]=i;
//cout<<"Now deg["<<i<<"] = "<<deg[i]<<" and deg["<<j<<"] = "<<deg[j]<<endl;
    edge_set.insert(eidx(i,j));
    //edge_set.insert(pair<int, int>(i,j));
    edge_list.push_back(pair<int,int>(i,j));
    Nr_edges++;
  }
}


void 
Graph::del_edge(int i, int j)
{
  assert(i>j);
  if( is_edge(i,j) ) {
    int pos;
    for(pos=0; pos<deg[i] && adj_list[i][pos]!=j; pos++);
    adj_list[i][pos]=adj_list[i][--deg[i]];
    for(pos=0; pos<deg[j] && adj_list[j][pos]!=i; pos++);
    adj_list[j][pos]=adj_list[j][--deg[j]];
    edge_set.erase(eidx(i,j));
    Nr_edges--;
  }
}


int 
Graph::components(const BitSet& DelSet, int *label, int *size) const 
{
  int cnt=0;
  int cur_size;
  Queue<int> S(Nr_nodes); //crea S.buf= int (Nr_nodes) e S.buflen=Nr_nodes//
  
  for(int i=0; i<Nr_nodes; ++i)
    label[i]=-1; /*label usate per indicare le compenenti da 0 a .., ad esempio label[2]=1 
	indica che il nodo 2 fa parte della componente 1*/


  for(int i=0; i<Nr_nodes; ++i) {
    if( !DelSet.member(i) && label[i]<0 ) {// se il nodo i non è nei nodi cancellati e non fa parte di nessuna componente// 
      S.enqueue(i);
      label[i]=cnt;
//fprintf(stderr,"Nodo %d forma componente %d\n",i,cnt);
      cur_size=1;
      while( !S.is_empty() ) {
	int k=S.dequeue();
	for(int j=0; j<degree(k); ++j) {
	  const int v=adj(k)[j];
	  if( !DelSet.member(v) && label[v]<0 ) {
	    label[v]=cnt; 
	    cur_size++;
	    S.enqueue(v);
	  } /* if */
	} /* for j */
      } /* while */

      if( size )
	size[cnt]=cur_size;
//cout<<"from components : size["<<cnt<<"] = "<<size[cnt]<<endl;

      cnt++;

    } /* if !DelSet... */
  } /* for i */

  return cnt; /* Numero delle componenti connesse -1??? */
}


int 
Graph::evaluate_add_node(const BitSet& DelSet, int i, 
			 int *label, int *size)
{
  set<int> S;
  for(int j=0; j<degree(i); ++j) {
    int v=adj(i)[j];
    if( label[v]>=0 ) { // questo test si sostituisce a quello del DelSet.member(v)
      S.insert(label[v]);
//cout<<"add_node : "<<i<<", label["<<v<<"] = "<<label[v]<<", size = "<<size[label[v]]<<endl;
    }
  }

#if __debug_add_one_node__
  cout<<"Node "<<i<<": ";
  cout<<"merging components ";
  for(set<int>::iterator si=S.begin(); si!=S.end(); ++si) {
    cout<<*si<<"=[";
    for(int k=0; k<NumNodes(); ++k) {
      if( label[k]==*si )
	cout<<k<<" ";
    }
    cout<<"] ";
  }
  cout<<" of sizes ";
  for(set<int>::iterator si=S.begin(); si!=S.end(); ++si) {
    cout<<size[*si]<<" ";
  }
  cout<<endl;
#endif

  int new_size=1;//sto inserendo un nodo
  int old_nr_conn=0;
  for(set<int>::iterator si=S.begin(); si!=S.end(); ++si) {
    if( !size[*si] ) {
      cout<<"!!!"<<endl;
      cout<<"i = "<<i<<" adj(i) =";
      for(int j=0; j<degree(i); ++j)
        cout<<" "<<adj(i)[j]<<" ("<<label[adj(i)[j]]<<", "<<size[label[adj(i)[j]]]<<")";
      cout<<endl;
    }
    new_size+=size[*si];//cioè si parte da 1, il nodo che si sta valutando, + la numerosità delle attuali componenti
    old_nr_conn+= size[*si]*(size[*si]-1)/2;// conta il numero di connessioni con le attuali componenti
  }

#if __debug_add_one_node__
  cout<<"new_size="<<new_size<<" old_nr_conn="<<old_nr_conn<<endl;
#endif

  return new_size*(new_size-1)/2-old_nr_conn;//  tra tutti nodi che si vogliono inserire, si prende quello per cui l'aumento di connessioni è minimo..
}


void 
Graph::update_components_after_add_node(BitSet& DelSet, int i, 
					int *label, int *size, int *Nr_comp)
{ /* Fa il merge delle componenti connesse collegate da i */
  set<int> S;
  int new_size=1;
  for(int j=0; j<degree(i); ++j) { // even those present in DelSet ? why do we pass it in argument then ?
    if(!DelSet.member(adj(i)[j]))
    {
      int v=adj(i)[j];
      if( label[v]>=0 && !S.count(label[v]) ) {
        S.insert(label[v]);
        new_size+=size[label[v]];
        size[label[v]] = 0;//le altre componenti diventano di dimensone nulla perchè sono unite da i e dai suoi vicini
      }
    }
  }
  
  //if( S.empty() ) 
  //  cout<<"VUOTA! (deg="<<degree(i)<<")"<<endl;

  int comp_idx;
  if(new_size > 1)//se ho trovato almeno un vicino di i, devo unire le componenti, sennò no
  {
    comp_idx = *(S.begin());
    size[comp_idx] = new_size;//assegna la nuova dimensione alla prima componente in S.
    for(int j=0; j<NumNodes(); ++j) {
      if( label[j]!=comp_idx && S.count(label[j]) ) {
        label[j]=comp_idx;
      }
    }

    label[i] = comp_idx; //assegna la prima componente presa in S al nodo i e a tutti i nodi le cui label appartengono ad S, ovvero sono da unire
  }
  else
    *Nr_comp = components(DelSet, label, size);

}

int Graph::evaluate_node(const BitSet& DelSet, int i) const //passo in argomento l'attuale DelSet e il nodo che si vuoel togliere
{ /* Per togliere un nodo */
  /*static*/ int label[NumNodes()];
  /*static*/ int comp_size[NumNodes()];
  int Nr_comp=components(DelSet, label);
  /* Calcola obiettivo. */
  int Nr_pairs=0;
  for(int k=0; k<Nr_comp; ++k)
    comp_size[k]=0;
  for(int k=0; k<NumNodes(); ++k) {
    comp_size[label[k]]++;
  }

  for(int k=0; k<Nr_comp; ++k) {
    Nr_pairs+=(comp_size[k]*(comp_size[k]-1))/2;
  }

  BitSet S(DelSet);
  S.add(i);//togliamo un nodo
  int Nr_pairs1=0;

  Nr_comp=components(S, label);//calcoliamo le nuove componenti con il nodo i tolto.
  /* Calcola obiettivo. */
  for(int k=0; k<Nr_comp; ++k)
    comp_size[k]=0;
  for(int k=0; k<NumNodes(); ++k) {
    comp_size[label[k]]++;
  }

  for(int k=0; k<Nr_comp; ++k) {
    Nr_pairs1+=(comp_size[k]*(comp_size[k]-1))/2;
  }

  return Nr_pairs-Nr_pairs1;// restituisce le coppie in meno che si avrebbero togliendo il nodo i, quindi si preferisce la differenza max..
  
}

void 
Graph::gen_random_vertex_cover_maxcomp(vector<int>& res, BitSet& S)// restituisce in res i nodi appartenti al vertex cover
{
  
  BitSet Maxcp(NumNodes());
  Maxcp.copy(S);
  int cur;
  vector<int> tmp(NumNodes());
  list<int> N;
  vector<list<int>::iterator> I(NumNodes());
  vector<int> degg(NumNodes());
  BitSet C(NumNodes());
  //BitSet check(NumNodes());

  for(int i=0; i<NumNodes(); ++i) {
    tmp[i]=i;
    degg[i]=degree(i);
  }
  random_shuffle(tmp.begin(), tmp.end());//rimescola i valori nel vettore..

  N.insert(N.end(), tmp.begin(), tmp.end());

  for(list<int>::iterator li=N.begin(); li!=N.end(); ++li) {
    I[*li]=li;
  }

  do {

    cur=-1;
//cout<<"N.size = "<<N.size()<<endl;
    for(list<int>::iterator li=N.begin(); li!=N.end(); ++li) {
      if( degg[*li]==1 && Maxcp.member(*li)) {
	cur=*li;
//cout<<"erase li = "<<*li<<endl;
        //if(check.member(*li)) fprintf(stderr,"he ho, %d dÃ©jÃ  effacÃ© !!\n",*li);
	N.erase(li);
//cout<<"erased"<<", N.size = "<<N.size()<<endl;
        //check.add(*li);
	break;
      }
    }

    if( cur==-1 ) {
      cur=N.front();
      N.pop_front();
    }
    //cout<<"cur==-1 => "<<cur<<endl;

    //cout<<"Qui, cur="<<cur<<endl;
   if(Maxcp.member(cur)){
    for(int j=0; j<degree(cur); ++j) {
      int v=adj(cur)[j];
      if(Maxcp.member(v)){
      //cout<<"v="<<v<<endl;
      if(!C.member(v)) {
	C.add(v);
        //if(check.member(v)) fprintf(stderr,"he ho, %d dÃ©jÃ  effacÃ© !! -> %d\n",v,degg[v]);
	N.erase(I[v]);
        //check.add(v);
	for(int k=0; k<degree(v); ++k) {
	  //cout<<"Node "<<adj(v)[k]<<endl;
if(adj(v)[k] < 0 || adj(v)[k] >= NumNodes())
  cout<<"v = "<<v<<", k = "<<adj(v)[k]<<", adj = "<<adj(v)[k]<<endl;
	  degg[adj(v)[k]]--;
	  
	  //if( !degg[adj(v)[k]] )
	  //  N.erase(I[adj(v)[k]]);
	  
	}
       }
      }
    }
  }
  } while(!N.empty());

  // Versione alternativa
  /*for(int i=0; i<NumNodes(); ++i) {
    if(degg[tmp[i]] > 0)
    {
      C.add(tmp[i]);
      for(int v=0; v<degree(tmp[i]); v++)

      {
        degg[adj(tmp[i])[v]]--;
      }
    }
  }


  for(int i=0; i<NumNodes(); ++i)
  {
    if(!C.member(i) && degg[i] > 0)
      fprintf(stderr,"Node %d with deg=%d left in graph !\n",i,degg[i]);

    if(degg[i] < 0) fprintf(stderr,"degg[%d] = %d < 0 !!\n",i,degg[i]);
    if(!C.member(i))
      for(int j=0; j<degree(i); j++)
        if(adj(i)[j] > i && !C.member(adj(i)[j]))
          fprintf(stdout,"Nodes %d and %d still alive and linked\n",i,adj(i)[j]);

  }*/
//fprintf(stdout,"\n");

  res.clear();
  for(int i=0; i<NumNodes(); ++i) {
    if( C.member(i) ) {
      res.push_back(i);
    }
  }
}


void 
Graph::gen_random_vertex_cover(vector<int>& res)// restituisce in res i nodi appartenti al vertex cover
{
  int cur;
  vector<int> tmp(NumNodes());
  list<int> N;
  vector<list<int>::iterator> I(NumNodes());
  vector<int> degg(NumNodes());
  BitSet C(NumNodes());
  //BitSet check(NumNodes());

  for(int i=0; i<NumNodes(); ++i) {
    tmp[i]=i;
    degg[i]=degree(i);
  }
  random_shuffle(tmp.begin(), tmp.end());//rimescola i nodi nel vettore tmp

  N.insert(N.end(), tmp.begin(), tmp.end()); //mettiamo i nodi shuffled in N

  for(list<int>::iterator li=N.begin(); li!=N.end(); ++li) {
    I[*li]=li; //I contiene gli indirizzi dei N[i] ?
  }

  do {

    cur=-1;
//cout<<"N.size = "<<N.size()<<endl;
    for(list<int>::iterator li=N.begin(); li!=N.end(); ++li) {
      if( degg[*li]==1 ) {
	cur=*li;
//cout<<"erase li = "<<*li<<endl;
        //if(check.member(*li)) fprintf(stderr,"he ho, %d dÃ©jÃ  effacÃ© !!\n",*li);
	N.erase(li); //cancellare il nodo *li di N: evitiamo di tenere nel delset dei nodi di grado 1, meglio cancellare il loro vicino
//cout<<"erased"<<", N.size = "<<N.size()<<endl;
        //check.add(*li);
	break;
      }
    }

    if( cur==-1 ) { //se non c'era un nodo di grado 1, prendiamo il primo nodo in N
      cur=N.front();
      N.pop_front();
    }
    //cout<<"cur==-1 => "<<cur<<endl;

    //cout<<"Qui, cur="<<cur<<endl;

    for(int j=0; j<degree(cur); ++j) { //per tutti i vicini del nodo corrente
      int v=adj(cur)[j];
      //cout<<"v="<<v<<endl;
      if(!C.member(v)) { //se non c'è in C, lo aggiungiamo
	C.add(v);
        //if(check.member(v)) fprintf(stderr,"he ho, %d dÃ©jÃ  effacÃ© !! -> %d\n",v,degg[v]);
	N.erase(I[v]);
        //check.add(v);
	for(int k=0; k<degree(v); ++k) {
	  //cout<<"Node "<<adj(v)[k]<<endl;
if(adj(v)[k] < 0 || adj(v)[k] >= NumNodes())
  cout<<"v = "<<v<<", k = "<<adj(v)[k]<<", adj = "<<adj(v)[k]<<endl;
	  degg[adj(v)[k]]--; //abbassiamo il grado di ogni dei vicini del nodo messo in C (= cancellato dal grafo)
	  
	  //if( !degg[adj(v)[k]] )
	  //  N.erase(I[adj(v)[k]]);
	  
	}
      }
    }

  } while(!N.empty());

  // Versione alternativa
  /*for(int i=0; i<NumNodes(); ++i) {
    if(degg[tmp[i]] > 0)
    {
      C.add(tmp[i]);
      for(int v=0; v<degree(tmp[i]); v++)
      {
        degg[adj(tmp[i])[v]]--;
      }
    }
  }

  for(int i=0; i<NumNodes(); ++i)
  {
    if(!C.member(i) && degg[i] > 0)
      fprintf(stderr,"Node %d with deg=%d left in graph !\n",i,degg[i]);
    if(degg[i] < 0) fprintf(stderr,"degg[%d] = %d < 0 !!\n",i,degg[i]);
    if(!C.member(i))
      for(int j=0; j<degree(i); j++)
        if(adj(i)[j] > i && !C.member(adj(i)[j]))
          fprintf(stdout,"Nodes %d and %d still alive and linked\n",i,adj(i)[j]);
  }*/
//fprintf(stdout,"\n");

  res.clear();
  for(int i=0; i<NumNodes(); ++i) {
    if( C.member(i) ) {
      res.push_back(i);
    }
  }
}



void 
Graph::gen_random_vertex_cover(vector<int>& res, const vector<int>& w)
{
  int cur;
  vector<int> tmp;
  list<int> N;
  vector<list<int>::iterator> I(NumNodes());
  BitSet C(NumNodes());

  for(int i=0; i<NumNodes(); ++i) 
    N.insert(N.end(), i);

  for(list<int>::iterator li=N.begin(); li!=N.end(); ++li) {
    I[*li]=li;
  }

  do {

    tmp.clear();
    for(list<int>::iterator li=N.begin(); li!=N.end(); ++li) {
      if( !tmp.size() || w[*li]==w[tmp[0]] ) {
	tmp.push_back(*li);
      }
      else if( w[*li]>w[tmp[0]] ) {
	tmp.clear();
	tmp.push_back(*li);
      }
    }

    cur=tmp[rand()%tmp.size()];
    N.erase(I[cur]);

    for(int j=0; j<degree(cur); ++j) {
      int v=adj(cur)[j];
      if(!C.member(v)) {
	C.add(v);
	N.erase(I[v]);
      }
    }
  } while(!N.empty());

  res.clear();
  for(int i=0; i<NumNodes(); ++i) {
    if( C.member(i) ) {
      res.push_back(i);
    }
  }
}



void 
Graph::increase_density(int Kmax, int *sizeneigh) 
{
  int tot_cnt=0;
  bool done=true;
  do {
    done=true;
    for(int i=1; i<NumNodes(); ++i) {
      for(int j=0; j<i; ++j) {
        if( is_edge(i,j) ) 
          break;
        int cnt=0;
        for(int k=0; k<degree(i); ++k) 
          if( has_edge(adj(i)[k], j) )
            cnt++;
        if( cnt>Kmax ) {
          add_edge(i,j,sizeneigh);
          done=false;
          tot_cnt++;
        }
      }
    }
  } while(!done);
  printf( "%d edges have been added\n", tot_cnt );
}


void Graph::printgraph_vc(BitSet& S)
{
  FILE *print;
  char str[20];

  print = fopen("graphs/graph_testvc.gml","w");
  fprintf(print,"Creator\t\"yFiles\"\nVersion\t\"2.10\"\ngraph\n[\n\thierarchic\t1\n\tlabel\t\"\"\n\tdirected\t0\n");
  for(int i=0; i<NumNodes(); i++)
    if(!S.member(i))
      fprintf(print,"\tnode\n\t[\n\t\tid\t%d\n\t\tlabel\t\"%d\"\n\t\tgraphics\n\t\t[\n\t\t\tx\t%.1lf\n\t\t\ty\t30.0\n\t\t\tw\t30.0\n\t\t\th\t30.0\n\t\t\ttype\t\"rectangle\"\n\t\t\tfill\t\"#FFCC00\"\n\t\t\toutline\t\"#000000\"\n\t\t]\n\t\tLabelGraphics\n\t\t[\n\t\t\ttext\t\"%d\"\n\t\t\tfontSize\t12\n\t\t\tfontName\t\"Dialog\"\n\t\t\tmodel\t\"null\"\n\t\t]\n\t]\n",i,i,30.0+30*(double)i,i);
  for(int i=0; i<NumNodes(); i++)
    if(!S.member(i))
    {
      for(int j=0; j<degree(i); j++)
        if(!S.member(adj(i)[j]) && adj(i)[j] > i)
          fprintf(print,"\tedge\n\t[\n\t\tsource\t%d\n\t\ttarget\t%d\n\t\tgraphics\n\t\t[\n\t\t\tfill\t\"#000000\"\n\t\t\ttargetArrow\t\"standard\"\n\t\t]\n\t]\n",i,adj(i)[j]);
    }
  fprintf(print,"]\n");
  fclose(print);
}

void 
Graph::read_from_file(const char *fname, int *sizeneigh)
{
  char *buf[128];
  stringstream ss(stringstream::out);

  ifstream is(fname);
  if( !is.is_open() ) {
    ss<<"Cannot open "<<fname;
    throw(ss.str());
  }

  is>>Nr_nodes;
  if( is.eof() ) {
    ss<<"Cannot read Nr_nodes";
    throw(ss.str());
  }

  for(int i=0; i<Nr_nodes; ++i) {
    int idx, idx1;
    is>>idx;
//fprintf(stderr,"idx=%d\n");
    int c;
    for(c=is.get(); c!=':' && isspace(c); c=is.get());
    bool next_line=false;

    while( !is.eof() && !next_line) {
	/* Skip white */

      while( isspace(is.peek()) )
	next_line= (is.get()=='\n');
    
      if( !next_line ) {
	is>>idx1;
        //sizeneigh[i]++;
	//cout<<idx<<" "<<idx1<<endl;
//fprintf(stderr,"idx=%d, idx1=%d\n",idx,idx1);
if(idx>=Nr_nodes || idx1>=Nr_nodes) fprintf(stderr,"idx=%d, idx1=%d\n",idx,idx1);
	if( idx>idx1 ) 
	  add_edge(idx, idx1, sizeneigh);
	else 
	  add_edge(idx1, idx, sizeneigh);
      }
    }
//int stock;
//fscanf(stdin,"%d",&stock);    
  }
}



int 
Graph::evaluate_obj(const BitSet& DelSet) const 
{
  /*static*/ int label[NumNodes()];
  /*static*/ int comp_size[NumNodes()];
  int Nr_comp=components(DelSet, label, comp_size);
//cout<<Nr_comp<<endl;
//for(int i=0; i<Nr_comp; i++)
//  fprintf(stderr,"%d ",comp_size[i]);
//fprintf(stderr,"\n");
  /* Calcola obiettivo. */
  int Nr_pairs=0;

  /*
  for(int k=0; k<Nr_comp; ++k)
    comp_size[k]=0;
  for(int k=0; k<NumNodes(); ++k) {
    comp_size[label[k]]++;
  }
  */

#if __debug_evaluate_obj__
  vector<list<int>> tmp(Nr_comp);
  for(int i=0; i<NumNodes(); ++i) {
    if( label[i]!=-1 )
      tmp[label[i]].push_front(i);
  }
  cout<<"Components: ";
  for(int k=0; k<tmp.size(); k++) {
    cout<<"[";
    copy(tmp[k].begin(), tmp[k].end(), ostream_iterator<int>(cout, " "));
    cout<<"] ";
  }
  cout<<endl;
#endif

  for(int k=0; k<Nr_comp; ++k) {
//cout<<"comp_size["<<k<<"]"<<endl;
    Nr_pairs+=(comp_size[k]*(comp_size[k]-1))/2;
//cout<<"add : "<<(comp_size[k]*(comp_size[k]-1))/2<<" -> "<<Nr_pairs<<endl;
  }

  return Nr_pairs;
}

// Funzione per aggiornare le componenti connesse dopo aver tolto il nodo "node"
int 
Graph::components_single(const BitSet& DelSet, int *label, int *size, int node, int num_comp) const 
{
  int cnt=label[node], old_label=label[node];
  int cur_size;
  Queue<int> S(Nr_nodes);
  
  for(int i=0; i<Nr_nodes; ++i)
    if(label[i] == cnt)
      label[i]=-1;


  for(int i=0; i<Nr_nodes; ++i) {
    if( !DelSet.member(i) && label[i]<0 ) {
      S.enqueue(i);
      label[i]=cnt;
      cur_size=1;
      while( !S.is_empty() ) {
	int k=S.dequeue();
	for(int j=0; j<degree(k); ++j) {
	  const int v=adj(k)[j];
	  if( !DelSet.member(v) && label[v]<0 ) {
	    label[v]=cnt;
	    cur_size++;
	    S.enqueue(v);
	  } /* if */
	} /* for j */
      } /* while */

      if( size )
	size[cnt]=cur_size;
//cout<<"from components : size["<<cnt<<"] = "<<size[cnt]<<endl;

      if(cnt == old_label)
        cnt = num_comp;
      else
        cnt++;

    } /* if !DelSet... */
  } /* for i */

  return cnt; /* Numero delle componenti connesse. */
}



double divide(int a, int b)
{
  if(b > 0) return a/(double)b;
  else return 0.0;
}



// Function that evaluates the three following objectives: C[0] = pairwise connectivity, C[1] = maximum component size, C[2] = number of connected components
void Graph::eval_obj_general(int C[], const BitSet& DelSet)
{
  int label[NumNodes()];
  int comp_size[NumNodes()];
  int Nr_comp=components(DelSet, label, comp_size);
  /* Calcola obiettivo. */

  C[1] = 0;
  for(int k=0; k<Nr_comp; ++k)
    C[1] += comp_size[k]*(comp_size[k]-1)/2;

  C[2] = 0;
  for(int k=0; k<Nr_comp; ++k)
    if(comp_size[k] > C[2])
      C[2] = comp_size[k];

  C[3] = Nr_comp;
}

// Functino that updates the components of the graph after moving a node from the deleted set to the graph
int Graph::update_components_after_add_node_general(BitSet& DelSet, int i, int *label, int *size, int *Nr_comp)
{ /* Fa il merge delle componenti connesse collegate da i */
  set<int> S;
  int new_size=1;
  for(int j=0; j<degree(i); ++j) {
    int v=adj(i)[j];
    if( label[v]>=0 && !S.count(label[v]) ) {
      S.insert(label[v]);
      new_size+=size[label[v]];
      size[label[v]] = 0;
      //*Nr_comp--;
    }
  }
  
  int comp_idx;
  if(new_size > 1)
  {
    comp_idx = *(S.begin());
    size[comp_idx] = new_size;
    for(int j=0; j<NumNodes(); ++j) {
      if( label[j]!=comp_idx && S.count(label[j]) ) {
        label[j]=comp_idx;
      }
    }

    label[i]=comp_idx;
  }
  else
    *Nr_comp = components(DelSet, label, size);

  int max_size=0;
  for(int n=0; n<*Nr_comp; n++)
    if(max_size < size[n])
      max_size = size[n];

  return max_size;
}

// Function that updates the components after deleting a node from the graph
void Graph::update_components_after_del_node_general(int C[4], int i, int *label, int *size, int *Nr_comp)
{ /* Fa il merge delle componenti connesse collegate da i */
  int S[NumNodes()], curs, new_size, comp=-1, visit[NumNodes()];
  for(int j=0; j<NumNodes(); ++j)
    visit[j] = 0;
  visit[i] = 1;
  for(int j=0; j<degree(i); ++j) {
    int v=adj(i)[j];
//fprintf(stderr,"Visiting v = %d from i = %d (visit = %d, label[v] = %d) with label[i] = %d\n", v, i, visit[v], label[v], label[i]);
    if( label[v]>=0 && visit[v]==0 ) {
      if(comp == -1)
      {
        comp = label[i];
//fprintf(stderr,"comp = %d\n", label[i]);
      }
      else if(comp == label[i])
        comp = *Nr_comp;
      else
        comp++;

      visit[v] = 1;
      curs = 0;
      S[curs++] = v;
      label[v] = comp;
      new_size = 1;
      while(curs > 0)
      {
        v = S[--curs];
        for(int k=0; k<degree(v); k++)
        {
          int w=adj(v)[k];
          if(label[w] >= 0 && visit[w] == 0)
          {
            visit[w] = 1;
            S[curs++] = w;
//fprintf(stderr,"Adding w = %d from v = %d of comp = %d to S (curs = %d)\n", w, v, comp, curs);
            label[w] = comp;
            new_size++;
          }
        }
      }
      size[comp] = new_size;
//fprintf(stderr,"Found a component of size = %d at comp = %d, curs = %d\n", new_size, comp, curs);
//int scan;
//fscanf(stdin,"%d",&scan);
    }
  }
  if(comp >= *Nr_comp)
    *Nr_comp = comp+1;

  if(comp == -1)
  {
    for(int j=0; j<NumNodes(); j++)
      if(label[j] > label[i])
        label[j]--;
    for(int c=label[i]; c<*Nr_comp-1; c++)
      size[c] = size[c+1];
    (*Nr_comp)--;
  }
  label[i] = -1;

//fprintf(stderr,"Now Nr_comp = %d\n", *Nr_comp);
  C[3] = *Nr_comp;
  C[1] = 0;
  C[2] = 0;
  for(int c=0; c<*Nr_comp; c++)
  {
    if(size[c] > C[2])
      C[2] = size[c];
    C[1] += (size[c]*(size[c]-1))/2;
  }
}

// Checks if the solution is still feasible:
int condition_add(char x, int K, int num, int flag)
{
  if(x == 'a')
  {
    if(num > K)
      return 0;
    else
      return 1;
  }
  else if(x == 'b')
    return flag;
}

// Conditions to update the list of potential candidates for node addition, C represents the connectivity, Cconst the value under/over which we want to maitain connectivity and tmp the value to add to the connectivity or the new connectivity itself linked to the addition of node i in the graph
void update_list_add(int n, char x, int Cconst, int C, int *bestC, vector<int> tmp, vector<int>& cand, int i, int *cursiz)
{
  if(n == 1)
  {
    if(x == 'a')
    {
      if( cand.size() > 0 && tmp[0] < *bestC ) {
        cand.clear();
	cand.push_back(i);
	*bestC = tmp[0];
      }
      else if( !cand.size() || (cand.size() > 0 && tmp[0] == *bestC) ) {//nel caso del primo nodo e di uguaglianze..
        cand.push_back(i);
	*bestC = tmp[0];
      }
    }
    if(x == 'b')
    {
      if((C + tmp[0]) <= Cconst && tmp[0] < *bestC)
      {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[0];
      }
      else if((C + tmp[0]) <= Cconst && *bestC == tmp[0] ) {
        cand.push_back(i);
      }
    }
  }
  else if(n == 2)
  {
    if(x == 'a')
    {
      if(tmp[1] < *bestC)
      {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[1];
      }
      else if(*bestC == tmp[1]) {
        cand.push_back(i);
      }
    }
    if(x == 'b')
    {
      if(tmp[1] <= Cconst && tmp[1] < *bestC)
      {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[1];
      }
      else if(tmp[1] <= Cconst && *bestC == tmp[1]) {
        cand.push_back(i);
      }
    }
  }
  else if(n == 3)
  {
    if(x == 'a')
    {
//fprintf(stderr,"bestC = %d, tmp[2] = %d, cursiz = %d, tmp[1] = %d\n", *bestC, tmp[2], *cursiz, tmp[1]);
      // we want a minimum decrease of the number of components
      if(tmp[2] > *bestC)
      {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[2];
	*cursiz = tmp[1];
      }
      else if( tmp[2] == *bestC && tmp[1]  < *cursiz ) {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[2];
        *cursiz = tmp[1];
      }
      else if( tmp[2] == *bestC && tmp[1] == *cursiz ) {
        cand.push_back(i);
      }
    }
    if(x == 'b')
    {
      // we want a minimum decrease of the number of components
      if((C + tmp[2] >= Cconst) && tmp[2] > *bestC)
      {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[2];
        *cursiz = tmp[1];
      }
      else if( (C + tmp[2] >= Cconst) && tmp[2] == *bestC && tmp[1] < *cursiz ) {
        cand.clear();
        cand.push_back(i);
        *bestC = tmp[2];
        *cursiz = tmp[1];
      }
      else if( (C + tmp[2] >= Cconst) &&  tmp[2] == *bestC && tmp[1] == *cursiz ) {
        cand.push_back(i);
      }
    }
  }
//int scan;
//fscanf(stdin,"%d",&scan);
}

// Greedy algorithm proceeding from a vertex cover of the graph and adding nodes back to the graph until the solution is feasible (type x='a' of CNP) or until the solution is no more feasible (type x='b' of CNP). K is either the budget or the connectivity constraint value
int Graph::heur_add_general(int K, vector<int>& res, int n, char x, int partial, BitSet *S)// si mette alla fine in res i nodi cancellati
{
  /*static*/ int label[NumNodes()];
  /*static*/ int size[NumNodes()];
  vector<int> cand;
  int flag=0, C, bestC, largest_comp=0;

  int num;
  if(partial == 0)
  {
    S->clear();
    vector<int> VC;
    gen_random_vertex_cover(VC);
    //VC contiene i nodi del vertex cover
    for(int i=0; i<VC.size(); ++i) {
      S->add(VC[i]);//metto in S i nodi appartenenti al vertex cover
      //num=VC.size();
    }
  }
  num = S->size;

  int Nr_comps=components(*S, label, size);//restituisce il numero di componenti con S corrente, assegnado allo stesso tempo le label..
  if(n == 1)
  {
    C = 0;
    for(int c=0; c<Nr_comps; c++)
      C += size[c]*(size[c]-1)/2;
  }
  if(n == 2)
  {
    C = 1;
    for(int c=0; c<Nr_comps; c++)
      if(size[c] > C)
        C = size[c];
  }
  if(n == 3)
    C = Nr_comps;

//fprintf(stderr,"num = %d, C = %d -> condition = %d\n", num, C, condition_add(x, K, num, flag));

  while(condition_add(x, K, num, flag) == 0) //finche non riempiamo le condizioni necessarie per smettere di ridurre |S|
  {
    int cursiz=0;
    cand.clear();//serve per valutare i nodi candidati ad essere aggiunti
    if(n == 1)
      bestC = NumNodes()*(NumNodes()-1)/2+1;
    if(n == 2)
      bestC = NumNodes()+1;
    if(n == 3)
      bestC = -Nr_comps-1;
   
    for(int i=0; i<NumNodes(); ++i) {
      if( S->member(i) ) {
	vector<int> tmp = evaluate_add_node_general(n, *S, i, label, size);//cerchiamo il valore minimo
        update_list_add(n, x, K, C, &bestC, tmp, cand, i, &cursiz);
      }
    } /* for i */
//fprintf(stderr,"cand.size = %d\n", cand.size());

    if(cand.size() > 0)
    {
      int pos=rand()%cand.size(); //nel caso di più candidati: sceglie a caso..
      int cur=cand[pos];
      S->del(cur); //toglie da S, ovvero aggiunge cur al grafo finale
      num--; //un nodo è stato aggiunto
      //fprintf(stderr,"num = %d\n", num);

      if(n == 1 || n == 3)
        C += bestC; // variazione funzione obiettivo
      if(n == 2 && bestC > C)
        C = bestC; // -> senza il test fra bestC e obj, darebbe la dimensione della componente creata dall'introduzione del nodo cur, che non è per forza la più grande !

      update_components_after_add_node_general(*S, cur, label, size, &Nr_comps); //riaggiorna le componenti
//fprintf(stderr," now C = %d\n", C);
    }
    else
    {
//fprintf(stderr,"Can't find a node\n");
      flag = 1;
    }

  } /* while */

  res.clear();
  for(int i=0; i<NumNodes(); ++i) {//mette i nodi appartenenti a S, cioè nodi da cancellare, nel vettore res..
    if( S->member(i) ) {
      res.push_back(i);
    }
  }
if(S->size != num)
{
  fprintf(stderr,"S->size = %d, num = %d\n", S->size, num);
  exit(1);
}
  int obj;
  if(x == 'a')
    obj = C;
  if(x == 'b')
  {
    if(n <= 2 || C >= K)
      obj = num;
    else
    {
//fprintf(stderr,"Couldn't get that solution right\n");
      obj = NumNodes();
    }
  }

  return obj;
}

// Evaluate the impact of moving a node from the deleted set back into the graph:
vector<int> Graph::evaluate_add_node_general(int n, const BitSet& DelSet, int i, int *label, int *size)
{
  // S contains the labels of all components to be merged, old_nr_pair the pairwise connectivity in the graph without i, newsize the size of the merged components, checj the number of components that have been lost by reintroducing node i: the difference in pairwise connectivity, the size of the new component and the number of lost components are loaded inside vector fo:
  set<int> S;
  vector<int> fo;
  int old_nr_pair=0;
  int new_size=1;
  int checj=1;

  for(int j=0; j<degree(i); ++j) {
    int v=adj(i)[j];
    if( label[v]>=0 && !S.count(label[v]) ) { // questo test si sostituisce a quello del DelSet.member(v)
      S.insert(label[v]);
      old_nr_pair += size[label[v]]*(size[label[v]]-1)/2;// conta il numero di connessioni con le attuali componenti
      new_size+=size[label[v]];
      checj--;
    }
  }

  fo.push_back(new_size*(new_size-1)/2-old_nr_pair);
  fo.push_back(new_size);
  fo.push_back(checj);

  return fo;
}

// Function to select the best node for deletion (to be used in heur_del)
void select_best_del(int n, char x, int *bestnode, int *numnode, int *impact, int fragment[][2], int *deg, int v, int version)
{

  if(n == 1)
  {
    if(*numnode == -1 || (impact[bestnode[*numnode]] < impact[v] || (impact[bestnode[*numnode]] == impact[v] && deg[bestnode[*numnode]] <= 1 && deg[v] > 1)))
    {
      while(*numnode >= 1)
        bestnode[(*numnode)--] = 0;
      *numnode = 0;
      bestnode[*numnode] = v;
    }
    else if(impact[bestnode[*numnode]] == impact[v] && (deg[v] > 1 || (deg[v] <= 1 && deg[bestnode[*numnode]] <= 1)))
    {
      bestnode[++(*numnode)] = v;
    }
  }
  if(n == 2)
  {
    if(*numnode == -1 || (x == 'a' && impact[bestnode[*numnode]] < impact[v]) || (x == 'b' && ((version == 1 && (impact[bestnode[*numnode]] < impact[v] || (impact[bestnode[*numnode]] == impact[v] && fragment[bestnode[*numnode]][0] < fragment[v][0]))) || (version == 2 && (fragment[bestnode[*numnode]][0] < fragment[v][0] || (fragment[bestnode[*numnode]][0] == fragment[v][0] && impact[bestnode[*numnode]] < impact[v]))))))
    {
      while(*numnode >= 1)
        bestnode[(*numnode)--] = 0;
      *numnode = 0;
      bestnode[*numnode] = v;
    }
    else if(impact[bestnode[*numnode]] == impact[v])
    {
      bestnode[++(*numnode)] = v;
    }
  }
  if(n == 3)
  {
    if(deg[v] > 1 && (*numnode == -1 || impact[bestnode[*numnode]] < impact[v]))
    {
      while(*numnode >= 1)
        bestnode[(*numnode)--] = 0;
      *numnode = 0;
      bestnode[*numnode] = v;
    }
    else if(deg[v] > 1 && impact[bestnode[*numnode]] == impact[v])
    {
      bestnode[++(*numnode)] = v;
    }
  }
}

// Check if a solution is feasible
int feas(int n, char x, int K, int k, int C)
{
  if(x == 'a')
  {
    if(k != K)
      return 0;
    else
      return 1;
  }
  if(x == 'b')
  {
    if(n <= 2)
    {
      if(C > K)
        return 0;
      else
        return 1;
    }
    if(n == 3)
    {
      if(C < K)
        return 0;
      else
        return 1;
    }
  }
}

// Greedy algorithm which proceeds by deleting nodes from the graph until we can do it no longer (become infeasible or suboptimal)
int Graph::heur_del_avoid_general(int n, char x, int K, vector<int>& resplus, vector<int>& resminus, BitSet& DelSet, int version, BitSet& nodes_avoid)
{
  int DFN[NumNodes()], LOW[NumNodes()], VISITED[NumNodes()], COUNTED[NumNodes()], PARENT[NumNodes()], SIZE[NumNodes()], Nbchild[NumNodes()], num, S[NumNodes()], comp[NumNodes()], bestnode[NumNodes()], impact[NumNodes()], articulation[NumNodes()], pile, v, y, i, w, root=1, nbcomp=0, numcomp, obj, k=DelSet.size, subtreesize[NumNodes()], fragment[NumNodes()][2];
  /*static*/ int label[NumNodes()];
  /*static*/ int comp_size[NumNodes()];
  BitSet B(DelSet);
  int deg[NumNodes()];
  //int comp_AP, numcompold;
  int numnode=0, largestcomp=0, largestsize=0, C=0, pass=0, checkpossible=1;

//cout<<"NumNodes = "<NumNodes

  numcomp = components(B, label, comp_size);
  if(n == 3)
    C = numcomp;
  for(int c=0; c<numcomp; c++)
    if(comp_size[c] > largestsize)
    {
      largestsize = comp_size[c];
      largestcomp = c;
    }
  if(n == 2)
    C = largestsize;
  if(n == 1)
    for(int c=0; c<numcomp; c++)
      C += comp_size[c]*(comp_size[c]-1)/2;
//fprintf(stderr,"start at C = %d, largestcomp = %d, largestsize = %d\n", C, largestcomp, largestsize);
  int inter=0;
  for(i=0; i<NumNodes(); i++)
    if(label[i] == largestcomp && nodes_avoid.member(i))
      inter++;
  if(largestsize == inter)
    checkpossible = 0;

  // While we do not fulfill the conditions to stop node deletion, we delete one more node
  while(feas(n, x, K, k, C) == 0 && numnode != -1)
  {
//fprintf(stderr,"k = %d\n",k);
    // Compute the effective degree of each node in the remaining graph since it is useless to cancel nodes of degree 1:
    for(i = 0; i < NumNodes(); i++)
      if(!B.member(i))
      {
        deg[i] = 0;
        for(y = 0; y < degree(i); y++)
          if(!B.member(adj(i)[y]))
            deg[i]++;
      }
    for(i = 0; i < NumNodes(); i++)
    {
      S[i] = 0;
      DFN[i] = 0;
      LOW[i] = 0;
      VISITED[i] = 0;
      COUNTED[i] = 0;
      PARENT[i] = 0;
      SIZE[i] = 0;
      impact[i] = 0;
      subtreesize[i] = 0;
      fragment[i][0] = 0;
      fragment[i][1] = -1;
      Nbchild[i] = 0;
      comp[i] = 0;
      articulation[i] = 0;
      bestnode[i] = 0;
      numnode = -1;
    }

    for(i = 0; i < NumNodes(); i++)
      if(VISITED[i] != 1 && !B.member(i) && (n != 2 || (x == 'a' && label[i] == largestcomp) || (x == 'b' && comp_size[label[i]] > K)))
      {
//cout<<"New component"<<endl;
//fprintf(stderr,"Starting component with node %d\n",i);
        num = 1;
        comp[num] = i;
        pile = 1;
        root = i;
        S[pile] = root;
        VISITED[root] = 1;
        DFN[root] = num;
        LOW[root] = DFN[root];
        
        SIZE[root] = 1;
        impact[root] = 0;

        while(pile > 0)
        {
          v = S[pile];
//cout<<"v = "<<v<<endl;
          for(y = 0; y < degree(v) && (VISITED[adj(v)[y]] == 1 || B.member(adj(v)[y])); y++);
          if(y < degree(v))
          {
            VISITED[adj(v)[y]] = 1;
            comp[++num] = adj(v)[y];
            S[++pile] = adj(v)[y];
            DFN[adj(v)[y]] = num;
            LOW[adj(v)[y]] = DFN[adj(v)[y]];
            PARENT[adj(v)[y]] = v;
            Nbchild[v]++;
            SIZE[adj(v)[y]] = 1;
            impact[adj(v)[y]] = 0;
//cout<<"Visit "<<adj(v)[y]<< "("<<DFN[adj(v)[y]]<<") "<<", PARENT = "<<v<<endl;
          }
          else
          {
            S[pile--] = 0;
            for(w = 0; w < degree(v); w++)
              if(!B.member(adj(v)[w]))
              {
                if(DFN[adj(v)[w]] < DFN[v])
                {
                  LOW[v] = min(LOW[v], DFN[adj(v)[w]]);
//cout<<"DFN: New LOW["<<v<<"] = "<<LOW[v]<<" from "<<adj(v)[w]<<endl;
                }
                else
                {
                  LOW[v] = min(LOW[v], LOW[adj(v)[w]]);
//cout<<"LOW: New LOW["<<v<<"] = "<<LOW[v]<<" from "<<adj(v)[w]<<endl;
                  if(COUNTED[adj(v)[w]] == 0 && (PARENT[v] != adj(v)[w] || v == root))
                  {
                    COUNTED[adj(v)[w]] = 1;
                    SIZE[v] += SIZE[adj(v)[w]];
                    //cout<<adj(v)[w]<<": "<<SIZE[adj(v)[w]]<<" ";
//cout<<adj(v)[w]<<" COUNTED, SIZE["<<v<<"] = "<<SIZE[v]<<endl;
                  }
                  
                  if(LOW[adj(v)[w]] >= DFN[v] && v != root && PARENT[adj(v)[w]] == v)
                  {
                    articulation[v] = 1;
                    subtreesize[v] += SIZE[adj(v)[w]];
                    if(n == 1)
                      impact[v] += SIZE[adj(v)[w]]*(SIZE[adj(v)[w]]-1)/2;
                    if(n == 2)
                    {
                      // For articulation points, save the value of the largest connected sub component one gains from deleting the AP and the number of subcomponents that are smaller than L
                      if(fragment[v][1] < SIZE[adj(v)[w]])
                        fragment[v][1] = SIZE[adj(v)[w]];
                      if(x == 'b' || SIZE[adj(v)[w]] <= K)
                        fragment[v][0]++;
                    }
                    if(n == 3)
                      fragment[v][0]++;
                  // cout<<v<<" is an articulation point from "<<adj(v)[w]<<", SIZE = "<<SIZE[adj(v)[w]]<<", impact = "<<impact[v]<<"subtree "<<subtreesize[v]<<endl;
		  }
                }
                if(v == root && Nbchild[v] > 1  && PARENT[adj(v)[w]] == v)
                {
                  articulation[v] = 1;
                  subtreesize[v] += SIZE[adj(v)[w]];
                  if(n == 1)
                    impact[v] += SIZE[adj(v)[w]]*(SIZE[adj(v)[w]]-1)/2;
                  if(n == 2)
                  {
                    // For articulation points, save the value of the largest connected sub component one gains from deleting the AP and the number of subcomponents that are smaller than L
                    if(fragment[v][1] < SIZE[adj(v)[w]])
                      fragment[v][1] = SIZE[adj(v)[w]];
                    if(x == 'b' || SIZE[adj(v)[w]] <= K)
                      fragment[v][0]++;
                  }
                  if(n == 3)
                    fragment[v][0]++;
                  //cout<<v<<" is an articulation (root) point from "<<adj(v)[w]<<", SIZE = "<<SIZE[adj(v)[w]]<<", impact = "<<impact[v]<<endl;
                }

              }
          }
        }
        
        for(v = 1; v <= num; v++)
          if(!B.member(comp[v]) && (checkpossible == 0 || !nodes_avoid.member(comp[v])))
          {
            //cout<<"impact["<<comp[v]<<"] = "<<impact[comp[v]]<<", AP = "<<articulation[comp[v]]<<endl;
            if(articulation[comp[v]] == 1 ) {//&& check[comp[v]]==1
              if(n == 1)
                impact[comp[v]] += (num-subtreesize[comp[v]]-1)*(num-subtreesize[comp[v]]-2)/2;
              if(n == 2)
              {
                if(x == 'b' && num-subtreesize[comp[v]]-1 <= K && comp[v] != root)
                  fragment[comp[v]][0]++;
                if(num-subtreesize[comp[v]]-1 > fragment[comp[v]][1])
                  fragment[comp[v]][1] = num-subtreesize[comp[v]]-1;
                impact[comp[v]] = num-fragment[comp[v]][1];
              }
              if(n == 3)
              {
                // If the node is the root, the subcomponents we have encountered are all the subcomponents created, however if it is not, the subcomponent we descended from has to be taken into account in the number of components created (-1 since we take into account the variation in the number of components)
                if(comp[v] == root)
                  impact[comp[v]] = fragment[comp[v]][0]-1;
                else
                  impact[comp[v]] = fragment[comp[v]][0];
              }
            }
            else
            {
              if(n == 1)
                impact[comp[v]] += (num-2)*(num-1)/2;
              if(n == 2)
                impact[comp[v]] = 1;
              // If the node has an effective degree of 1, it means we're actually making a component disappear, otherwise since the node is not an AP, there is no impact on the objective
              if(n == 3)
              {
                if(deg[comp[v]] == 1)
                  impact[comp[v]] = -1;
                else
                  impact[comp[v]] = 0;
              }
            }

            if(n == 1)
              impact[comp[v]] = num*(num-1)/2 - impact[comp[v]];

            //BitSet V(NumNodes());
            //V.add(comp[v]);
//fprintf(stderr,"impact[%d] = %d (%d, %d), ", comp[v], impact[comp[v]], label[comp[v]], comp_size[label[comp[v]]]);
            select_best_del(n, x, bestnode, &numnode, impact, fragment, deg, comp[v], version);
          }

      }
//fprintf(stderr,"\n");
    if(numnode > -1)
    {
      i = bestnode[rand()%(numnode+1)];
//fprintf(stderr,"Choose node i = %d\n", i);
//fprintf(stderr,"Choosing node with impact %d\n", impact[i]);
      for(y = 0; y < degree(i); y++)
        if(!B.member(adj(i)[y]))
          deg[adj(i)[y]]--;
      B.add(i);
      if(n == 2)
      {
        numcomp = components(B, label, comp_size);
        largestsize = 0;
        for(int c = 0; c < numcomp; c++)
          if(comp_size[c] > largestsize)
          {
            largestsize = comp_size[c];
            largestcomp = c;
          }
//fprintf(stderr,"Now largestcomp = %d with size = %d (B.size = %d)\n", largestcomp, largestsize, B.size);
      }

      if(n == 1)
        C -= impact[i];
      if(n == 3)
        C += impact[i];
      if(n == 2)
        C = largestsize;
      k++;
    }
    //else if(n != 3 || x != 'b' || (n == 2 && x == 'b' && largestsize > K))
    //{
      //fprintf(stderr,"numnode = %d, K = %d, largestsize = %d, largestcomp = %d, numcomp = %d (k = %d)\n", numnode, K, largestsize, largestcomp, numcomp, k);
      //for(i=0; i<NumNodes(); i++)
      //  if(comp_size[label[i]] > K)
      //    fprintf(stderr,"%d had belonged to a comp_size = %d, label = %d: test = %d/%d/%d -> VIS = %d, !B = %d | impact = %d, nodes_avoid.member = %d\n", i, comp_size[label[i]], label[i], VISITED[i] != 1, !B.member(i), (n != 2 || (x == 'a' && label[i] == largestcomp) || (x == 'b' && comp_size[label[i]] > K)), VISITED[i], !B.member(i), impact[i], nodes_avoid.member(i));
      //exit(1);
    //}
  }
//fprintf(stderr,"\n");
//fprintf(stderr,"B.size up to %d\n", B.size);
  // For b types of CNP, try to reduce the cardinality of the deleted nodes by applying the Pardalos-type of greedy
  if(x == 'b')
  {
//fprintf(stderr,"doing Pardalos for b\n");
    obj = heur_add_general(K, resplus, n, x, 1, &B);
//fprintf(stderr,"Output of heur_add: B.size = %d, obj = %d\n", B.size, obj);
  }
  else
  {
    obj = C;
    //int verifC[4];
    //eval_obj_general(verifC, B);
    //if(obj != verifC[n])
    //{
    //  fprintf(stderr,"Inside Ven: obj = %d while verifC = %d\n", obj, verifC[n]);
    //  exit(1);
    //}
  }

  resplus.clear();
  for(int i=0; i<NumNodes(); ++i)
  {
    if( B.member(i) && !DelSet.member(i) )
      resplus.push_back(i);
    if( !B.member(i) && DelSet.member(i) )
      resminus.push_back(i);
  }
//fprintf(stderr,"res.size = %d, B.size = %d, obj = %d\n", res.size(), B.size, obj);

  return(obj);
}



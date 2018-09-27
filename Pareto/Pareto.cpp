//
// Created by Sdaxddx
//
#include "Pareto.h"
#include "ProblemData.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <bitset>
#include <list>

ParetoFront::ParetoFront(int dim, int costMAX) {
	this->dim = dim;
	this->costMAX = costMAX;
	this->costMIN = 0;
	this->PWC = new int[dim];	//LB or UB a seconda delle occasoni
	this->prec = new int[dim];
	this->succ = new int[dim];
	this->time = new int[dim];
	for (int i = 0; i < dim; ++i) {
		this->PWC[i] = PAIRWISEMAX; // LB[i] == PWC == pairwise connectivity?
		this->prec[i] = costMAX + 1;
		this->succ[i] = costMAX + 1;
		this->time[i] = INFINITE;
	}
}
;

ParetoFront::~ParetoFront() {
	delete[] PWC;
	delete[] prec;
	delete[] succ;
	delete[] time;
}

/**
 * Print the cost, PWC and time of each solution of the ParetoFront
 */
void ParetoFront::PrintPareto() {
	for (int i = 0; i < dim; i++) {
		printf("The pairwise with cost %d is %d, calculated in %d time \n", i,
				this->PWC, this->time);
	};
}

/**
 * Save the cost, PWC and time of each solution of the ParetoFront in the File f
 */
void ParetoFront::SavePareto(FILE *f) {
	for (int i = 0; i < dim; i++) {
		fprintf(f, "The pairwise with cost %d is %d, calculated in %d time \n",
				i, this->PWC, this->time);
	};
}

/**
 * check if the solution passed is dominated by one point of the ParetoFront
 * @return true if point is dominated, false otherwise
 */
bool ParetoFront::DominatedPoint(int PWC, int cost) {
	if (cost > costMAX)
		return true; //if the solution passed has cost greater tha the max return true
	if (fabs(PWC - this->PWC[cost]) > EPSILON) //se le soluzioni non sono uguali  //useless?
		return (PWC > this->PWC[cost]); //ritorna vero se la soluzione passata è maggiore di quella attuale, falso se il contrario
	return true;
}

void ParetoFront::UpdatePareto(int PWC, int cost, int time) {
	if (ParetoFront::DominatedPoint(PWC, cost)) {
		printf("The point with cost %d and pairwise %d is dominated", cost,
				PWC);
		return;
	}

	while ((cost != this->costMAX) && (PWC < this->PWC[cost])) {
		this->PWC[cost] = PWC;
		cost = this->succ[cost];
	}
}

BandBnode::BandBnode(int id, int livello, int size) {
	this->id = id;
	this->livello = livello;
//	this->LB = {0,0};
//	this->succ = nullptr;
//	this->prec = nullptr;
//	this->type = 0;
//	this->var = 0;
//	this->id_figlio = 0;
	saved_nodes = std::bitset<500>{};
	deleted_nodes = std::bitset<500>();
}

BandBnode::BandBnode(int id, int livello, std::bitset<500> saved, std::bitset<500> deleted) {	//create a new node with the bitset passed
	this->id = id;
	this->livello = livello;
//	LB = {0,0};
//	this->succ = nullptr;
//	this->prec = nullptr;
//	this->type = 0;
//	this->var = 0;
//	this->id_figlio = 0;
	saved_nodes = saved;
	deleted_nodes = deleted;
}

BandBnode::~BandBnode() {
//	delete this->prec;
//	delete this->succ;
	delete[] LB;
	delete[] UB;
	delete saved_nodes;
	delete deleted_nodes;
}

/**
 * check if the current Branch and bound node is dominated in the given Pareto Front
 */
bool BandBnode::useful(ParetoFront* PF) {
	if(!PF->DominatedPoint(this->LB[0], this->LB[1]))
		return true;
	return false;
}


/**
 * Given a List return its first element
 * @return the first element of the list
 */
//BandBnode *firstlist(BandBnode *List) { //List è un nodo di chiusura, che viene prima del primo elemento della lista e dopo l'ultimo elemento
//	BandBnode *first = List->succ;
//	return first;
//}

/**
 * Given a List return its last element
 * @return the last element of the list
// */
//BandBnode *lastlist(BandBnode *List) {
//	BandBnode *last = List->prec;
//	return last;
//}

/**
 * Given a element and its list check if the node is the last element of the list
// */
//bool endlist(BandBnode *p, BandBnode *List) { //se p è l'ultimo elemento della lista allora sarà uguale a List
//	return p == List;
//}

/**
 * Given a element of a list return the next element of the List
// */
//BandBnode *nextlist(BandBnode *p) {  // ritorna l'elemento successivo a p
//	return p->succ;
//}
//
//void insert_before(BandBnode *node, BandBnode *p) {  //inserisce node prima di p
//	p->prec->succ = node;
//	node->prec = p->prec;
//	node->succ = p;
//	p->prec = node;
//}

/**
 * Given a List check if it is empty
// */
//bool is_emptylist(BandBnode *List) { //controlla se la lista è vuota, controllando che ci sia solo l'elemento List
//	if ((List->prec = List) && (List->succ = List))
//		return true;
//	else
//		return false;
//}

/**
 * Given a node and a List add the node in the list based on the visit strategy
 */
//void insert_BnBnode(BandBnode *node, BandBnode *List, int VisitStrategy) { //inserisce un nodo nella lista in base al tipo di visita
//	BandBnode *p;
//	if (VisitStrategy == DEPTH_FIRST) //inserisco il nodo come primo elemneto della lista, p==primo elemento
//		p = firstlist(List);
//	else if (VisitStrategy == BREADTH_FIRST) //inserisco il nodo come ultimo elemento della lista, p==List
//		p = nextlist(lastlist(List));
//	else if (VisitStrategy == BEST_FIRST) { //lista ordinata per costi crescenti, trovo il costo peggiore
//		p = firstlist(List);
//		while ((node->LB[0] >= p->LB[0])
//				|| ((fabs(node->LB[0] - p->LB[0]) < EPSILON
//						&& node->LB[1] >= p->LB[1]) && !endlist(p, List))) // A OR (B AND C)
//			p = nextlist(p);
//	}
//	insert_before(node, p);
//}
/*********************************
 ******	DIFFERENT METHOD	******
 ******						******
 ********************************/
void insert_BnBnode(BandBnode *node, std::list<BandBnode*> *list, int VisitStrategy) {
	if(VisitStrategy == DEPTH_FIRST)
		list->push_front(node);
	else if (VisitStrategy == BREADTH_FIRST)
		list->push_back(node);
	else if (VisitStrategy == BEST_FIRST) {		//ANCHE: (PWC MASSIMA- PWC ATTUALE)/ costo
		for(const auto& temp :*list) {
			if (node->get_LB()[0] >= temp->get_LB()[0])
				list->insert(*node, *temp);
			}
		}
	}

}


////TODO
//BandBnode* Extractlist(BandBnode* *pp, BandBnode* L)	//passa un puntatore di nodi (array di nodi) e la lista da cui estrarli
//		{
//	BandBnode *q;
//	q = *pp;
//	q->prec->succ = q->succ;
//	q->succ->prec = q->prec;
//	*pp = q->succ;
//
//	return q;
//}
//
//void cancel_from_List (BandBnode* p){
//	BandBnode* q = p;
//	q->prec->succ = q->succ;
//	q->succ->prec = q->prec;
//	p = q->succ;
//	delete q;
//}
//
//void delete_List (BandBnode* List){
//	BandBnode* q;
//	for(q = firstlist(List); is_emptylist(List); cancel_from_List(q));	//no q = q->succ perch� cancel_from_list dopo aver cancellato passa la secondo elemento
//	delete q;
//	delete List;
//}
//
//void updateLBset(ParetoFront* PF, BandBnode* List) {	//pointer version
//	BandBnode* p = firstlist(List);
//
//	while(endlist(p,List)) {
//		if(p->useful(PF))
//			PF->UpdatePareto(p->LB[0],p->LB[1], 0);
//		p = nextlist(p);
//	}
//}

//void updateLBset(ParetoFront* PF, list<BandBnode>* list) {	//list version
//	for(const auto& node : list)
//		if(node->useful(PF))
//			PF->UpdatePareto(node->LB[0], node->LB[1], 0);
//}

void calculate_LB(BandBnode* node, ProblemData* PD) {
	PD->update_current(node->get_deleted());

	node->get_LB()[0] = PD->get_current().pairwise();	//pairwise minima (grafo senza tutti nodi tranne quelli non eliminati)
	node->get_LB()[1] = node->get_deleted().count();	//costo minimo (solo nodi eliminati)
}

void calculate_UB(BandBnode* node, ProblemData* PD) {
	PD->update_current(node->get_deleted().flip());
	node->get_deleted().flip();

	node->get_UB()[0] = PD->get_current().pairwise(); //pairwise massima (grafo senza nodi eliminati e senza nodi non visitati)
	node->get_UB()[1] = node->get_saved().size() - node->get_saved().count();	//costo massimo (nodi eliminati e nodi ancora non visitati)
}

int node_to_process(ProblemData* PD) {	//select the node to process taking the max degree avaible
	return PD->get_current().get_max_degree();
}

// Function that computes a LB for objective 1, used inside the B&B algorithm
int node_LB1(ProblemData *pPD,...)
{
  int LB;
// ...

  return LB;
}

// Function that computes a LB for objective 2, used inside the B&B algorithm
int node_LB2(ProblemData *pPD,...)
{
  int LB;
// ...

  return LB;
}

// Function that processes a node from the B&B
//void Process_BandBnode(BandBnode *node, ProblemData *pPD, ParetoFront *PF, long int time_start,...)
//{
//  timeval end;
//
//  // Compute the LBs of the node's selected states:
//  node->LB[0] = node_LB1(pPD,...);
//  node->sol->LB[0];
//  node->LB[1] = node_LB2(pPD,...);
//  node->sol->LB[1];
//
//
//  // Compute heuristic solutions of the Pareto front with the help of the heuristic:
//  gettimeofday(&end,NULL);
//  buildUB(pPD, PF, time_start,...);
//
//
//}

//TODO calculate time
bool Process_BandBnode(std::list<BandBnode*> list, ProblemData *PD, ParetoFront* PF, long int time_start, int visitStrategy) {
	//timeval end;
	BandBnode* node = list.front();
	calculate_LB(node, PD);
	if(!node->useful(PF)) {	//se la pwc minima del nodo attuale � maggiore uguale alla soluzione corrente con pari costo finisco e ritorno false
		list.remove(node);
		return false;
	}

	calculate_UB(node, PD);	//altrimenti calcolo UB e inserisco due nuovi nodi alla lista

	int bit_to_remove = PD->get_current().get_max_degree();	//trovo il prossimo nodo da eliminare

	std::bitset<500>* temp_bitset1 = new std::bitset<500>(node->get_deleted().to_ulong());	//creo i due bitset di nodi rimossi
	std::bitset<500>* temp_bitset2 = new std::bitset<500>(node->get_deleted().to_ulong());

	temp_bitset1->set(bit_to_remove, 1);


	BandBnode* primo = new BandBnode(node->get_id()+1, node->get_livello()+1, std::bitset<500>(), std::bitset<500>());	//TODO set bit to 0
	BandBnode* secondo = new BandBnode(node->get_id()+2, node->get_livello()+1, std::bitset<500>(), std::bitset<500>());	//TODO set bit to 1
	insert_BnBnode(primo, &list, visitStrategy);
	insert_BnBnode(secondo, &list, visitStrategy);
	return true;

}

// Function to solve the problem with branch and bound:
void Solve_BB(ProblemData *pPD, int time, ParetoFront *PF, long int time_start)
{
  int d, s;
  timeval start, end;

  gettimeofday(&start,NULL);
  //double UB=INFINITE;

  buildUB(pPD, 1, PF, time_start,...);	//superfluo

  BandBnode *father, *son;

  // Now evaluate the root node of the branching tree:
  father = new BandBnode(pPD, 1, 0);

  Process_BandBnode(father, pPD, PF, time_start);
  fprintf(stderr,"Father price node: %d -> cost = (%d, %d) -> %d, branching var = %d\n", father->id, father->LB[0], father->LB[1], father->useful(PF), father->var);

  int ExaminedNodes = 1;	//nodi esaminati
  int VisitStrategy=BEST_FIRST;/*BREADTH_FIRST;/*DEPTH_FIRST;*/
  BandBnode *pT;
  pT = new BandBnode(pPD, 0, 0);	//puntatore lista nodi da esaminare (inizio/fine)
  pT->prev = pT;
  pT->next = pT;

  Insert_BandBnode(father, pT, VisitStrategy);	//inserisce il padre nella lista di nodi da esaminare
  int WaitingNodes = 1;
  int id = 1;
  int RemovedNodes = 0;

  gettimeofday(&end,NULL);
  while (emptylist(pT) == 0 && (int)(end.tv_sec-time_start) < time)
  {
    father = Extract_BandBnode(pT);
    WaitingNodes--;

    if (father->useful(PF) == 1) // if FALSE here from the beginning no node is useful and we use only the first node
    {
//fprintf(stderr,"numfigli = %d\n",father->numfigli);
      for (int f = 1; f <= father->numfigli; f++)
      {
        id++;
        son = new BandBnode(pPD, id, father->livello+1);
        son->derive(father, f, pPD);
        Process_BandBnode(son, pPD, PF, time_start);
        //fprintf(stderr,"Solution for node %d is:", son->id);

        //fprintf(stderr,"Price node: %d -> LB1 = %d and LB2 = %d (%d), branching on %d\n", son->id, son->LB[0], son->LB[1], son->useful(PF) == 0, son->job);
        ExaminedNodes++;
        if (son->useful(PF) == 0)
          delete son;
        else
        {
          Insert_BandBnode(son, pT, VisitStrategy);
          WaitingNodes++;
        }
      }
    }
    delete father;
    RemovedNodes++;
    gettimeofday(&end,NULL);
//int scan;
//fscanf(stdin,"%d",&scan);
  }
  ExtractLBset(pLBset, PF, pT, pPD);

  fprintf(stderr,"UB set:\n");
  PrintPareto(PF);
  fprintf(stderr,"LB set:\n");
  PrintPareto(pLBset);

  Destroy_list(&pT);
}



void Solve_BB(ProblemData* PD, ParetoFront* PF, int time, long int time_start) {
	//timeval start, end;
	std::list<BandBnode*> visited_nodes, waiting_nodes;
	int removedNodes, id, livello;

	int visitStrategy = BEST_FIRST; //BREADTH_FIRST, DEPTH_FIRST

	BandBnode* root =  new BandBnode(1, 1, std::bitset<500>(), std::bitset<500>());	//creo il nodo radice con id 1, livello 1 e bitset impostato a 0 (tutti i nodi presenti)
	waiting_nodes.push_front(root);
	Process_BandBnode(waiting_nodes, PD, PF,time_start, visitStrategy);

	visited_nodes.push_front(root);

	while(!waiting_nodes.empty()) {
		if (Process_BandBnode(waiting_nodes, PD, PF, time_start, visitStrategy) )
			removedNodes++;	//se il nodo viene chiuso aumento i nodi rimossi
		visited_nodes.push_back(waiting_nodes.front()); //scriverlo prima?
	}

}

int main() {
	BandBnode Lista = new BandBnode[];
	BandBnode nodo = new BandBnode();
	nodo.id = 66;
	insert_before(&Lista, new BandBnode);
	insert_before(&Lista, new BandBnode);
	insert_before(&Lista, &nodo);

	BandBnode array[2] = Extractlist(&Lista, &nodo);

	printf("%d %d", array[0].id, array[1].id);
	return 0;

}

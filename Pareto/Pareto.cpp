//
// Created by Sdaxddx
//
#include "Pareto.h"
#include "ProblemData.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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


BandBnode::BandBnode(int id, int livello) {
	this->id = id;
	this->livello = livello;
	this->LB = {0,0};
	this->succ = nullptr;
	this->prec = nullptr;
	this->type = 0;
	this->var = 0;
	this->id_figlio = 0;
}

BandBnode::~BandBnode() {
	delete this->prec;
	delete this->succ;
	delete[] this->LB;
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
BandBnode *firstlist(BandBnode *List) { //List è un nodo di chiusura, che viene prima del primo elemento della lista e dopo l'ultimo elemento
	BandBnode *first = List->succ;
	return first;
}

/**
 * Given a List return its last element
 * @return the last element of the list
 */
BandBnode *lastlist(BandBnode *List) {
	BandBnode *last = List->prec;
	return last;
}

/**
 * Given a element and its list check if the node is the last element of the list
 */
bool endlist(BandBnode *p, BandBnode *List) { //se p è l'ultimo elemento della lista allora sarà uguale a List
	return p == List;
}

/**
 * Given a element of a list return the next element of the List
 */
BandBnode *nextlist(BandBnode *p) {  // ritorna l'elemento successivo a p
	return p->succ;
}

void insert_before(BandBnode *node, BandBnode *p) {  //inserisce node prima di p
	p->prec->succ = node;
	node->prec = p->prec;
	node->succ = p;
	p->prec = node;
}

/**
 * Given a List check if it is empty
 */
bool is_emptylist(BandBnode *List) { //controlla se la lista è vuota, controllando che ci sia solo l'elemento List
	if ((List->prec = List) && (List->succ = List))
		return true;
	else
		return false;
}

/**
 * Given a node and a List add the node in the list based on the visit strategy
 */
void insert_BnBnode(BandBnode *node, BandBnode *List, int VisitStrategy) { //inserisce un nodo nella lista in base al tipo di visita
	BandBnode *p;
	if (VisitStrategy == DEPTH_FIRST) //inserisco il nodo come primo elemneto della lista, p==primo elemento
		p = firstlist(List);
	else if (VisitStrategy == BREADTH_FIRST) //inserisco il nodo come ultimo elemento della lista, p==List
		p = nextlist(lastlist(List));
	else if (VisitStrategy == BEST_FIRST) { //lista ordinata per costi crescenti, trovo il costo peggiore
		p = firstlist(List);
		while ((node->LB[0] >= p->LB[0])
				|| ((fabs(node->LB[0] - p->LB[0]) < EPSILON
						&& node->LB[1] >= p->LB[1]) && !endlist(p, List))) // A OR (B AND C)
			p = nextlist(p);
	}
	insert_before(node, p);
}

//TODO
BandBnode* Extractlist(BandBnode* *pp, BandBnode* L)	//passa un puntatore di nodi (array di nodi) e la lista da cui estrarli
		{
	BandBnode *q;
	q = *pp;
	q->prec->succ = q->succ;
	q->succ->prec = q->prec;
	*pp = q->succ;

	return q;
}

void cancel_from_List (BandBnode* p){
	BandBnode* q = p;
	q->prec->succ = q->succ;
	q->succ->prec = q->prec;
	p = q->succ;
	delete q;
}

void delete_List (BandBnode* List){
	BandBnode* q;
	for(q = firstlist(List); is_emptylist(List); cancel_from_List(q));	//no q = q->succ perch� cancel_from_list dopo aver cancellato passa la secondo elemento
	delete q;
	delete List;
}

void updateLBset(ParetoFront* PF, BandBnode* List){
	BandBnode* p = firstlist(List);

	while(endlist(p,List)) {
		if(p->useful(PF))
			PF->UpdatePareto(p->LB[0],p->LB[1], 0);
		p = nextlist(p);
	}
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



int main() {
	BandBnode Lista = new BandBnode[];
	BandBnode nodo = new BandBnode;
	nodo.id = 66;
	insert_before(&Lista, new BandBnode);
	insert_before(&Lista, new BandBnode);
	insert_before(&Lista, &nodo);

	BandBnode array[2] = Extractlist(&Lista, &nodo);

	printf("%d %d", array[0].id, array[1].id);
	return 0;

}

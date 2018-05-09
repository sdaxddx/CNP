//
// Created by Sdaxddx
//

#ifndef PROJECT_PARETO_H
#define PROJECT_PARETO_H
#define DEPTH_FIRST               1 //profondità
#define BREADTH_FIRST             2 //ampiezza
#define BEST_FIRST                3 //miglior PWC
#define EPSILON 0.000001
#define INFINITE 1000000000
#define PAIRWISEMAX 100000000 //TODO calculate max PairWise Connectivity

#include <stdio.h>



class ParetoFront {
public:
    ParetoFront(int dim, costMAX, ProblemData *PD); //costruttore
    ~ParetoFront(); //distruttore
    void PrintPareto(); //stampa ogni soluzione
    void SavePareto(FILE *f);   //salva ogni soluzione sul file passato
    bool DominatedPoint(int PWC, int cost); //controlla se la soluzione passata è dominata da quelle del fronte
    void UpdatePareto(int PWC, int cost, int time); //aggiorna il fronte se la soluzione passata non è dominata

private:    //or package
    int dim;    //dimensione del fronte di pareto = numero soluzioni
    int costMIN;    //costo associato alla PAIRWISEMAX (=0)
    int costMAX;    //costo associato alla PAIRWISE nulla (= attack vector)
    int *PWC;   //soluzioni associate a ogni costo
    int *prec, *succ, *time;    //rispettivamente soluzione precedente, successiva e tempo di esecuzione


};

class BandBnode{
public:
    BandBnode(int id, int livello);
    ~BandBnode();

private:
    int id, livello, id_figlio; //debug, non essenziali
    BandBnode *prec, *succ; //nodi contigui
    int var, type; // tipo di branching
    int LB[2]; //soluzioni del nodo //  LB[0] == costo     LB[1] == PWC
};
#endif //PROJECT_PARETO_H

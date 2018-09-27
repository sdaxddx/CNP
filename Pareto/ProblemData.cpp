/*
 * ProblemData.cpp
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#include "ProblemData.h"

ProblemData::ProblemData(std::ifstream& f) {
	this->original = Graph::Grafo(f);
	this->current = Graph::Grafo(original);

}

ProblemData::~ProblemData() {
	delete original, current;
}

void ProblemData::update_current(std::bitset<500>* removed_nodes) {
	delete current;	//chiamo il decostruttore del problema corrente
	current= Graph::Grafo(original);	//creo un nuovo grafo uguale all'originale
	current.update_Graph(removed_nodes);	//aggiorno con i dati correnti
}


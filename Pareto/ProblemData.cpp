/*
 * ProblemData.cpp
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#include "ProblemData.h"

ProblemData::ProblemData(FILE* file) {
	original = new Graph(file);
}

ProblemData::~ProblemData() {
	delete original, current;
}

void ProblemData::update_current(std::bitset* removed_nodes) {
	delete current;	//chiamo il decostruttore del problema corrente
	current(original);	//creo un nuovo grafo uguale all'originale
	current.update_Graph(removed_nodes);	//aggiorno con i dati correnti
}


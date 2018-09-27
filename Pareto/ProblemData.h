/*
 * ProblemData.h
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#ifndef PARETO_PROBLEMDATA_H_
#define PARETO_PROBLEMDATA_H_

#include "Graph.cpp"
#include "Grafo.h"
#include <bitset>

class ProblemData {
public:
	explicit ProblemData(std::ifstream& f);
	virtual ~ProblemData();
	inline Graph::Grafo get_original() { return original;}
	inline Graph::Grafo get_current() { return current;}

	void update_current(std::bitset<500>* removed_nodes);
protected:
	Graph::Grafo original, current;
};

#endif /* PARETO_PROBLEMDATA_H_ */

/*
 * ProblemData.h
 *
 *  Created on: 15 mag 2018
 *      Author: Sdaxddx
 */

#ifndef PARETO_PROBLEMDATA_H_
#define PARETO_PROBLEMDATA_H_

#include "Graph.cpp"

class ProblemData {
public:
	ProblemData(FILE* f);
	virtual ~ProblemData();

	void update_current(std::bitset* removed_nodes);
protected:
	Graph original, current;
};

#endif /* PARETO_PROBLEMDATA_H_ */

#ifndef CNRT_PGRK_H
#define CNRT_PGRK_H

#include "net.h"
double *pagerank(struct Net *net, double c);
double *simpagerank(struct Net *net, double c, struct Net *simnet);
	
#endif

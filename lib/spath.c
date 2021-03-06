#include "spath.h"
#include "base.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "limits.h"

/*****************standard dijkstra algorithm, used in test.*******************************/
static double ** dijkstra_setasp_undirected_weighted_Net(struct Net *net) {
	double *asp_onetime = malloc((net->maxId + 1) * (net->maxId + 1) * sizeof(double));
	int i, j;
	double **asp = smalloc((net->maxId + 1) * sizeof(double *));
	for (i = 0; i < net->maxId + 1; ++i) {
		asp[i] = asp_onetime + (net->maxId + 1)*i;
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->maxId + 1; ++j) {
			asp[i][j] = INT_MAX;
		}
	}
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			int id = net->edges[i][j];
			asp[i][id] = net->weight[i][j];
		}
	}
	return asp;
}
double *dijkstra_1A_undirected_weighted_Net(struct Net *net, int nid) {
	if (net->weight == NULL || net->inedges != NULL) {
		isError("the net should be weighted and undirected.");
	}
	if (nid < 0 || nid > net->maxId) {
		isError("nid should be in [0, maxId]");
	}
	double **asp = dijkstra_setasp_undirected_weighted_Net(net);
	double *sp = smalloc((net->maxId + 1) * sizeof(double));
	char *flag = smalloc(net->maxId + 1);
	int i;
	for (i = 0; i < net->maxId + 1; ++i) {
		sp[i] = asp[nid][i];
		flag[i] = 0;
	}
	flag[nid] = 1;
	sp[nid] = -1;
	int alreadyflag = 1;
	while (alreadyflag != net->maxId + 1) {
		int be = -1;
		double min = 2.0*INT_MAX;
		for (i = 0; i < net->maxId + 1; ++i) {
			if (flag[i] == 0 && min > sp[i]) {
				min = sp[i];
				be = i;
			}
		}
		flag[be] = 1;
		++alreadyflag;
		for (i = 0; i < net->maxId + 1; ++i) {
			if (flag[i] == 0 && sp[i] > sp[be] + asp[be][i]) {
				sp[i] = sp[be] + asp[be][i];
			}
		}
	}

	free(flag);
	free(asp[0]);
	free(asp);

	return sp;
}
double dijkstra_avesp_undirected_weighted_Net(struct Net *net) {
	if (net->weight == NULL || net->inedges == NULL) {
		isError("the net should be weighted and undirected.");
	}
	double **asp = dijkstra_setasp_undirected_weighted_Net(net);
	double *sp = smalloc((net->maxId + 1) * sizeof(double));
	char *flag = smalloc(net->maxId + 1);

	double avesp = 0;
	int j;
	for (j = 0; j < net->maxId; ++j) {
		int i;
		for (i = 0; i < net->maxId + 1; ++i) {
			sp[i] = asp[j][i];
			flag[i] = 0;
		}
		flag[j] = 1;
		sp[j] = -1;
		int alreadyflag = 1;
		while (alreadyflag != net->maxId + 1) {
			int be = -1;
			double min = 2.0*INT_MAX;
			for (i = 0; i < net->maxId + 1; ++i) {
				if (flag[i] == 0 && min > sp[i]) {
					min = sp[i];
					be = i;
				}
			}
			flag[be] = 1;
			++alreadyflag;
			for (i = 0; i < net->maxId + 1; ++i) {
				if (flag[i] == 0 && sp[i] > sp[be] + asp[be][i]) {
					sp[i] = sp[be] + asp[be][i];
				}
			}
		}

		for (i = j+1; i < net->maxId + 1; ++i) {
			avesp += sp[i];
		}
	}

	free(flag);
	free(asp[0]);
	free(asp);
	free(sp);
	double EE = (double)(net->maxId + 1)*net->maxId/2;
	return avesp/EE;
}
/********************************************************************************************************/

/******************1A&average shortest path for undirect unweight.***************************************/
static void core_spath_1A_undirect_unweight_Net(int *sp, int **left, int **right, int *lNum, int *rNum, struct Net *net, int *STEP_END) {
	int i,j;
	int STEP = 0;
	while (*lNum && STEP != *STEP_END) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				if (sp[neigh] == 0) {
					sp[neigh] = STEP;
					(*right)[(*rNum)++] = neigh;
				}
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
//the returned sp's length is net->maxId + 1.
//sp[x] = 0 means originId is not connected to x.
//only sp[originId] = -1;
//for the other vertices, sp[x] is the shortest path between x and originId.
int *spath_1A_undirect_unweight_Net(struct Net *net, int originId) {
	if (originId<net->minId || originId>net->maxId) {
		isError("originId is not a valid vertex id");
	}
	if (net->inedges != NULL || net->weight != NULL) {
		isError("the net is not an undirected or not an unweighted network");
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = -1;
	core_spath_1A_undirect_unweight_Net(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(left);
	free(right);
	return sp;
}
//return a list of vertex id, the shortest path between these vertices and originId is the value of step.
//*Num is the number of the vertices in the list.
//*ret is the list.
void spath_1A_step_undirect_unweight_Net(struct Net *net, int originId, int step, int *Num, int **ret) {
	if (originId<net->minId || originId>net->maxId) {
		isError("originId is not a valid vertex id");
	}
	if (net->inedges != NULL || net->weight != NULL) {
		isError("the net is not an undirected or not an unweighted network");
	}
	int *sp = calloc(net->maxId + 1, sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;
	lNum = 1;
	left[0] = originId;
	sp[originId] = -1;
	int STEP_END = step;
	core_spath_1A_undirect_unweight_Net(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
	free(sp);
	free(right);
	*Num = lNum;
	*ret = left;
}
void spath_avesp_undirect_unweight_Net(struct Net *net, double *avesp) {
	if (net->inedges != NULL || net->weight != NULL) {
		isError("the net is not an undirected or not an unweighted network");
	}
	if (net->connectnessStatus != NS_CNNTNESS ) {
		isError("this method can only calculate the avesp of a net which is fully connected.");
	}
	int *sp = malloc((net->maxId + 1)*sizeof(int));
	int *left = malloc((net->maxId + 1)*sizeof(int));
	int *right = malloc((net->maxId + 1)*sizeof(int));
	int lNum, rNum;

	int i,j;
	int STEP_END = -1;
	*avesp = 0;
	for (i=0; i<net->maxId + 1; ++i) {
		lNum = 1;
		left[0] = i;
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
		}
		sp[i] = -1;
		core_spath_1A_undirect_unweight_Net(sp, &left, &right, &lNum, &rNum, net, &STEP_END);
		for (j=i+1; j<net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
	}

	free(left);
	free(right);
	free(sp);
	double EE = (double)(net->maxId + 1)*(net->maxId)/2.0;
	*avesp /= EE;
}
/********************************************************************************************************/

/*******get avesp and coupling between two undirected&unweighted net: base and air.**********************/
//to find coupling of two net: base and air.
static void core_spath_avesp_coupling_undirect_unweight_Net(int *sp, char *stage, int **left, int **right, int *lNum, int *rNum, struct Net *base, struct Net *air, double *spa, double *spb, double *spab) {
	int i;
	int j;
	int STEP = 1;
	memset(stage, 0 ,sizeof(char)*(base->maxId + 1));
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<base->degree[id]; ++j) {
				int neigh = base->edges[id][j];
				if (sp[neigh] == 0) {
					spab[neigh] += spab[id];
					spab[neigh] += spa[id];
					spb[neigh] += spb[id];
					if (stage[neigh] == 0) {
						stage[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->degree[id]; ++j) {
					int neigh = air->edges[id][j];
					if (sp[neigh] == 0) {
						spab[neigh] += spab[id];
						spab[neigh] += spb[id];
						spa[neigh] += spa[id];
						if (stage[neigh] == 0) {
							stage[neigh] = 1;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}
		}
		for (i = 0; i < *rNum; ++i) {
			sp[(*right)[i]] = STEP;
			stage[(*right)[i]] = 0;
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
//the vertices in air is a subset of the vertices in base.
void spath_avesp_coupling_undirect_unweight_Net(struct Net *base, struct Net *air, double *avesp, double *coupling) {
	if (base->inedges != NULL || base->weight != NULL) {
		isError("the base net is not undirected or not unweighted.");
	}
	if (air->inedges != NULL || air->weight != NULL) {
		isError("the air net is not undirected or not unweighted.");
	}
	int *sp = smalloc((base->maxId + 1)*sizeof(int));
	int *left = smalloc((base->maxId + 1)*sizeof(int));
	int *right = smalloc((base->maxId + 1)*sizeof(int));
	double *spa = smalloc((base->maxId + 1) * sizeof(double));
	double *spb = smalloc((base->maxId + 1) * sizeof(double));
	double *spab = smalloc((base->maxId + 1) * sizeof(double));
	char *stage = smalloc((base->maxId + 1) * sizeof(char));
	int lNum, rNum;

	int i,j;
	int k;
	*avesp = 0;
	*coupling =0;
	for (i=0; i<base->maxId; ++i) {
		for (j=0; j<base->maxId + 1; ++j) {
			sp[j] = 0;
			spa[j] = 0;
			spb[j] = 0;
			spab[j] = 0;
		}
		sp[i] = -1;
		lNum = 0;
		for (k = 0; k < base->degree[i]; ++k) {
			int to = base->edges[i][k];
			left[lNum++] = to;
			sp[to] = 1;
			++spb[to];
		}
		if (i < air->maxId + 1) {
			for (k = 0; k < air->degree[i]; ++k) {
				int to = air->edges[i][k];
				left[lNum++] = to;
				sp[to] = 1;
				++spa[to];
			}
		}
		core_spath_avesp_coupling_undirect_unweight_Net(sp, stage, &left, &right, &lNum, &rNum, base, air, spa, spb, spab);
		for (j = i+1; j < base->maxId + 1; ++j) {
			*avesp += sp[j];
			*coupling += spab[j]/(spa[j] + spb[j] + spab[j]);
		}
	}

	free(left); free(right);
	free(spa); free(spb); free(spab);
	free(sp);
	free(stage);
	double ij = (double)(base->maxId + 1)*base->maxId/2;
	*avesp /= ij;
	*coupling /= ij;
}
/********************************************************************************************************/

/****************avesp & gini for undirect and unweight net.*********************************************/
//this spath06 is for unweighted_undirected base and weighted_undirected air network.
//to find gini of two net: base and air.
static void set_spAspall_spath_avesp_gini_undirect_unweight_Net(int *sp, char *stage,  int **left, int **right, int *lNum, int *rNum, struct Net *net, double *spall) {
	int i,j;
	int STEP = 1;
	memset(stage, 0 ,sizeof(char)*(net->maxId + 1));
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				if (sp[neigh] == 0) {
					spall[neigh] += spall[id];
					if (stage[neigh] == 0) {
						stage[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
				}
			}
		}
		for (j = 0; j < *rNum; ++j) {
			sp[(*right)[j]] = STEP;
			stage[(*right)[j]] = 0;
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
static void set_edgesAttr_spath_avesp_gini_undirect_unweight_Net(int source, int *sp, char *stage, int **left, int **right, int *lNum, int *rNum, struct Net *net, double *spall) {
	int i,j,k;
	*rNum = 0;
	memset(stage, 0 ,sizeof(char)*(net->maxId + 1));
	for (i = 0; i < net->maxId + 1; ++i) {
		int step = sp[i];
		double aij = spall[i];
		if (step == 1) {
			net->edgesAttr[i][net->edgesMatrix.pp[i][source]] += 1;
			net->edgesAttr[source][net->edgesMatrix.pp[source][i]] += 1;
		}
		else if (step > 1) {
			*lNum = 0;
			(*left)[(*lNum)++] = i;
			while (step != 1) {
				step--;
				*rNum = 0;

				for (k=0; k<*lNum; ++k) {
					int id = (*left)[k];
					for (j=0; j<net->degree[id]; ++j) {
						int neigh = net->edges[id][j];
						if (sp[neigh] == step) {
							net->edgesAttr[id][net->edgesMatrix.pp[id][neigh]] += spall[neigh]/aij;
							net->edgesAttr[neigh][net->edgesMatrix.pp[neigh][id]] += spall[neigh]/aij;
							if (stage[neigh] == 0) {
								stage[neigh] = 1;
								(*right)[(*rNum)++] = neigh;
							}
						}
					}
				}

				for (k = 0; k < *rNum; ++k) {
					int id = (*right)[k];
					stage[id] = 0;	
				}

				int *tmp = *left;
				*left = *right;
				*right = tmp;
				*lNum = *rNum;
			}
			for (k=0; k<*lNum; ++k) {
				int id = (*left)[k];
				net->edgesAttr[id][net->edgesMatrix.pp[id][source]] += 1/aij;
				net->edgesAttr[source][net->edgesMatrix.pp[source][id]] += 1/aij;
			}
		}
	}
}
static double calculate_gini_spath_avesp_gini_undirect_unweight_Net(struct Net *net) {
	int i,j;
	int m,n;
	double diff = 0.0;
	double total = 0.0;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			if (i<net->edges[i][j]) {
				double x1 = net->edgesAttr[i][j];	
				total += x1;
				for (m = 0; m < net->maxId + 1; ++m) {
					for (n = 0; n < net->degree[m]; ++n) {
						if (m < net->edges[m][n]) {
							double x2 = net->edgesAttr[m][n];
							diff += fabs(x1 - x2);
						}
					}
				}
			}
		}
	}
	//double Tij=(double)(net->maxId + 1)*net->maxId/2;
	double E = (double)net->edgesNum;
	double G = diff/(2*E*total/**Tij*/);
	return G;
}
void spath_avesp_gini_undirect_unweight_Net(struct Net *net, double *avesp, double *gini) {
	if (net->inedges != NULL || net->weight != NULL) {
		isError("net should be undirected and unweight");
	}
	if (net->edgesAttr == NULL) {
		isError("gini calculation need edgesAttr element.");
	}
	if (net->edgesMatrix.sign == NS_NON_VALID) {
		set_option_edgesMatrix_Net(net);
	}
	int *sp = smalloc((net->maxId + 1)*sizeof(int));
	int *left = smalloc((net->maxId + 1)*sizeof(int));
	int *right = smalloc((net->maxId + 1)*sizeof(int));
	double *spall = smalloc((net->maxId + 1) * sizeof(double));
	char *stage = smalloc((net->maxId + 1) * sizeof(char));
	int lNum, rNum;

	*avesp = 0;
	*gini = 0;
	int i,j;
	for (i=0; i<net->maxId; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = 0;
			spall[j] = 0;
		}
		sp[i] = -1;
		lNum = 0;
		for (j = 0; j < net->degree[i]; ++j) {
			int to = net->edges[i][j];
			left[lNum++] = to;
			sp[to] = 1;
			++spall[to];
		}
		set_spAspall_spath_avesp_gini_undirect_unweight_Net(sp, stage, &left, &right, &lNum, &rNum, net, spall);
		for (j = i+1; j < net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
		set_edgesAttr_spath_avesp_gini_undirect_unweight_Net(i, sp, stage, &left, &right, &lNum, &rNum, net, spall);
	}

	free(left);
	free(right);
	free(sp);
	free(spall);
	free(stage);
	*avesp /= (double)(net->maxId + 1)*net->maxId/2;
	*gini = calculate_gini_spath_avesp_gini_undirect_unweight_Net(net);
}
/********************************************************************************************************/




/***********get avesp of two undirected&1up_weighted net *****************/
//the vertices in air is a subset of the vertices in base.
//this can only handle the net in which the weight of all edges are not less than 1.0, so called 1up.
//both base and air are requited to satisfy this prerequisite.
static void core_spath_avesp_undirect_1upweight_Net(double *sp, signed char *gs, int *upCSlist, signed char *ups, int upCSlistNum, int **left, int **right, int *lNum, int *rNum, struct Net *net) {
	int i;
	int j;
	int STEP = 1;
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				sp[neigh] = dmin(sp[neigh], sp[id] + net->weight[id][j]);
				if (sp[neigh] < STEP + 1.0 && gs[neigh] == 0) {
					gs[neigh] = 1;
					(*right)[(*rNum)++] = neigh;
				}
				else if (sp[neigh] >= STEP + 1.0 && gs[neigh] == 0 && ups[neigh] == 0) {
					upCSlist[upCSlistNum++] = neigh;
					ups[neigh] = 1;
				}
			}
		}
		for (i = 0; i <upCSlistNum; ) {
			int id = upCSlist[i];
			if (gs[id] == 1) {
				ups[id] = 0;
				upCSlist[i] = upCSlist[--upCSlistNum];
			}
			else if(sp[id]<STEP+1) {
				gs[id] = 1;
				(*right)[(*rNum)++] = id;
				upCSlist[i] = upCSlist[--upCSlistNum];
				ups[id] = 0;
			}
			else {
				++i;
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
	}
}
void spath_avesp_undirect_1upweight_Net(struct Net *net, double *avesp) {
	if (net->inedges != NULL || net->weight == NULL) {
		isError("the net should be undirected and weighted.");
	}
	double *sp = smalloc((net->maxId + 1)*sizeof(double));
	int *left = smalloc((net->maxId + 1)*sizeof(int));
	int *right = smalloc((net->maxId + 1)*sizeof(int));
	int *upCSlist = smalloc((net->maxId + 1)*sizeof(int));
	signed char *gs = smalloc((net->maxId + 1) * sizeof(signed char));
	signed char *ups = scalloc((net->maxId + 1) , sizeof(signed char));
	int lNum, rNum, upCSlistNum;

	int i,j,k;
	*avesp = 0;
	for (i=0; i<net->maxId; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = INT_MAX;
			gs[j] = 0;
		}
		gs[i] = sp[i] = -1;
		lNum = 0;
		upCSlistNum = 0;
		for (k = 0; k < net->degree[i]; ++k) {
			int to = net->edges[i][k];
			sp[to] = net->weight[i][k];
			if (sp[to] < 2.0 && gs[to] == 0) {
				left[lNum++] = to;
				gs[to] = 1;
			}
			else {
				upCSlist[upCSlistNum++] = to;
				ups[to] = 1;
			}
		}
		core_spath_avesp_undirect_1upweight_Net(sp, gs, upCSlist, ups, upCSlistNum, &left, &right, &lNum, &rNum, net);
		for (j = i+1; j < net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
	}

	free(left); free(right);
	free(upCSlist);
	free(sp); free(gs); free(ups);
	double ij = (double)(net->maxId + 1)*net->maxId/2;
	*avesp /= ij;
}
/********************************************************************************************************/

/********************get avesp&coupling of two undirected&1up_weighted net: base and air*****************/
//the vertices in air is a subset of the vertices in base.
//this can only handle the net in which the weight of all edges are not less than 1.0, so called 1up.
//both base and air are requited to satisfy this prerequisite.
/*
 * after while && ++STEP && *rNum=0:
 * 		in left: only have vertices whose sp is [STEP-1, STEP).
 * 		in right: only have vertices whose sp is [STEP, STEP+1).
 * 		in CSlist: only have vertices whose sp is [STEP, +00).
 * 		in left's neighs, if a vertex's gs = 0, then its sp only can be [STEP, +00)
 * 		when a vertex is put into right, gs will be 1.
 * 		after looking into left's neight, CSlist may have some values whose gs is 1, should get rid of them.
 * 		after looking into left's neight, CSlist may have some values whose gs is 0 and sp is [STEP, STEP+1), should 
 * 		put them into right.
 *
 * difficult logic, but should be much faster then dijkstra.
 */
#define ES  0.000000000001
static void core_spath_avesp_coupling_undirect_1upweight_Net(double *sp, signed char *gs, int *upCSlist, signed char *ups, int upCSlistNum, int **left, int **right, int *lNum, int *rNum, struct Net *base, struct Net *air, double *spa, double *spb, double *spab) {
	int i;
	int j;
	int STEP = 1;
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<base->degree[id]; ++j) {
				int neigh = base->edges[id][j];
				sp[neigh] = dmin(sp[neigh], sp[id] + base->weight[id][j]);
				if (sp[neigh] < STEP + 1.0 && gs[neigh] == 0) {
					gs[neigh] = 1;
					(*right)[(*rNum)++] = neigh;
				}
				else if (sp[neigh] >= STEP + 1.0 && gs[neigh] == 0 && ups[neigh] == 0) {
					upCSlist[upCSlistNum++] = neigh;
					ups[neigh] = 1;
				}

			}
			if(id < air->maxId + 1) {
				for (j=0; j<air->degree[id]; ++j) {
					int neigh = air->edges[id][j];
					sp[neigh] = dmin(sp[neigh], sp[id] + air->weight[id][j]);
					if (sp[neigh] < STEP + 1.0 && gs[neigh] == 0) {
						gs[neigh] = 1;
						(*right)[(*rNum)++] = neigh;
					}
					else if (sp[neigh] >= STEP + 1.0 && gs[neigh] == 0 && ups[neigh] == 0) {
						upCSlist[upCSlistNum++] = neigh;
						ups[neigh] = 1;
					}
				}
			}
		}
		for (i = 0; i <upCSlistNum; ) {
			int id = upCSlist[i];
			if (gs[id] == 1) {
				ups[id] = 0;
				upCSlist[i] = upCSlist[--upCSlistNum];
			}
			else if(sp[id]<STEP+1) {
				gs[id] = 1;
				(*right)[(*rNum)++] = id;
				upCSlist[i] = upCSlist[--upCSlistNum];
				ups[id] = 0;
			}
			else {
				++i;
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;
		for (i = 0; i < *lNum; ++i) {
			int id = (*left)[i];
			int s = 0;
			for (j = 0; j < base->degree[id]; ++j) {
				int neigh = base->edges[id][j];
				if (gs[neigh]==1 && fabs(sp[id] - sp[neigh] - base->weight[id][j]) < ES) {
					spb[id] += spb[neigh];
					spab[id] += spa[neigh] + spab[neigh];
					s = 1;
				}
				else if (gs[neigh]==-1) {
					isError("should not arrive here.");
				}
			}
			if (id < air->maxId + 1) {
				for (j = 0; j < air->degree[id]; ++j) {
					int neigh = air->edges[id][j];
					if (gs[neigh]==1 && fabs(sp[id] - sp[neigh] - air->weight[id][j]) < ES) {
						spa[id] += spa[neigh];
						spab[id] += spb[neigh] + spab[neigh];
						s=1;
					}
					else if (gs[neigh]==-1 && fabs(sp[id] - air->weight[id][j]) < ES) {
						spa[id]++;
						s=1;
					}
				}
			}
			if (s == 0) {
				isError("should not arrive here. s = 0");
				//for (j = 0; j < base->degree[id]; ++j) {
				//	int neigh = base->edges[id][j];
				//	printf("b: %d\t%d\t%f\n", id, neigh, sp[id]-sp[neigh]-base->weight[id][j]);
				//}
				//if (id < air->maxId + 1) {
				//	for (j = 0; j < air->degree[id]; ++j) {
				//		int neigh = air->edges[id][j];
				//		printf("a: %d\t%d\t%f\n", id, neigh, sp[id]-sp[neigh]-air->weight[id][j]);
				//	}
				//}
			}
		}
	}
}
void spath_avesp_coupling_undirect_1upweight_Net(struct Net *base, struct Net *air, double *avesp, double *coupling) {
	if (base->inedges != NULL|| base->weight == NULL) {
		isError("the base net should be undirected and weighted.");
	}
	if (air->inedges != NULL || air->weight == NULL) {
		isError("the air net should be undirected and weighted.");
	}
	double *sp = smalloc((base->maxId + 1)*sizeof(double));
	double *spa = smalloc((base->maxId + 1) * sizeof(double));
	double *spb = smalloc((base->maxId + 1) * sizeof(double));
	double *spab = smalloc((base->maxId + 1) * sizeof(double));
	int *left = smalloc((base->maxId + 1)*sizeof(int));
	int *right = smalloc((base->maxId + 1)*sizeof(int));
	int *upCSlist = smalloc((base->maxId + 1)*sizeof(int));
	signed char *gs = smalloc((base->maxId + 1) * sizeof(signed char));
	signed char *ups = scalloc((base->maxId + 1) , sizeof(signed char));
	int lNum, rNum, upCSlistNum;

	int i,j,k;
	*avesp = 0;
	*coupling = 0;
	for (i=0; i<base->maxId; ++i) {
		for (j=0; j<base->maxId + 1; ++j) {
			sp[j] = INT_MAX;
			gs[j] = 0;
			spa[j] = 0;
			spb[j] = 0;
			spab[j] = 0;
		}
		gs[i] = sp[i] = -1;
		lNum = 0;
		upCSlistNum = 0;
		for (k = 0; k < base->degree[i]; ++k) {
			int to = base->edges[i][k];
			left[lNum++] = to;
			sp[to] = 1;
			gs[to] = 1;
			spb[to]++;
		}
		if (i < air->maxId + 1) {
			for (k = 0; k < air->degree[i]; ++k) {
				int to = air->edges[i][k];
				sp[to] = air->weight[i][k];
				if (sp[to] < 2.0) {
					left[lNum++] = to;
					gs[to] = 1;
					spa[to]++;
				}
				else {
					upCSlist[upCSlistNum++] = to;
					ups[to] = 1;
				}
			}
		}
		core_spath_avesp_coupling_undirect_1upweight_Net(sp, gs, upCSlist, ups, upCSlistNum, &left, &right, &lNum, &rNum, base, air, spa, spb, spab);
		for (j = i+1; j < base->maxId + 1; ++j) {
			*avesp += sp[j];
			*coupling += spab[j]/(spa[j] + spb[j] + spab[j]);
		}
	}

	free(left); free(right);
	free(upCSlist);
	free(sp); free(gs); free(ups);
	free(spa); free(spb); free(spab);
	double ij = (double)(base->maxId + 1)*base->maxId/2;
	*avesp /= ij;
	*coupling /= ij;
}
/********************************************************************************************************/

/***********get avesp&gini of two undirected&1up_weighted net: net *****************/
//the vertices in air is a subset of the vertices in base.
//this can only handle the net in which the weight of all edges are not less than 1.0, so called 1up.
//both base and air are requited to satisfy this prerequisite.
static void set_spAspall_spath_avesp_gini_undirect_1upweight_Net(double *sp, double *spall, signed char *gs, int *upCSlist, signed char *ups, int upCSlistNum, int **left, int **right, int *lNum, int *rNum, struct Net *net) {
	int i;
	int j;
	int STEP = 1;
	while (*lNum) {
		++STEP;
		*rNum = 0;

		for (i=0; i<*lNum; ++i) {
			int id = (*left)[i];
			for (j=0; j<net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				sp[neigh] = dmin(sp[neigh], sp[id] + net->weight[id][j]);
				if (sp[neigh] < STEP + 1.0 && gs[neigh] == 0) {
					gs[neigh] = 1;
					(*right)[(*rNum)++] = neigh;
				}
				else if (sp[neigh] >= STEP + 1.0 && gs[neigh] == 0 && ups[neigh] == 0) {
					upCSlist[upCSlistNum++] = neigh;
					ups[neigh] = 1;
				}
			}
		}
		for (i = 0; i <upCSlistNum; ) {
			int id = upCSlist[i];
			if (gs[id] == 1) {
				ups[id] = 0;
				upCSlist[i] = upCSlist[--upCSlistNum];
			}
			else if(sp[id]<STEP+1) {
				gs[id] = 1;
				(*right)[(*rNum)++] = id;
				upCSlist[i] = upCSlist[--upCSlistNum];
				ups[id] = 0;
			}
			else {
				++i;
			}
		}
		int *tmp = *left;
		*left = *right;
		*right = tmp;
		*lNum = *rNum;

		for (i = 0; i < *lNum; ++i) {
			int id = (*left)[i];
			int s = 0;
			for (j = 0; j < net->degree[id]; ++j) {
				int neigh = net->edges[id][j];
				if (gs[neigh]==1 && fabs(sp[id] - sp[neigh] - net->weight[id][j]) < ES) {
					spall[id] += spall[neigh];
					s = 1;
				}
				else if (gs[neigh]==-1 && fabs(sp[id] - net->weight[id][j]) < ES) {
					spall[id]++;
					s=1;
				}
			}
			if (s == 0) {
				isError("should not arrive here. s = 0");
			}
		}
	}
}
static void set_edgesAttr_spath_avesp_gini_undirect_1upweight_Net(int source, double *sp, double *spall, signed char *stage, int **left, int **right, int *lNum, int *rNum, struct Net *net) {
	int i,j,k;
	for (i = 0; i < net->maxId + 1; ++i) {
		if (sp[i] < 0.5) continue;
		double aij = spall[i];
		*lNum = 0;
		(*left)[(*lNum)++] = i;
		while (*lNum) {
			*rNum = 0;
			for (k=0; k<*lNum; ++k) {
				int id = (*left)[k];
				for (j=0; j<net->degree[id]; ++j) {
					int neigh = net->edges[id][j];
					if (fabs(sp[neigh] + net->weight[id][j] - sp[id]) < ES) {
						net->edgesAttr[id][net->edgesMatrix.pp[id][neigh]] += spall[neigh]/aij;
						net->edgesAttr[neigh][net->edgesMatrix.pp[neigh][id]] += spall[neigh]/aij;
						if (stage[neigh] == 0) {
							stage[neigh] = 1;
							(*right)[(*rNum)++] = neigh;
						}
					}
				}
			}

			for (k = 0; k < *rNum; ++k) {
				int id = (*right)[k];
				stage[id] = 0;	
			}

			int *tmp = *left;
			*left = *right;
			*right = tmp;
			*lNum = *rNum;
		}
	}
}
static double calculate_gini_spath_avesp_gini_undirect_1upweight_Net(struct Net *net) {
	int i,j;
	int m,n;
	double diff = 0.0;
	double total = 0.0;
	for (i = 0; i < net->maxId + 1; ++i) {
		for (j = 0; j < net->degree[i]; ++j) {
			if (i<net->edges[i][j]) {
				double x1 = net->edgesAttr[i][j];	
				total += x1;
				for (m = 0; m < net->maxId + 1; ++m) {
					for (n = 0; n < net->degree[m]; ++n) {
						if (m < net->edges[m][n]) {
							double x2 = net->edgesAttr[m][n];
							diff += fabs(x1 - x2);
						}
					}
				}
			}
		}
	}
	//double Tij=(double)(net->maxId + 1)*net->maxId/2;
	double E = (double)net->edgesNum;
	double G = diff/(2*E*total/**Tij*/);
	return G;
}
void spath_avesp_gini_undirect_1upweight_Net(struct Net *net, double *avesp, double *gini) {
	if (net->inedges != NULL || net->weight == NULL) {
		isError("the net should be undirected and weighted.");
	}
	if (net->edgesAttr == NULL) {
		isError("gini calculation need edgesAttr element.");
	}
	if (net->edgesMatrix.sign == NS_NON_VALID) {
		set_option_edgesMatrix_Net(net);
	}
	double *sp = smalloc((net->maxId + 1)*sizeof(double));
	double *spall = smalloc((net->maxId + 1)*sizeof(double));
	int *left = smalloc((net->maxId + 1)*sizeof(int));
	int *right = smalloc((net->maxId + 1)*sizeof(int));
	int *upCSlist = smalloc((net->maxId + 1)*sizeof(int));
	signed char *gs = smalloc((net->maxId + 1) * sizeof(signed char));
	signed char *ups = scalloc((net->maxId + 1) , sizeof(signed char));
	signed char *stage = scalloc((net->maxId + 1) , sizeof(signed char));
	int lNum, rNum, upCSlistNum;

	int i,j,k;
	*avesp = 0;
	*gini = 0;
	for (i=0; i<net->maxId; ++i) {
		for (j=0; j<net->maxId + 1; ++j) {
			sp[j] = INT_MAX;
			spall[j] = 0;
			gs[j] = 0;
		}
		gs[i] = sp[i] = -1;
		lNum = 0;
		upCSlistNum = 0;
		for (k = 0; k < net->degree[i]; ++k) {
			int to = net->edges[i][k];
			sp[to] = net->weight[i][k];
			if (sp[to] < 2.0 && gs[to] == 0) {
				left[lNum++] = to;
				gs[to] = 1;
				spall[to]++;
			}
			else {
				upCSlist[upCSlistNum++] = to;
				ups[to] = 1;
			}
		}
		set_spAspall_spath_avesp_gini_undirect_1upweight_Net(sp, spall, gs, upCSlist, ups, upCSlistNum, &left, &right, &lNum, &rNum, net);
		for (j = i+1; j < net->maxId + 1; ++j) {
			*avesp += sp[j];
		}
		sp[i] = 0;
		spall[i] = 1;
		set_edgesAttr_spath_avesp_gini_undirect_1upweight_Net(i, sp, spall, stage, &left, &right, &lNum, &rNum, net);
	}

	free(left); free(right);
	free(upCSlist);
	free(sp); free(spall);
	free(gs); free(ups); free(stage);
	double ij = (double)(net->maxId + 1)*net->maxId/2;
	*avesp /= ij;
	*gini = calculate_gini_spath_avesp_gini_undirect_1upweight_Net(net);
}
/********************************************************************************************************/


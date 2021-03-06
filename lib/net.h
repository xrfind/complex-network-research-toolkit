#ifndef CNRT_NET_H
#define CNRT_NET_H

enum NET_STATUS {
	NS_NON_VALID, NS_VALID,
	NS_DUPPAIRS, NS_NON_DUPPAIRS, 
	NS_CNNTNESS, NS_NON_CNNTNESS, 
	NS_NOTSURE
};

struct netOption_d{
	enum NET_STATUS sign;
	double value;
};

struct netOption_i{
	enum NET_STATUS sign;
	int value;
};

struct netOption_ipp{
	enum NET_STATUS sign;
	int **pp;
};

/*
 * there are a lot of options, some optins may seem important and should not be only an option.
 * the reason I put them in options is :
 * 		may be not existed by default.
 * 		may be memory-greedy 
 * 		may be time-greedy(computing-complex)
 * 		may be difficult to maintain when modifying the net.
 * 		or they will not used by me frequently.
 *
 * if net is directed, 
 * 		diffence between degree&indegree, edges&inedges, weight&inweight is obvious.
 * if net is undirected,
 * 		NULL == indegree == inedges == inweight.
 *
 * when net is undirected, each edge is stored twice in "edges" element.
 * 		an edge between 34 and 22, there are edges[34][x] = 22 and edges[22][y] = 34. 
 *
 * when net is directed, each edge is stored only once in "edges" element, and  stored another time in "inedges" element.
 * 		an edge from 34 to 22, there is edges[34][x] = 22, but edges[22][y] = 34 maybe not existed(depends on whether there is an edge from 22 to 34.)
 * 		but inedge[22][z] = 34 is existed.
 *
 * if weight != NULL, then the Net is weighted.
 * if weight == NULL, then the Net is unweighted.
 * if inedges != NULL, then the Net is directed.
 * if inedges == NULL, then the Net is undirected.
 * 		thus, there are some other rules:
 * 		1, if weight != NULL and inedges != NULL, then inweight must not be NULL.
 * 		2, same inedgesAttr.
 *
 */
struct Net{
	//the number of vertices.
	int idNum; 
	//the maximum and minimum id of the vertices.
	//the scope of ids is [0, ∞), but the minId doesn't have to be 0.
	//"maxId - minId + 1" doen't have to be "idNum", hole is acceptable.
	int maxId; 
	int minId; 
	//the number of edges.
	long edgesNum; 
	//degree[11] means the degree of the vertex whose id is 11.
	int *degree; 
	//indegree will be NULL in a undirected net.
	int *indegree;
	//edges[23][2] means the id of the third neighbour of the vertex whose id is 23.
	//edges[23] doesn't have to be a sorted array.
	int **edges; //essential
	//inedges will be NULL in a undirected net
	int **inedges;
	//same structure to edges.
	//the values are got from d1 of LineFile.
	double **weight;
	double **inweight;
	//same structure to edges.
	//this will not be used often, but when an edge has some extra attribute, this is the place storing that.
	//the values are got from d2 of LineFile.
	double **edgesAttr;
	double **inedgesAttr;
	
	//connectnessStatus == NS_NON_CNNTNESS, not fully connected.
	//connectnessStatus == NS_CNNTNESS, fully connected.
	//connectnessStatus == NS_NOTSURE, not sure whether the net is fully connected or not.
	enum NET_STATUS connectnessStatus;
	//duplicatepairsStatus == NS_NON_DUPPAIRS, no duplicatepairs.
	//duplicatepairsStatus == NS_DUPPAIRS, has duplicatepairs.
	//duplicatepairsStatus == NS_NOTSURE, not sure whether there are any duplicate pairs or not.
	enum NET_STATUS duplicatepairsStatus;

	//if degreeMax.sign == NS_VALID, then degreeMax.value is valid.
	//if degreeMax.sign == NS_NON_VALID, then degreeMax.value can not be used.
	struct netOption_i degreeMax;
	struct netOption_i degreeMin;
	struct netOption_i indegreeMax;
	struct netOption_i indegreeMin;
	//average shortest path.
	struct netOption_d avesp;

	//be careful with this option. it's a matrix: (maxId + 1)^2 * sizeof(int) BYTE.
	//this option should be used when you have quite sufficient memory space and you want to find a edge ASAP.
	//e.g. if you want to find a edge's weight, the edge is from 23 to 77.
	//		then the value of edgesMatrix.pp[23][77] is 55; 
	//		then the value of weight[23][55] is 43.23;
	//		then 43.23 is the weight of the edge.
	//		if edgesMatrix.pp[23][77] is -1, it means there is no edge from 23 to 77.
	struct netOption_ipp edgesMatrix;

};

//free net, net itself is dynamically allocated.
void free_Net(struct Net *net);

//LineFile is another core structure, more basic.
/*
 * create_Net is undirected, wighted or not depends on file->d1 is available or not. file->d2 decide the edgesAttr.
 * create_directed_Net is directed, other same as above.
 */
#include "linefile.h"
struct Net *create_Net(struct LineFile * file);
struct Net *create_directed_Net(struct LineFile * file);

//generate the Matrix according to edges element.
//may be there will be a inedgesMatrix element later.
void set_option_edgesMatrix_Net(struct Net *net);
//test duplicatepairs only according to edges element.
void set_status_connectness_Net(struct Net *net);
//test duplicatepairs only according to edges element.
//if there is no duplicatepairs in edges element, then no duplicatepairs
//in the entire net.
void set_status_duplicatepairs_Net(struct Net *net);
void clean_duplicatepairs_Net(struct Net *net, char *cleanfilename, char *duplicatefilename);
void delete_duplicatepairs_Net(struct Net *net);

//common neighbour, directed net.
struct LineFile *similarity_linkout_CN_directed_Net(struct Net *net);
struct LineFile *similarity_linkin_CN_directed_Net(struct Net *net);
struct LineFile *similarity_linkboth_CN_directed_Net(struct Net *net);
//common neighbour, undirected net.
struct LineFile *similarity_CN_Net(struct Net *net, struct Net *acc);

//spread for onion-simpagerank.
int *sir_spread_scope_Net(struct Net *net, double lambda, int *initInfect, int initInfectNum);

#endif

#include <check.h>
#include "spath.h"
#include "mtprand.h"
#include "../research/tdpotn/common.h"
#include <stdio.h>
#include "dataset.h"
#include "base.h"
#include <math.h>
#define ES  0.0000000000000001

static test_spath_avesp_coupling_undirect_unweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = 0;
	double alpha = get_d_MTPR()+0.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;	
	}
	baself->d1=weight;
	struct Net *base = create_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	struct Net *air = create_Net(airlf);
	double coupling;
	spath_avesp_coupling_undirect_unweight_Net(base, air, avesp, &coupling);
	free_Net(base);
	free_Net(air);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	free_Net(addnet);
}

START_TEST (test_spath_avesp_coupling_undirect_unweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;

	D_12 = 1; N = 1000;
	test_spath_avesp_coupling_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
	ck_assert(fabs(avesp - avesp_dj) < ES);

	D_12 = 2; N = 34*34;
	test_spath_avesp_coupling_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
	ck_assert(fabs(avesp - avesp_dj) < ES);
}
END_TEST

static void test_spath_avesp_gini_undirect_unweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN =5;
	double theta=1.0+get_d_MTPR(), lambda = 0;
	double alpha = get_d_MTPR()+1.5;

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *edgesAttr = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr[i] = 0.0;
	}
	baself->d2 = edgesAttr;
	double *weight = smalloc(baself->linesNum * sizeof(double));
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;

	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	free_Net(base);
	double *edgesAttr2 = smalloc(baself->linesNum * sizeof(double));
	for (i = 0; i < baself->linesNum; ++i) {
		edgesAttr2[i] = 0.0;
	}
	airlf->d2 = edgesAttr2;

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	double gini;
	double **wt = addnet->weight;
	addnet->weight = NULL;
	spath_avesp_gini_undirect_unweight_Net(addnet, avesp, &gini);
	addnet->weight = wt;
	free_Net(addnet);
}

START_TEST (test_spath_avesp_gini_undirect_unweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;
	
	D_12 = 1; N = 1000;
	test_spath_avesp_gini_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
	ck_assert(fabs(avesp - avesp_dj) < ES);

	D_12 = 2; N = 34 * 34;
	test_spath_avesp_gini_undirect_unweight_Net_core(D_12, N, &avesp, &avesp_dj);
	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
	ck_assert(fabs(avesp - avesp_dj) < ES);

}
END_TEST

static void test_spath_avesp_undirect_1upweight_Net_core(\
		int D_12, int N, double *avesp, double *avesp_dj) {

	set_timeseed_MTPR();
	int limitN = 5;
	double theta=1.0 + get_d_MTPR(), lambda = get_d01_MTPR();
	double alpha = 1.5 + get_d_MTPR();

	struct LineFile *baself = tdpotn_lf(D_12, N);
	double *weight = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		weight[i] = 1.0;
	}
	baself->d1 = weight;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;
	struct LineFile *airlf = tdpotn_create_air(base, alpha, limitN, theta, lambda);
	struct Net *air = create_weighted_Net(airlf);
	spath_avesp_undirect_1upweight_Net(base, air, avesp);
	free_Net(base);
	free_Net(air);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	*avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	free_Net(addnet);
}

START_TEST (test_spath_avesp_undirect_1upweight_Net)
{
	int D_12, N;
	double avesp, avesp_dj;

	int i;
	for (i = 0; i < 10; ++i) {
		D_12 = get_i31_MTPR()%2 + 1; 
		int L = 5+get_i31_MTPR()%30;
		N = L*L;
		test_spath_avesp_undirect_1upweight_Net_core(D_12, N, &avesp, &avesp_dj);
		printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);
		ck_assert(fabs(avesp - avesp_dj) < ES);
	}
}
END_TEST

Suite *spath_suite(void) {
	Suite *s = suite_create("shortest path");

	TCase *tc_spath_avesp_coupling_undirect_unweight_Net = tcase_create("spath_avesp_coupling_undirect_unweight_Net");
	tcase_set_timeout(tc_spath_avesp_coupling_undirect_unweight_Net, 0);
	tcase_add_test(tc_spath_avesp_coupling_undirect_unweight_Net, test_spath_avesp_coupling_undirect_unweight_Net);
	suite_add_tcase(s, tc_spath_avesp_coupling_undirect_unweight_Net);


	TCase *tc_spath_avesp_gini_undirect_unweight_Net = tcase_create("spath_avesp_gini_undirect_unweight_Net");
	tcase_set_timeout(tc_spath_avesp_gini_undirect_unweight_Net, 0);
	tcase_add_test(tc_spath_avesp_gini_undirect_unweight_Net, test_spath_avesp_gini_undirect_unweight_Net);
	suite_add_tcase(s, tc_spath_avesp_gini_undirect_unweight_Net);


	TCase *tc_spath_avesp_undirect_1upweight_Net = tcase_create("spath_avesp_undirect_1upweight_Net");
	tcase_set_timeout(tc_spath_avesp_undirect_1upweight_Net, 0);
	tcase_add_test(tc_spath_avesp_undirect_1upweight_Net, test_spath_avesp_undirect_1upweight_Net);
	suite_add_tcase(s, tc_spath_avesp_undirect_1upweight_Net);
	return s;
}

/*
START_TEST (test_spath_avesp_undirect_1upweight_Net_0)
{

	struct LineFile *baself = line1d_DS(20, DS_CYCLE, DS_NON_DIRECT);
	double *d1 = smalloc(baself->linesNum * sizeof(double));
	long i;
	for (i = 0; i < baself->linesNum; ++i) {
		d1[i] = 1.0;
	}
	baself->d1 = d1;
	struct Net *base = create_weighted_Net(baself);
	base->duplicatepairsStatus = NS_NON_DUPPAIRS;

	struct LineFile *airlf = create_LineFile("/tmp/airlf_20", 1, 1, 2, -1);
	struct Net *air = create_weighted_Net(airlf);

	struct LineFile *addlf = add_LineFile(airlf, baself);
	free_LineFile(baself);
	free_LineFile(airlf);
	struct Net *addnet = create_weighted_Net(addlf);
	free_LineFile(addlf);
	double avesp_dj = dijkstra_avesp_undirected_weighted_Net(addnet);
	free_Net(addnet);

	double avesp;
	spath_avesp_undirect_1upweight_Net(base, air, &avesp);
	free_Net(base);
	free_Net(air);

	printf("avesp: %f, avesp_dj: %f\n", avesp, avesp_dj);fflush(stdout);
	ck_assert(fabs(avesp - avesp_dj) < ES);
}
END_TEST
*/

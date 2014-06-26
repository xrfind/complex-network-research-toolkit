#include <check.h>
#include "main.h"
#include "base.h"
#include <stdlib.h>

int main(void) {

	SRunner *sr;
   	sr = srunner_create(spath_suite());
   	//sr = srunner_create(net_line1d_suite());
	//srunner_add_suite(sr, net_line1d_suite());
	//srunner_add_suite(sr, net_lattice2d_suite());
	//srunner_add_suite(sr, utilities_suite());
	//srunner_add_suite(sr, linefile_suite());



	srunner_set_log (sr, "test.log");
	srunner_run_all(sr, CK_VERBOSE);

	int number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS: EXIT_FAILURE;
}

#include "mex.h"
#include "string.h"

#ifndef VERSION
#error "VERSION must be passed in by command: -DVERSION=<version>"#define VERSION ""
#endif

#define stringify_(x) #x
#define stringify(x) stringify_(x)

/*
* This function checks if the version that was stored at compile time equals the current version
* given by the ert_linux_target_version function.
* Returns true when the mex is up to date, false when it needs to be recompiled
*/
void mexFunction( int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	mxArray *result, *version_actual;
	char *version_local_str = stringify(VERSION);
	char *version_actual_str;
	mexCallMATLAB(1, &version_actual, 0, NULL, "ert_linux_target_version");

	version_actual_str = mxArrayToString(version_actual);
	if (strcmp(version_actual_str, version_local_str)==0) {
		plhs[0] = mxCreateLogicalScalar(true);
	} else {
		plhs[0] = mxCreateLogicalScalar(false);
	}
}
#define S_FUNCTION_NAME sfcn_UDPReceive

#include "header.c"

#include "simstruc.h"

#define PARAM_NAME_BUFF_LEN "buff_len"

enum params {
	PARAM_BUFF_LEN,
	PARAM_COUNT,
};

enum outputs {
	OUT_FNC_CALL,
	OUT_MSG,
	OUT_LEN,
	OUT_COUNT
};

static void mdlInitializeSizes(SimStruct *S) {
	ssSetNumSFcnParams(S, PARAM_COUNT);
	if (!ssSetNumInputPorts(S,0))
		return;
	if (!ssSetNumOutputPorts(S, OUT_COUNT))
		return;

	ssSetOutputPortWidth(S, OUT_FNC_CALL, 1);
	ssSetOutputPortDataType(S, OUT_FNC_CALL, SS_FCN_CALL);
	ssSetOutputPortComplexSignal(S, OUT_FNC_CALL, COMPLEX_NO);

	ssSetOutputPortWidth(S, OUT_MSG, (int_T)mxGetPr(ssGetSFcnParam(S, PARAM_BUFF_LEN))[0]);
	ssSetOutputPortDataType(S, OUT_MSG, SS_UINT8);
	ssSetOutputPortComplexSignal(S, OUT_MSG, COMPLEX_NO);

	ssSetOutputPortWidth(S, OUT_LEN, 1);
	ssSetOutputPortDataType(S, OUT_LEN, SS_UINT32);
	ssSetOutputPortComplexSignal(S, OUT_LEN, COMPLEX_NO);

	ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
	ssSetNumSampleTimes(S, 1);
	ssSetOptions(S,
	SS_OPTION_WORKS_WITH_CODE_REUSE |
	SS_OPTION_CAN_BE_CALLED_CONDITIONALLY |
	SS_OPTION_USE_TLC_WITH_ACCELERATOR);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0);
	ssSetCallSystemOutput(S,0);
}

#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S) {
	if (!ssSetNumRunTimeParams(S, PARAM_COUNT))
		return;
	
	ssRegDlgParamAsRunTimeParam(S, PARAM_BUFF_LEN, PARAM_BUFF_LEN, PARAM_NAME_BUFF_LEN, SS_UINT32);
}
#endif

#define UNUSED_MDLCHECKPARAMETERS
#define UNUSED_MDLTERMINATE
#define UNUSED_MDLOUTPUTS
#include "trailer.c"
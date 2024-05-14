/************************************************************************************//**
* \file			sfcn_UDPReceive.c
* \brief		matlab s-function for receiving UDP messages
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* Permission is hereby granted, free of charge, to any person obtaining a copy of this
* software and associated documentation files (the "Software"), to deal in the Software
* without restriction, including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
* PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
* FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
* \endinternal
****************************************************************************************/

#define S_FUNCTION_NAME sfcn_UDPReceive

#include "header.c"

#include "simstruc.h"

#define PARAM_NAME_BUFF_LEN "buff_len"
#define PARAM_NAME_SOCKET_ID "socket_id"

enum params {
	PARAM_BUFF_LEN,
	PARAM_SOCKET_ID,
	PARAM_TSAMP,
	PARAM_COUNT,
};

enum outputs {
	OUT_FCN_CALL,
	OUT_MSG,
	OUT_LEN,
	OUT_COUNT
};

static void mdlInitializeSizes(SimStruct *S) {
	ssSetNumSFcnParams(S, PARAM_COUNT);
	ssSetSFcnParamTunable(S,PARAM_TSAMP,false);
	ssSetSFcnParamTunable(S,PARAM_BUFF_LEN,false);
	ssSetSFcnParamTunable(S,PARAM_SOCKET_ID,false);
	if (!ssSetNumInputPorts(S,0))
		return;
	if (!ssSetNumOutputPorts(S, OUT_COUNT))
		return;

	AddOutputPort(S, OUT_FCN_CALL, SS_FCN_CALL);
	AddOutputVectorPort(S, OUT_MSG, SS_UINT8, (int_T)mxGetPr(ssGetSFcnParam(S,PARAM_BUFF_LEN))[0]);
	AddOutputPort(S, OUT_LEN, SS_UINT32);

	ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
	SetStandardOptions(S);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	ssSetNumSampleTimes(S,1);
	ssSetSampleTime(S, 0, mxGetPr(ssGetSFcnParam(S, PARAM_TSAMP))[0]);
	ssSetOffsetTime(S, 0, 0);
	ssSetCallSystemOutput(S,OUT_FCN_CALL);
}

#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S) {
	if (!ssSetNumRunTimeParams(S, 1))
		return;
	
	ssRegDlgParamAsRunTimeParam(S, PARAM_BUFF_LEN, PARAM_BUFF_LEN, PARAM_NAME_BUFF_LEN, SS_UINT32);
}
#endif

//Only allowed to write string parameter to RTW here
#ifdef MATLAB_MEX_FILE
#define MDL_RTW
static void mdlRTW(SimStruct *S){
	char_T *id = mxArrayToString(ssGetSFcnParam(S,PARAM_SOCKET_ID));
	ssWriteRTWParamSettings(S, 1, SSWRITE_VALUE_STR, PARAM_NAME_SOCKET_ID, id);
}
#endif

#define UNUSED_MDLCHECKPARAMETERS
#define UNUSED_MDLTERMINATE
#define UNUSED_MDLOUTPUTS
#include "trailer.c"
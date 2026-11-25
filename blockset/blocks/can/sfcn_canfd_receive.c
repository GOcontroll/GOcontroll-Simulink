/************************************************************************************//**
* \file         sfcn_canfd_receive.c
* \brief        Simulink S-function of the CAN FD Receive block: every frame received on
*               one CAN bus, classic CAN or CAN FD, as a CAN_FD_MESSAGE_BUS, with a
*               function call for each frame.
*
*               The message is the bus type of the CAN FD Pack and CAN FD Unpack blocks
*               of the Vehicle Network Toolbox. The bus object CAN_FD_MESSAGE_BUS has to
*               exist when the block is initialized; this function creates it with
*               canFDMessageBusType when it is missing.
*
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*  Copyright 2026 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
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
*
* Compile with (from the blockset/blocks/can directory):
*   mex sfcn_canfd_receive.c
****************************************************************************************/

#define S_FUNCTION_NAME sfcn_canfd_receive

#include "../header.c"
#include "simstruc.h"

#define PARAM_NAME_MODULE_ID "module_id" /* CAN bus number starting at 1 */
#define CANFD_BUS_OBJECT     "CAN_FD_MESSAGE_BUS"

/** Identifiers of the block parameters */
enum params { PARAM_MODULE_ID, PARAM_TSAMP, PARAM_COUNT };

enum outputs { OUT_FNC_CALL, OUT_MSG, OUT_COUNT };

static void mdlInitializeSizes(SimStruct *S) {
	DTypeId busId = INVALID_DTYPE_ID;

	if (!SetNumParams(S, PARAM_COUNT)) {
		return;
	}
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
		return;
	}
	if (!ssSetNumInputPorts(S, 0)) {
		return;
	}
	if (!ssSetNumOutputPorts(S, OUT_COUNT)) {
		return;
	}

	AddOutputPort(S, OUT_FNC_CALL, SS_FCN_CALL);

#ifdef MATLAB_MEX_FILE
	/* In a fresh MATLAB session the bus object is not there yet. Create it first, the way
	 * the CAN FD Pack block of the Vehicle Network Toolbox does: registering a type that
	 * does not exist is an error, not a result that can be checked. */
	mexEvalString("if ~evalin('base', 'exist(''CAN_FD_MESSAGE_BUS'', ''var'')'), canFDMessageBusType; end");
	ssRegisterTypeFromNamedObject(S, CANFD_BUS_OBJECT, &busId);
	if (busId == INVALID_DTYPE_ID) {
		ssSetErrorStatus(S, "The bus object CAN_FD_MESSAGE_BUS does not exist. "
							"It is made by canFDMessageBusType of the Vehicle Network Toolbox.");
		return;
	}
#endif
	AddOutputPort(S, OUT_MSG, busId);
	ssSetBusOutputObjectName(S, OUT_MSG, (void *)CANFD_BUS_OBJECT);
	ssSetBusOutputAsStruct(S, OUT_MSG, true);
	ssSetOutputPortBusMode(S, OUT_MSG, SL_BUS_MODE);

	/* Set standard options for this block */
	SetStandardOptions(S);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	const mxArray *tsamp = ssGetSFcnParam(S, PARAM_TSAMP);
	if (mxIsEmpty(tsamp)) return;
	ssSetNumSampleTimes(S, 1);
	ssSetSampleTime(S, 0, mxGetPr(tsamp)[0]);
	ssSetOffsetTime(S, 0, 0);
	ssSetCallSystemOutput(S, 0); /* function call on first output */
}

#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S) {
	if (!ssSetNumRunTimeParams(S, 1)) {
		return;
	}
	ssRegDlgParamAsRunTimeParam(S, PARAM_MODULE_ID, PARAM_MODULE_ID,
								PARAM_NAME_MODULE_ID, SS_INT8);
}
#endif

#define UNUSED_MDLCHECKPARAMETERS
#define UNUSED_MDLOUTPUTS
#define UNUSED_MDLTERMINATE
#include "../trailer.c"

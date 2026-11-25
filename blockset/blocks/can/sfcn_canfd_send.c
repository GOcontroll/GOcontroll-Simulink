/************************************************************************************//**
* \file         sfcn_canfd_send.c
* \brief        Simulink S-function of the CAN FD Send block: sends the CAN_FD_MESSAGE_BUS
*               on its input, as a classic CAN frame or as a CAN FD frame, on one CAN bus.
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
*   mex sfcn_canfd_send.c
****************************************************************************************/

#define S_FUNCTION_NAME sfcn_canfd_send

#include "../header.c"
#include "simstruc.h"

#define PARAM_NAME_MODULE_ID "module_id" /* CAN bus number starting at 1 */
#define CANFD_BUS_OBJECT     "CAN_FD_MESSAGE_BUS"

/** Identifier of the input */
enum input { IN_MSG, IN_COUNT };

/** Identifiers of the block parameters */
enum params { PARAM_MODULE_ID, PARAM_TSAMP, PARAM_COUNT };

static void mdlInitializeSizes(SimStruct *S) {
	DTypeId busId = INVALID_DTYPE_ID;

	if (!SetNumParams(S, PARAM_COUNT)) return;
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;
	if (!ssSetNumInputPorts(S, IN_COUNT)) return;

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
	AddInputPort(S, IN_MSG, busId);
	ssSetBusInputAsStruct(S, IN_MSG, true);
	ssSetInputPortBusMode(S, IN_MSG, SL_BUS_MODE);

	if (!ssSetNumOutputPorts(S, 0)) return;

	SetStandardOptions(S);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	const mxArray *tsamp = ssGetSFcnParam(S, PARAM_TSAMP);
	if (mxIsEmpty(tsamp)) return;
	ssSetNumSampleTimes(S, 1);
	ssSetSampleTime(S, 0, mxGetPr(tsamp)[0]);
	ssSetOffsetTime(S, 0, 0);
}

#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S) {
	if (!ssSetNumRunTimeParams(S, 1)) return;
	ssRegDlgParamAsRunTimeParam(S, PARAM_MODULE_ID, PARAM_MODULE_ID,
								PARAM_NAME_MODULE_ID, SS_INT8);
}
#endif

#define UNUSED_MDLCHECKPARAMETERS
#define UNUSED_MDLOUTPUTS
#define UNUSED_MDLTERMINATE
#include "../trailer.c"

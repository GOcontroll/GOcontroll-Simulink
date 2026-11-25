/**************************************************************************************
 * \file        sfcn_can_send_v2.c
 * \brief       C-MEX S-Function for CAN message transmission.
 *              For simulation this is a stub; code generation is handled by
 *              sfcn_can_send_v2.tlc.
 *
 *---------------------------------------------------------------------------------------
 *                          C O P Y R I G H T
 *---------------------------------------------------------------------------------------
 *  Copyright 2025 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
 *
 *---------------------------------------------------------------------------------------
 *                            L I C E N S E
 *---------------------------------------------------------------------------------------
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
 * Parameters
 * ----------
 *   module_id  : CAN bus index (1 = CAN1, 2 = CAN2)
 *   tsamp      : sample time [s]
 *
 * Ports
 * -----
 *   Input  1 : CAN_MESSAGE object (created with CAN Pack block)
 *   Output   : none
 *
 * Compile with (from the blockset/blocks directory):
 *   mex sfcn_can_send_v2.c
 **************************************************************************************/

#define S_FUNCTION_NAME sfcn_can_send_v2
#include <stdio.h>
#include <string.h>

#include "../header.c"
#include "simstruc.h"

#define PARAM_NAME_MODULE_ID "module_id"  /* CAN bus number starting at 1 */

/** Identifier of the input */
enum input { IN_MSG, IN_COUNT };

/** Identifiers of the block parameters */
enum params { PARAM_MODULE_ID, PARAM_TSAMP, PARAM_COUNT };

static void mdlInitializeSizes(SimStruct *S) {
	if (!SetNumParams(S, PARAM_COUNT)) return;
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;

	if (!ssSetNumInputPorts(S, IN_COUNT)) return;
	AddInputPort(S, IN_MSG, DYNAMICALLY_TYPED);

	if (!ssSetNumOutputPorts(S, 0)) return;

	ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
	SetStandardOptions(S);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	const mxArray *tsamp = ssGetSFcnParam(S, PARAM_TSAMP);
	if (mxIsEmpty(tsamp)) return;
	ssSetNumSampleTimes(S, 1);
	ssSetSampleTime(S, 0, mxGetPr(tsamp)[0]);
	ssSetOffsetTime(S, 0, 0);
}

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_DATA_TYPE
void mdlSetInputPortDataType(SimStruct *S, int_T port, DTypeId type) {
	if (port == IN_MSG) {
		/* Accept CAN_MESSAGE or CAN_MESSAGE_EXTENDED from Vehicle Network Toolbox */
		const char *typeName = ssGetDataTypeName(S, type);
		if (strcmp(typeName, "CAN_MESSAGE") == 0 ||
			strcmp(typeName, "CAN_MESSAGE_EXTENDED") == 0) {
			ssSetInputPortDataType(S, port, type);
		} else {
			static char msg[300];
			snprintf(msg, sizeof(msg),
					 "Unsupported data type '%s' on input port. "
					 "Expected CAN_MESSAGE or CAN_MESSAGE_EXTENDED.",
					 typeName);
			ssSetErrorStatus(S, msg);
		}
	}
}
#endif

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

/* Copyright (C) 2014 Czech Technical University in Prague
 *
 * Authors:
 *     - Michal Horn <hornmich@fel.cvut.cz>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File : sfunction_canreceive.c
 * Abstract:
 *     C-MEX S-function block for RPP CAN bus receive message.
 *
 * References:
 *     header.c
 *     trailer.c
 *
 * Compile with:
 *     mex(['-I' matlabroot '/toolbox/shared/can/src/scanutil'], ['-I' matlabroot '/toolbox/rtw/targets/common/can/datatypes'],  'sfunction_canreceive.c', [matlabroot '/toolbox/rtw/targets/common/can/datatypes/sfun_can_util.c'], [matlabroot '/toolbox/rtw/targets/common/can/datatypes/can_msg.c'])
 */


/* Adopted for use by GOcontroll 2024		http://www.gocontroll.com		All rights reserved
* \file			sfcn_can_receive.c
* \brief		matlab sfunction for receiving CAN messages on the Moduline Controllers
*/

#define S_FUNCTION_NAME sfcn_can_receive_v2

#include "header.c"
#include "sfun_can_util.h"
#include "simstruc.h"

#define PARAM_NAME_MODULE_ID	"module_id"

/** Identifiers of the block parameters */
enum params{
	PARAM_MODULE_ID,
	PARAM_TSAMP,
	PARAM_COUNT
};

enum outputs {
	OUT_FNC_CALL,
	OUT_MSG,
	OUT_COUNT
};

static void mdlInitializeSizes(SimStruct *S){

	CAN_Common_MdlInitSizes(S);

	/*DTypeId type;*/

	if(!SetNumParams(S, PARAM_COUNT)) {
		return;
	}

	/* No input ports */
	if(!ssSetNumInputPorts(S, 0)) {
		return;
	}
     
	/*
	 * Configure output ports:
	 *	- Received Message
	 *	- Message is received
	 */
	if(!ssSetNumOutputPorts(S, OUT_COUNT)) {
		return;
	}
	AddOutputPort(S, OUT_FNC_CALL, SS_FCN_CALL);

	AddOutputPort(S, OUT_MSG, ssGetDataTypeId(S, "CAN_MESSAGE"));

	ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
	/* Set standard options for this block */
	SetStandardOptions(S);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
	ssSetNumSampleTimes(S,1);
	ssSetSampleTime(S, 0, mxGetPr(ssGetSFcnParam(S, PARAM_TSAMP))[0]);
	ssSetOffsetTime(S, 0, 0);

    ssSetCallSystemOutput(S,0);  /* call on first element */
}


#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S){

	if(!ssSetNumRunTimeParams(S, 1)) {
    	return;
	}

	ssRegDlgParamAsRunTimeParam(S, PARAM_MODULE_ID,  PARAM_MODULE_ID, PARAM_NAME_MODULE_ID, SS_INT8);
}
#endif

#define UNUSED_MDLCHECKPARAMETERS
#define UNUSED_MDLOUTPUTS
#define UNUSED_MDLTERMINATE
#include "trailer.c"

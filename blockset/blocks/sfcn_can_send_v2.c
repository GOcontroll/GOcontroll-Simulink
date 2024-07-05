/* Copyright (C) 2013-2014 Czech Technical University in Prague
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
 * File : sfunction_cantransmit.c
 * Abstract:
 *     C-MEX S-function block for RPP CAN bus transmit message.
 *
 * References:
 *     header.c
 *     trailer.c
 *
 * Compile with:
 *     mex(['-I' matlabroot '/toolbox/shared/can/src/scanutil'], ['-I' matlabroot '/toolbox/rtw/targets/common/can/datatypes'],  'sfunction_cantransmit.c', [matlabroot '/toolbox/rtw/targets/common/can/datatypes/sfun_can_util.c'], [matlabroot '/toolbox/rtw/targets/common/can/datatypes/can_msg.c'])
 */

/*
%YAML 1.2
---
Name: CAN Transmit
Category: CAN
Header: rpp/can.h
Mnemonic: CANT

Inputs:
  - { name: "Msg", type: "dynamically" }

Outputs:

Parameters:
  - { name: "Module",                   type: "Choice", range: "CAN1, CAN2, CAN3"   }
  - { name: "Message ID type",          type: "Choice", range: "Standard, Extended" }
  - { name: "Message ID",               type: "uint16"                              }
  - { name: "Automatic mailbox number", type: "bool"                                }
  - { name: "Mailbox number",           type: "int8"                              }

# Description is in Markdown mark-up
Description: |

  This block allows to send a message to the CAN bus. It can be
  configured for any of the CAN ports (modules) CAN1, CAN2 or CAN3.

  The message data are read from `Msg` input port. The data type is
  decided automatically from the input, but it is restricted to uint8,
  uint16, uint32 and CAN\_MESSAGE. The CAN\_MESSAGE object can be
  created by the `CAN Pack` block.

  The message sent by the block will have an ID assigned according to
  the number in the *Message ID* parameter. The block supports both
  types of message IDs: Standard (11b) and Extended (29b). If
  CAN\_MESSAGE is used as input type, the message ID stored in
  CAN\_MESSAGE object is ignored and the ID from the parameter of this
  block is used instead.

  The mailbox number can be assigned automatically or manually. Automatic mailbox
  numbers are generated in ascending order from 0 to 31. Every mailbox must have a unique
  number. It is possible to mix blocks with automatically and manually
  assigned mailboxes. If the manually assigned mailbox number would
  collide with the automatic one then the automatically generated
  block will get assigned a next higher non-colliding ID.
  The mailbox numbers are shared between CAN Transmit and CAN Receive
  blocks with the same CAN port (module) parameter.

  In order to use this block, there must be a `CAN Configure` block in the model.

Status:
  Tested:
    - Transmission of the message with configured ID
    - Automatic generation of mailboxes numbers in combination with
      manual specification in other blocks. Colliding mailbox numbers
      are correctly handled.
    - Input message data type recognition
    - When unsupported data type is connected to the Msg input port, Simulink generates a reasonable error message
  Untested:
    - Handling of error states on CAN bus
  Not working:
    - External mode - throwing syntax error during compilation

RPP API functions used:
    - rpp_can_write()

Relevant demos:
    - cantransmit
    - can_demo
...
*/

/* Adopted for use by GOcontroll 2024		http://www.gocontroll.com		All rights reserved
* \file			sfcn_can_send_v2.c
* \brief		matlab sfunction for sending CAN messages on the Moduline Controllers
*/


#define S_FUNCTION_NAME sfcn_can_send_v2
#include "header.c"
#include <stdio.h>
#include "sfun_can_util.h"
#include "simstruc.h"

#define PARAM_NAME_MODULE_ID	"module_id" //CAN bus number starting at 1

/** Identifier of the input */
enum input {
	IN_MSG,
	IN_COUNT
};

/** Identifiers of the block parameters */
enum params{
	PARAM_MODULE_ID,
	PARAM_TSAMP,
	PARAM_COUNT
};

static void mdlInitializeSizes(SimStruct *S)
{

	CAN_Common_MdlInitSizes(S);

	if (!SetNumParams(S, PARAM_COUNT)) {
		return;
	}
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
        return;
    }

	if (!ssSetNumInputPorts(S, IN_COUNT)) {
		return;
	}

	AddInputPort(S, IN_MSG, DYNAMICALLY_TYPED);

	if (!ssSetNumOutputPorts(S, 0)) {
		return;
	}

	ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);
	/* Set standard options for this block */
	SetStandardOptions(S);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	ssSetNumSampleTimes(S,1);
	ssSetSampleTime(S, 0, mxGetPr(ssGetSFcnParam(S, PARAM_TSAMP))[0]);
	ssSetOffsetTime(S, 0, 0);
}

#if defined(MATLAB_MEX_FILE)
#define MDL_SET_INPUT_PORT_DATA_TYPE
void mdlSetInputPortDataType(SimStruct *S, int_T port, DTypeId type)
{
	if (port == IN_MSG) {
		if (/* CAN pack seems to use this data type, but it is
		     * not registered in sfun_can_util.c. Strange. */
		    type == ssGetDataTypeId(S, "CAN_MESSAGE") ||
		    type == ssGetDataTypeId(S, SL_CAN_STANDARD_FRAME_DTYPE_NAME) ||
		    type == ssGetDataTypeId(S, SL_CAN_EXTENDED_FRAME_DTYPE_NAME))
			ssSetInputPortDataType(S, port, type);
		else {
			static char msg[300];
			snprintf(msg, sizeof(msg), "Unsupported data type '%s' on Msg input port.",
				 ssGetDataTypeName(S, type));
			ssSetErrorStatus(S, msg);
		}
	}
}
#endif

#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct *S){

	if (!ssSetNumRunTimeParams(S, 1)) {
		return;
	}

	ssRegDlgParamAsRunTimeParam(S, PARAM_MODULE_ID,  PARAM_MODULE_ID, PARAM_NAME_MODULE_ID, SS_INT8); 
}
#endif

#define UNUSED_MDLCHECKPARAMETERS
#define UNUSED_MDLOUTPUTS
#define UNUSED_MDLTERMINATE
#include "trailer.c"
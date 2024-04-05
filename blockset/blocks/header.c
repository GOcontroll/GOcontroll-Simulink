/* Copyright (C) 2013 Czech Technical University in Prague
 *
 * Authors:
 *     - Carlos Jenkins <carlos@jenkins.co.cr>
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
 * File : header.c
 * Abstract:
 *     Common header for all RPP S-Functions.
 *     This file should be included at the beginning of a C-MEX S-Function block
 *     implementation after defining S_FUNCTION_NAME. It include refactored and
 *     commonly repeated structures that pollute S-Functions implementations
 *     (in conjunction with trailer.c). This file include basic includes,
 *     required definitions, macro definitions and documentation and commented
 *     prototypes for optional model calls/hooks.
 *
 * References:
 *     mdlInitializeSizes()       : sfunctions.pdf p. 441 [Required]
 *     mdlInitializeSampleTimes() : sfunctions.pdf p. 436 [Required]
 *     mdlOutputs()               : sfunctions.pdf p. 447 [Required]
 *     mdlTerminate()             : sfunctions.pdf p. 493 [Required]
 *
 *                          .: S-Function callback methods :.
 *
 *                              ***********************
 *                              * Start of simulation *
 *                              ***********************
 *                                         |
 *                        _               \_/
 *                       |      +--------------------+
 *                       |      | mdlInitializeSizes |
 *                       |      +--------------------+
 *      Initialization  -|                 |
 *                       |                \_/
 *                       |   +--------------------------+
 *                       |   | mdlInitializeSampleTimes |
 *                       |_  +--------------------------+
 *                                         |
 *                        _______________\ |
 *                       |               /\_/
 *                       |          +------------+
 *      Simulation loop -|          | mdlOutputs |
 *                       |          +------------+
 *                       |_________________|
 *                                         |
 *                                        \_/
 *                                 +--------------+
 *                                 | mdlTerminate |
 *                                 +--------------+
 *
 *     mdlCheckParameters()       : sfunctions.pdf p. 421 [Optional]
 *     mdlRTW()                   : sfunctions.pdf p. 458 [Optional]
 *     mdlSetWorkWidths()         : sfunctions.pdf p. 489 [Optional]
 *     mdlStart()                 : sfunctions.pdf p. 492 [Optional]
 *
 *     trailer.c
 */


#ifndef S_FUNCTION_NAME
#error 'Please include this file inside an S-Function implementation.'
#endif

#define S_FUNCTION_LEVEL 2


/*
 * Include C numerics library in order to use trunc() and other nice goodies.
 */
#include <math.h>

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"
#define EDIT_OK(S, P_IDX) \
 (!((ssGetSimMode(S)==SS_SIMMODE_SIZES_CALL_ONLY) && mxIsEmpty(ssGetSFcnParam(S, P_IDX))))
#define SAMPLE_TIME (ssGetSFcnParam(S, 1))


/* Function: isRealMatrix ======================================================
 * Abstract:
 *     Utility function to verify that the mxArray is a real (double) finite
 *     matrix.
 */
static bool isRealMatrix(const mxArray * const m)
{
    if (mxIsNumeric(m) && mxIsDouble(m) && !mxIsLogical(m) && !mxIsComplex(m) &&
       !mxIsSparse(m) && !mxIsEmpty(m) && mxGetNumberOfDimensions(m) == 2) {

        const double * const data = mxGetPr(m);
        const size_t numEl = mxGetNumberOfElements(m);
        size_t i;

        for (i = 0; i < numEl; i++) {
            if (!mxIsFinite(data[i])) {
                return(false);
            }
        }

        return(true);

    } else {

        return(false);

    }
}


/* Function: checkSampleTime ===================================================
 * Abstract:
 *     Utility function to verify that sample time (for variable sample time
 *     blocks) is valid.
 */
static bool checkSampleTime(SimStruct* S, int paramNum)
{
    if EDIT_OK(S, paramNum) {

        const real_T* sampleTime = NULL;
        const size_t stArraySize = mxGetM(SAMPLE_TIME) * mxGetN(SAMPLE_TIME);

        /* Sample time must be a real scalar value or 2 element array. */
        if (isRealMatrix(SAMPLE_TIME) &&
           (stArraySize == 1 || stArraySize == 2)) {

            sampleTime = (real_T*) mxGetPr(SAMPLE_TIME);

        } else {
            ssSetErrorStatus(S,
                "Invalid sample time. Sample time must be a "
                "real scalar value or an array of two real values.");
            return false;
        }

        if (sampleTime[0] < 0.0 && sampleTime[0] != -1.0) {
            ssSetErrorStatus(S,
                "Invalid sample time. Period must be non-negative or "
                "-1 (for inherited).");
            return false;
        }

        if (stArraySize == 2 && sampleTime[0] > 0.0 &&
           sampleTime[1] >= sampleTime[0]) {
            ssSetErrorStatus(S,
                "Invalid sample time. Offset must be smaller than period.");
            return false;
        }

        if (stArraySize == 2 && sampleTime[0] == -1.0 && sampleTime[1] != 0.0) {
            ssSetErrorStatus(S,
                "Invalid sample time. When period is -1, offset must be 0.");
            return false;
        }

        if (stArraySize == 2 && sampleTime[0] == 0.0 &&
           !(sampleTime[1] == 1.0)) {
            ssSetErrorStatus(S,
                "Invalid sample time. When period is 0, offset must be 1.");
            return false;
        }
    }
    return true;
}


/* Function: SetNumParams ===================================================
 * Abstract:
 *     Utility function to set the number of expected parameters, verify that
 *     the number of expected parameters is equal to the number of parameters
 *     entered in the dialog box return, and that the values entered are correct
 *     (by calling S-Function specific mdlCheckParameters() function).
 */
static void mdlCheckParameters(SimStruct* S);
static bool SetNumParams(SimStruct* S, int_T numParams)
{
    ssSetNumSFcnParams(S, numParams);

    #ifdef MATLAB_MEX_FILE
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {

        mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) {
            return false;
        }
    } else {
        /* Return if number of expected != number of actual parameters */
        return false;
    }
    #endif

    /* Set the parameters tunable status */
    int i;
    for (i = 0; i < numParams; i++) {
        ssSetSFcnParamTunable(S, i, false);
    }

    return true;
}


/* Function: AddInputPort ===================================================
 * Abstract:
 *     Utility function to add an standard single width input port.
 */
static bool AddInputPort(SimStruct* S, int_T port, DTypeId type)
{
    ssSetInputPortWidth(S, port, 1);
    ssSetInputPortDataType(S, port, type);
    ssSetInputPortComplexSignal(S, port, COMPLEX_NO);
    ssSetInputPortDirectFeedThrough(S, port, true);
    ssSetInputPortRequiredContiguous(S, port, true);
}

/* Function: AddInputVectorPort ===================================================
 * Abstract:
 *     Utility function to add an standard vector width input port.
 */
static bool AddInputVectorPort(SimStruct* S, int_T port, DTypeId type, int_T size)
{
    ssSetInputPortWidth(S, port, size);
    ssSetInputPortDataType(S, port, type);
    ssSetInputPortComplexSignal(S, port, COMPLEX_NO);
    ssSetInputPortDirectFeedThrough(S, port, true);
    ssSetInputPortRequiredContiguous(S, port, true);
}


/* Function: AddOutputPort ==================================================
 * Abstract:
 *     Utility function to add an standard single width output port.
 */
static bool AddOutputPort(SimStruct* S, int_T port, DTypeId type)
{
    ssSetOutputPortWidth(S, port, 1);
    ssSetOutputPortDataType(S, port, type);
    ssSetOutputPortComplexSignal(S, port, COMPLEX_NO);
}

/* Function: AddOutputVectorPort ==================================================
 * Abstract:
 *     Utility function to add an standard vector width output port.
 */
static bool AddOutputVectorPort(SimStruct* S, int_T port, DTypeId type, int_T size)
{
    ssSetOutputPortWidth(S, port, size);
    ssSetOutputPortDataType(S, port, type);
    ssSetOutputPortComplexSignal(S, port, COMPLEX_NO);
}


/* Function: SetStandardOptions =============================================
 * Abstract:
 *     Common / standard options for RPP blocks.
 */
static void SetStandardOptions(SimStruct* S)
{
    /* Specify the sim state compliance to be same as a built-in block */
    ssSetSimStateCompliance(S, USE_DEFAULT_SIM_STATE);

    /* Set the number of sample time. */
    ssSetNumSampleTimes(S, 1);

    /*
     * All options have the form SS_OPTION_<name> and are documented in
     * <matlabroot>/simulink/include/simstruc.h. The options should be
     * combined with bitwise OR as in
     *   ssSetOptions(S, (SS_OPTION_name1 | SS_OPTION_name2))
     */
    ssSetOptions(S,
        SS_OPTION_WORKS_WITH_CODE_REUSE |
        SS_OPTION_CAN_BE_CALLED_CONDITIONALLY |
        SS_OPTION_USE_TLC_WITH_ACCELERATOR);
}


/* Function: ValidParamRange ================================================
 * Abstract:
 *     Validate that given parameter is scalar, numeric and between given range.
 */
static bool ValidParamRange(SimStruct* S, int_T parIdx, int_T mn, int_T mx)
{
    const mxArray* raw = ssGetSFcnParam(S, parIdx);
    if (mxGetNumberOfElements(raw) != 1) {
        ssSetErrorStatus(S, "Parameter to S-function must be a scalar.");
        return false;
    }

    if (!mxIsDouble(raw)) {
        ssSetErrorStatus(S, "Parameter to S-function must be numeric.");
        return false;
    }

    double num = (mxGetPr(raw))[0];
    if (trunc(num) != num) {
        /* printf("Num      : %2.6f\n", num); */
        /* printf("Num trunc: %2.6f\n", trunc(num)); */
        ssSetErrorStatus(S, "Parameter to S-function must be an integer.");
        return false;
    }

    if ((num < mn) || (num > mx)) {
        ssSetErrorStatus(S, "Parameter to S-function must be "
                            "between specified range.");
        return false;
    }

    return true;
}


/* Function: ValidParamBoolean ==============================================
 * Abstract:
 *     Validate that given parameter is boolean (logical).
 */
static bool ValidParamBoolean(SimStruct* S, int_T parIdx)
{
    const mxArray* raw = ssGetSFcnParam(S, parIdx);
    if (!mxIsLogicalScalar(raw)) {
        ssSetErrorStatus(S, "Parameter to S-function must be a scalar.");
        return false;
    }

    return true;
}


/*
 *==============================================================================
 *=           BEGIN EXAMPLE CODE AND FUNCTIONS DOCUMENTATION                   =
 *==============================================================================
 */
#ifdef NEVER_DEFINED


/* Function: mdlCheckParameters ================================================
 * Abstract:
 *     mdlCheckParameters verifies new parameter settings whenever parameter
 *     change or are re-evaluated during a simulation. When a simulation is
 *     running, changes to S-function parameters can occur at any time during
 *     the simulation loop.
 *
 *     See http://www.mathworks.com/help/simulink/sfg/mdlcheckparameters.html
 */
#ifdef MATLAB_MEX_FILE
#define MDL_CHECK_PARAMETERS
static void mdlCheckParameters(SimStruct* S)
{
    /* Check the parameter 1 */
    if (!ValidParamRange(S, 0, 1, 20)) {
        return;
    }
}
#endif


/* Function: mdlRTW ============================================================
 * Abstract:
 *     This function is called when the Real-Time Workshop is generating
 *     the model.rtw file.
 *
 *     See http://www.mathworks.com/help/simulink/sfg/mdlrtw.html
 */
#ifdef MATLAB_MEX_FILE
#define MDL_RTW
static void mdlRTW(SimStruct* S)
{
    UNUSED_PARAMETER(S);
}
#endif


/* Function: mdlSetWorkWidths ==================================================
 * Abstract:
 *     The optional method, mdlSetWorkWidths is called after input port
 *     width, output port width, and sample times of the S-function have
 *     been determined to set any state and work vector sizes which are
 *     a function of the input, output, and/or sample times.
 *
 *     Run-time parameters are registered in this method using methods
 *     ssSetNumRunTimeParams, ssSetRunTimeParamInfo, and related methods.
 *
 *     See http://www.mathworks.com/help/simulink/sfg/mdlsetworkwidths.html
 */
#ifdef MATLAB_MEX_FILE
#define MDL_SET_WORK_WIDTHS
static void mdlSetWorkWidths(SimStruct* S)
{
    /* Set number of run-time parameters */
    if (!ssSetNumRunTimeParams(S, 1)) {
        return;
    }

    /* Register the run-time parameter 1 */
    ssRegDlgParamAsRunTimeParam(S, 0, 0, "p1", SS_UINT8);
}
#endif


/* Function: mdlStart ==========================================================
 * Abstract:
 *     This function is called once at start of model execution. If you
 *     have states that should be initialized once, this is the place
 *     to do it.
 *
 *     See http://www.mathworks.com/help/simulink/sfg/mdlstart.html
 */
#define MDL_START
static void mdlStart(SimStruct* S)
{
    UNUSED_PARAMETER(S);
}

#endif /* NEVER_DEFINED */

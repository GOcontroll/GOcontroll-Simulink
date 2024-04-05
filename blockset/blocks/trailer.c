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
 * File : trailer.c
 * Abstract:
 *     Common trailer for all RPP S-Functions.
 *
 * References:
 *     header.c
 */

#ifndef S_FUNCTION_NAME
#error 'Please include this file inside an S-Function implementation.'
#endif

/* Function: mdlInitializeSizes ================================================
 * Abstract:
 *     The sizes information is used by Simulink to determine the S-function
 *     block's characteristics (number of inputs, outputs, states, etc.).
 */
#ifdef UNUSED_MDLINITIALIZESIZES
static void mdlInitializeSizes(SimStruct *S)
{
    UNUSED_PARAMETER(S);
}
#endif

/* Function: mdlInitializeSampleTimes ==========================================
 * Abstract:
 *     This function is used to specify the sample time(s) for your
 *     S-function. You must register the same number of sample times as
 *     specified in ssSetNumSampleTimes.
 */
#ifdef UNUSED_MDLINITIALIZESAMPLETIMES
static void mdlInitializeSampleTimes(SimStruct *S)
{
    UNUSED_PARAMETER(S);
}
#endif

#ifdef COMMON_MDLINITIALIZESAMPLETIMES_INHERIT
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    #if defined(ssSetModelReferenceSampleTimeDefaultInheritance)
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
    #endif
}
#endif

/* Function: mdlOutputs ========================================================
 * Abstract:
 *     In this function, you compute the outputs of your S-function
 *     block. Generally outputs are placed in the output vector(s),
 *     ssGetOutputPortSignal.
 */
#ifdef UNUSED_MDLOUTPUTS
static void mdlOutputs(SimStruct *S, int_T tid)
{
    UNUSED_PARAMETER(S);
    UNUSED_PARAMETER(tid);
}
#endif

/* Function: mdlTerminate ======================================================
 * Abstract:
 *     In this function, you should perform any actions that are necessary
 *     at the termination of a simulation.
 */
#ifdef UNUSED_MDLTERMINATE
static void mdlTerminate(SimStruct *S)
{
    UNUSED_PARAMETER(S);
}
#endif

/* Function: mdlCheckParameters ================================================
 * Abstract:
 *     mdlCheckParameters verifies new parameter settings whenever parameter
 *     change or are re-evaluated during a simulation. When a simulation is
 *     running, changes to S-function parameters can occur at any time during
 *     the simulation loop.
 *
 *     Note: this an optional function for S-Function, in contrast to the ones
 *           above. This is here just because header.c declares
 *           checkParametersMismatch() that uses this function in order to
 *           refactor that commonly used block of code.
 */
#ifdef UNUSED_MDLCHECKPARAMETERS
static void mdlCheckParameters(SimStruct *S)
{
    UNUSED_PARAMETER(S);
}
#endif

/*
 * Required S-function trailer
 */
#ifdef MATLAB_MEX_FILE
# include "simulink.c"
#else
# include "cg_sfun.h"
#endif
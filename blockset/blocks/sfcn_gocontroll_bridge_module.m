%%***************************************************************************************
%% file         sfcn_gocontroll_bridge_module.m
%% brief        Level-2 M file S-Function for GOcontroll bridge module.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
%%
%%---------------------------------------------------------------------------------------
%%                            L I C E N S E
%%---------------------------------------------------------------------------------------
%% Permission is hereby granted, free of charge, to any person obtaining a copy of this
%% software and associated documentation files (the "Software"), to deal in the Software
%% without restriction, including without limitation the rights to use, copy, modify, merge,
%% publish, distribute, sublicense, and/or sell copies of the Software, and to permit
%% persons to whom the Software is furnished to do so, subject to the following conditions:
%%
%% The above copyright notice and this permission notice shall be included in all copies or
%% substantial portions of the Software.
%%
%% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
%% INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
%% PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
%% FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
%% OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
%% DEALINGS IN THE SOFTWARE.
%%
%%***************************************************************************************
function sfcn_gocontroll_bridge_module(block)
  setup(block);
%endfunction


%% Function: setup ===================================================
%% Abstract:
%%   Set up the S-function block's basic characteristics such as:
%%   - Input ports
%%   - Output ports
%%   - Dialog parameters
%%   - Options
%%
%%   Required         : Yes
%%   C-Mex counterpart: mdlInitializeSizes

%% DatatypeID's
%% DOUBLE  =  0
%% SINGLE  =  1
%% INT8    =  2
%% UINT8   =  3
%% INT16   =  4
%% UINT16  =  5
%% INT32   =  6
%% UINT32  =  7
%% BOOLEAN =  8

function setup(block)
	tsamp = 1;
	%% Register number of input and output ports
	block.NumInputPorts = 2;
	block.NumOutputPorts = 0;
	%% configurable input module channel 1
	block.InputPort(1).Dimensions = 1;
	block.InputPort(1).DatatypeID = 5; %% int32 is type 6, see rtwtypes.h
	block.InputPort(1).Complexity = 'Real';
	block.InputPort(1).DirectFeedthrough = false;  %% We will not use the direct (.Data) value of the input to calculate the direct (.Data) value of the output
	block.InputPort(1).SamplingMode = 'sample';
	%% configurable input module channel 2
	block.InputPort(2).Dimensions = 1;
	block.InputPort(2).DatatypeID = 5; %% int32 is type 6, see rtwtypes.h
	block.InputPort(2).Complexity = 'Real';
	block.InputPort(2).DirectFeedthrough = false;  %% We will not use the direct (.Data) value of the input to calculate the direct (.Data) value of the output
	block.InputPort(2).SamplingMode = 'sample';

	% Number of S-Function parameters expected

	% (tsamp, moduleSlot, C1func, C1freq, C2func, C2freq)
	block.NumDialogPrms     = 6;
	block.SampleTimes = [block.DialogPrm(tsamp).Data 0];
	%% -----------------------------------------------------------------
	%% Register methods called at run-time
	%% -----------------------------------------------------------------

	%%
	%% Start:
	%%   Functionality    : Called in order to initialize state and work
	%%                      area values
	%%   C-Mex counterpart: mdlStart
	%%
	block.RegBlockMethod('Start', @Start);

	%%
	%% Outputs:
	%%   Functionality    : Called to generate block outputs in
	%%                      simulation step
	%%   C-Mex counterpart: mdlOutputs
	%%
	block.RegBlockMethod('Outputs', @Outputs);

	%%
	%% Update:
	%%   Functionality    : Called to update discrete states
	%%                      during simulation step
	%%   C-Mex counterpart: mdlUpdate
	%%
	block.RegBlockMethod('Update', @Update);

	block.RegBlockMethod('WriteRTW', @WriteRTW);
%endfunction

function Start(block)

  %% No start

%endfunction


function Outputs(block)

  %% No output

%endfunction


function Update(block)

  %% No update

%endfunction

function Terminate(block)

  %% No Terminate

%endfunction

function WriteRTW(block)
	moduleSlot = 2;
	C1func = 3;
	C1freq = 4;
	C2func = 5;
	C2freq = 6;

	block.WriteRTWParam('string', 'moduleSlot', num2str(block.DialogPrm(moduleSlot).Data));
	block.WriteRTWParam('string', 'C1func', num2str(block.DialogPrm(C1func).Data));
	block.WriteRTWParam('string', 'C1freq', num2str(block.DialogPrm(C1freq).Data));
	block.WriteRTWParam('string', 'C2func', num2str(block.DialogPrm(C2func).Data));
	block.WriteRTWParam('string', 'C2freq', num2str(block.DialogPrm(C2freq).Data));


%%******************************* end of sfcn_gocontroll_bridge_module.m **********************

%%***************************************************************************************
%% file         sfcn_gocontroll_bridge_module_monitor.m
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
function sfcn_gocontroll_bridge_module_monitor(block)
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
	block.NumInputPorts = 0;
	block.NumOutputPorts = 4;
	%% Module Temperature
	block.OutputPort(1).Dimensions = 1;
	block.OutputPort(1).DatatypeID = 4; %% int16 is type 4, see rtwtypes.h
	block.OutputPort(1).Complexity = 'Real';
	block.OutputPort(1).SamplingMode = 'sample';
	%% Module groundshift
	block.OutputPort(2).Dimensions = 1;
	block.OutputPort(2).DatatypeID = 5; %% uint16 is type 5, see rtwtypes.h
	block.OutputPort(2).Complexity = 'Real';
	block.OutputPort(2).SamplingMode = 'sample';
	%% Output channel 1
	block.OutputPort(3).Dimensions = 1;
	block.OutputPort(3).DatatypeID = 4; %% int16 is type 4, see rtwtypes.h
	block.OutputPort(3).Complexity = 'Real';
	block.OutputPort(3).SamplingMode = 'sample';
	%% Output channel 2
	block.OutputPort(4).Dimensions = 1;
	block.OutputPort(4).DatatypeID = 4; %% int16 is type 4, see rtwtypes.h
	block.OutputPort(4).Complexity = 'Real';
	block.OutputPort(4).SamplingMode = 'sample';
	% Number of S-Function parameters expected
	% (tsamp, canBus, canID)
	block.NumDialogPrms     = 2;
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

function WriteRTW(block)
	moduleSlot = 2;

	block.WriteRTWParam('string', 'moduleSlot', num2str(block.DialogPrm(moduleSlot).Data));

%%******************************* end of sfcn_gocontroll_bridge_module_monitor.m **********************

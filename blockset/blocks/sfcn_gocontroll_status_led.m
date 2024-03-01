%%***************************************************************************************
%% file         sfcn_gocontroll_status_led.m
%% brief        Level-2 M file S-Function for controlling the enclosure LEDs.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com   All rights reserved
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
%% endinternal
%%
%%***************************************************************************************
function sfcn_gocontroll_status_led(block)
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

%Our data type parameters:
%2 int8
%3 uint8
%4 int16
%5 uint16
%6 int32
%7 uint32
%8 boolean

function setup(block)
  %% Register number of input and output ports
  block.NumInputPorts = 1;
  block.NumOutputPorts = 0;

  block.InputPort(1).Dimensions = 1;
  block.InputPort(1).DatatypeID = 3;
  block.InputPort(1).Complexity = 'Real';
  block.InputPort(1).DirectFeedthrough = false;
  block.InputPort(1).SamplingMode = 'sample';

  % Number of S-Function parameters expected
  block.NumDialogPrms = 2;

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

    %%
  %% Terminate:
  %%   Functionality    : Called to terminate discrete states
  %%                      during simulation termination
  %%   C-Mex counterpart: mdlTerminate
  %%
  block.RegBlockMethod('Terminate', @Terminate);

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
	led = 1;
	color = 2;

	block.WriteRTWParam('string', 'led', num2str(block.DialogPrm(led).Data));
	block.WriteRTWParam('string', 'color', num2str(block.DialogPrm(color).Data));

%%******************************* end of sfcn_gocontroll_status_led.m **********************

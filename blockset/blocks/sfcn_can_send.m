%%***************************************************************************************
%% file         sfcn_can_send.m
%% brief        Level-2 M file S-Function for CAN message transmission
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2013 (c) by HAN Automotive     http://www.han.nl     All rights reserved
%%  Copyright 2024 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
%%
%%---------------------------------------------------------------------------------------
%%                            L I C E N S E
%%---------------------------------------------------------------------------------------
%% This file is part of the HANcoder Target Matlab/Simulink Blockset environment. For the
%% licensing terms, please contact HAN Automotive.
%%
%% This software has been carefully tested, but is not guaranteed for any particular
%% purpose. HAN Automotive does not offer any warranties and does not guarantee the
%% accuracy, adequacy, or completeness of the software and is not responsible for any
%% errors or omissions or the results obtained from use of the software.
%%
%%***************************************************************************************
function sfcn_can_send(block)
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
function setup(block)
	number_bytes = block.DialogPrm(4).Data;
  %% Register number of input and output ports
  block.NumInputPorts = 1+number_bytes;
  block.NumOutputPorts = 0;

  %% 'ID' port
  block.InputPort(1).Dimensions = 1;
  block.InputPort(1).DatatypeID = 7; %% uint32_T is type 7, see rtwtypes.h
  block.InputPort(1).Complexity = 'Real';
  block.InputPort(1).DirectFeedthrough = false;  %% We will not use the direct (.Data) value of the input to calculate the direct (.Data) value of the output
  block.InputPort(1).SamplingMode = 'sample';

  %% [sw] rest poorten van 2 t/m 9 ipv 1 t/m 8
  %% Override ports
  for inputCounter = 2:number_bytes+1
      block.InputPort(inputCounter).Dimensions = 1;
      block.InputPort(inputCounter).DatatypeID = block.DialogPrm(5).Data+1; %% user specified data type
      block.InputPort(inputCounter).Complexity = 'Real';
      block.InputPort(inputCounter).DirectFeedthrough = false;  %% We will not use the direct (.Data) value of the input to calculate the direct (.Data) value of the output
      block.InputPort(inputCounter).SamplingMode = 'sample';
  end

  % Number of S-Function parameters expected
  % (tsamp, canBus, frameType, inputNumber, dataType, byteOrder, RTR)
  block.NumDialogPrms     = 7;

  block.SampleTimes = [block.DialogPrm(1).Data 0];
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


%%******************************* end of sfcn_can_send.m ********************************

%%***************************************************************************************
%% file         sfcn_exec_shell.m
%% brief        Level-2 M file S-Function for executing a shell command
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2024 (c) by GOcontroll     http://www.gocontroll.com     All rights reserved
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
function sfcn_exec_shell(block)
	setup(block);
end

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
	%% Register number of input and output ports
	block.NumInputPorts = 1;
	block.NumOutputPorts = 0;

	%% configurable input module channel 1
	addSimpleInput(block, 1, DatatypeID.Boolean);

	%% Number of S-Function parameters expected
	tsamp = 1;

	block.NumDialogPrms     = 1;
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
end

function Start(~)

  %% No start

end


function Outputs(~)

  %% No output

end


function Update(~)

  %% No update

end

%%******************************* end of sfcn_exec_shell.m *****************************
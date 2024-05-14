%%***************************************************************************************
%% file         sfcn_can_receive.m
%% brief        Level-2 M file S-Function for CAN message reception
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
function sfcn_can_receive(block)
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
	number_bytes = block.DialogPrm(3).Data; %outputNumber
	%% Register number of input and output ports
	block.NumInputPorts = 1;
	block.NumOutputPorts = 1 + number_bytes;
	%% 'ID' port
	addSimpleInput(block, 1, DatatypeID.Uint32);
	%% 'new' port
	addSimpleOutput(block, 1, DatatypeID.Boolean);
	%% rest poorten van 2 t/m 9 ipv 1 t/m 8
	%% Override ports
	for outputCounter = 2:number_bytes+1
		addSimpleOutput(block, outputCounter, block.DialogPrm(4).Data+1) % dataType
	end

	% Number of S-Function parameters expected
	% (tsamp, canBus, outputNumber, dataType, byteOrder)
	block.NumDialogPrms     = 5;
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


%%******************************* end of sfcn_can_receive.m *****************************

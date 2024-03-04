%%***************************************************************************************
%% file         sfcn_UDPConfig.m
%% brief        Level-2 M file S-Function for writing a value to memory
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
function sfcn_UDPConfig(block)
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
  %% Register number of input and output ports
  block.NumInputPorts = 0;
  block.NumOutputPorts = 0;

  % Number of S-Function parameters expected

  % (tsamp, buffer_length, port, ip, broadcast)
  block.NumDialogPrms     = 3;
  block.SampleTimes = [1 0];
  %% -----------------------------------------------------------------
  %% Register methods called at run-time
  %% -----------------------------------------------------------------

  block.RegBlockMethod('Start', @Start);

  block.RegBlockMethod('Outputs', @Outputs);

  block.RegBlockMethod('Update', @Update);

  block.RegBlockMethod('Terminate', @Terminate);

  block.RegBlockMethod('WriteRTW', @WriteRTW);
end

function Start(~)
end


function Outputs(~)
end


function Update(~)
end

function Terminate(~)
end

function WriteRTW(block)
	port = 1;
	ip = 2;
	broadcast = 3;
	block.WriteRTWParam('string', 'port', num2str(block.DialogPrm(port).Data));
	block.WriteRTWParam('string', 'ip', ['"' block.DialogPrm(ip).Data '"']);
	block.WriteRTWParam('string', 'broadcast' , num2str(block.DialogPrm(broadcast).Data));
end

%%******************************* end of sfcn_UDPConfig.m **********************

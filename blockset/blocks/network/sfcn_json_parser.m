%%***************************************************************************************
%% file         sfcn_json_parser.m
%% brief        Level-2 M-file S-Function for the JSON Parser block.
%%              Parses a JSON object string received from an MQTT Subscribe block and
%%              outputs the numeric value for each configured key as a separate port.
%%
%%              Input ports  : 3 (fixed)
%%                Port 1 - uint8  scalar   : new_data flag (from MQTT Subscribe port 1)
%%                Port 2 - uint16 scalar   : payload length (from MQTT Subscribe port 2)
%%                Port 3 - uint8  array    : payload bytes  (from MQTT Subscribe port 3)
%%
%%              Output ports : N (variable, 1–8) - one real_T (double) per JSON key.
%%                Outputs retain their previous value when the key is absent from the
%%                incoming JSON message.
%%
%%              Dialog parameters (in order, must match Parameters field):
%%                1 : num_signals  - number of output signals / JSON keys (integer 1–8)
%%                2 : tsamp        - sample time in seconds
%%                3 : buf_size     - internal parse buffer size (should match MQTT
%%                                   Subscribe payload_size; default 256)
%%
%%              RTWdata fields (set by the mask Initialization callback, consumed by TLC):
%%                num_signals  - string copy of num_signals
%%                buf_size     - string copy of buf_size
%%                key_0 .. key_(N-1) - JSON key names to search for (0-indexed for TLC)
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2025 (c) by GOcontroll      http://www.gocontroll.com     All rights reserved
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
function sfcn_json_parser(block)
	setup(block);
end

%% Function: setup ===================================================
function setup(block)
	n        = block.DialogPrm(1).Data;   %% num_signals

	block.NumInputPorts  = 3;
	block.NumOutputPorts = n;

	%% Port 1 in: new_data flag (uint8 scalar) - from MQTT Subscribe output 1
	block.InputPort(1).DatatypeID        = 3;    %% uint8
	block.InputPort(1).Complexity        = 'Real';
	block.InputPort(1).Dimensions        = 1;
	block.InputPort(1).DirectFeedthrough = true;

	%% Port 2 in: payload length (uint16 scalar) - from MQTT Subscribe output 2
	block.InputPort(2).DatatypeID        = 5;    %% uint16
	block.InputPort(2).Complexity        = 'Real';
	block.InputPort(2).Dimensions        = 1;
	block.InputPort(2).DirectFeedthrough = true;

	%% Port 3 in: payload bytes (uint8 array) - from MQTT Subscribe output 3
	%% Width is inherited from the connected MQTT Subscribe payload_size.
	block.InputPort(3).DatatypeID        = 3;    %% uint8
	block.InputPort(3).Complexity        = 'Real';
	block.InputPort(3).Dimensions        = -1;   %% inherited
	block.InputPort(3).DirectFeedthrough = true;

	%% N output ports - one real_T (double) scalar per JSON key
	for i = 1:n
		block.OutputPort(i).DatatypeID  = 0;     %% real_T (double)
		block.OutputPort(i).Complexity  = 'Real';
		block.OutputPort(i).Dimensions  = 1;
	end

	block.NumDialogPrms = 3;
	block.SampleTimes   = [block.DialogPrm(2).Data 0];

	block.RegBlockMethod('SetInputPortDimensions', @SetInpPortDims);
	block.RegBlockMethod('Outputs', @Outputs);
end

%% Propagate inherited dimensions for port 3.
function SetInpPortDims(block, idx, di)
	block.InputPort(idx).Dimensions = di;
end

%% Simulation output - returns zeros (STM only; actual code generated by TLC).
function Outputs(block)
	n = block.DialogPrm(1).Data;
	for i = 1:n
		block.OutputPort(i).Data = double(0);
	end
end

%%******************************* end of sfcn_json_parser.m ****************************

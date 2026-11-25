%%***************************************************************************************
%% file         sfcn_MQTT_broker.m
%% brief        Level-2 M file S-Function for the MQTT Broker configuration block.
%%              STM target only. Parameters are supplied via the Simulink mask
%%              (RTWdata struct); no S-function dialog parameters are used.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%% Copyright 2025 (c) by GOcontroll http://www.gocontroll.com All rights reserved
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
%%***************************************************************************************
function sfcn_MQTT_broker(block)
	setup(block);
end


%% Function: setup ===================================================
%% Abstract:
%%   Set up the S-function block's basic characteristics.
%%   All broker parameters (url, port, user, pass, client_id, keep_alive)
%%   are supplied by the Simulink mask via set_param(gcb, 'RTWdata', struct(...)).
%%
%%   Required         : Yes
%%   C-Mex counterpart: mdlInitializeSizes
function setup(block)
	block.NumInputPorts  = 0;
	block.NumOutputPorts = 0;

	%% All parameters come from the mask RTWdata - no dialog params needed.
	%% Expected RTWdata fields (set by the mask initialisation callback):
	%%   apn      : APN string         (e.g. 'internet')
	%%   sim_pin  : SIM PIN string     ('' = the PIN set on the controller)
	%%   broker   : broker URL string  ('' = the broker set on the controller)
	%%   port     : broker port        (0 = the port set on the controller)
	%%   user     : username string    ('' = user + password of the controller)
	%%   password : password string    (used together with user)
	%%   client   : client ID string   ('' = the controller serial number)
	%%   ttl      : keep-alive in seconds (e.g. 60)
	block.NumDialogPrms  = 0;
	block.SampleTimes    = [1 0];

	block.RegBlockMethod('Start', @Start);
end

function Start(~)
	%% No simulation-time behaviour needed; code generation is handled by TLC.
end

%%******************************* end of sfcn_MQTT_broker.m *****************************

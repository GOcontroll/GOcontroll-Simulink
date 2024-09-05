%%***************************************************************************************
%% file         ManualUpload.m
%% brief        Allows calling the upload function manually.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2019 (c) by HAN Automotive     http://www.han.nl          All rights reserved
%%  Copyrigth 2024 (c) by GOcontroll B.V.    http://www.gocontroll.com  All rights reserved
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
%%
%% This file is a modified version of the ert_linux_target_upload.m file found in the
%% blockset folder to make it manually callable. It was modified by GOcontroll.
%%
%% Usage:
%% ManualUpload('modelname', 'address', port='port');
%%
%% port is an optional argument, it can be left out, by default port = 8001
%%
%%***************************************************************************************

function ManualUpload(modelname, ip, options)
arguments
	modelname {mustBeTextScalar}
	ip {mustBeTextScalar}
	options.port {mustBeTextScalar} = '8001'
end

elfPath = [modelname '.elf'];
a2lPath = [modelname '.a2l'];

% Upload the file to the controller
if (isfile(elfPath))
	cmdCommand = strcat('curl --connect-timeout 2 -i -X POST -H "Content-Type: multipart/form-data"',' -F "elfFile=@',elfPath,'" ',' http://',ip,':',options.port,'/upload');
	disp(cmdCommand)
	system(cmdCommand);
else
	error(['Could not find the elf that belongs with ', modelname]);
end
if (isfile(a2lPath))
	cmdCommand = strcat('curl --connect-timeout 2 -i -X POST -H "Content-Type: multipart/form-data"',' -F "a2lFile=@',a2lPath,'" ',' http://',ip,':',options.port,'/upload');
	disp(cmdCommand)
	system(cmdCommand);
elseif (~isfile(a2lPath))
	error(['Could not find the a2l file that belongs with ', modelname]);
end
end % end of function ManualUpload()

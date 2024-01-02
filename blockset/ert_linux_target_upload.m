%%***************************************************************************************
%% file         ert_linux_target_upload.m
%% brief        Allows calling the flashtool automatically.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2019 (c) by HAN Automotive    http://www.han.nl           All rights reserved
%%  Copyright 2024 (c) by GOcontroll        http://www.gocontroll.com   All rights reserved
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
%% This file has been modified by GOcontroll to support Linux host machines and timeout
%% When there is no target to upload to.
%%
%%***************************************************************************************
%% Automatic flash script
function ert_linux_target_upload(modelName)
% find settings block
AutoUpload = get_param(modelName,'tlcAutoUpload');

	%Only upload file when Auto upload setting is switched on
    if (strcmp(AutoUpload,'Auto upload'))
	 disp('### Starting automatic flash procedure');
     UploadModelToTarget(modelName);
    else

    end
end % end of function ert_linux_target_upload()


function UploadModelToTarget(modelName)
ModelPath = [pwd filesep '..' filesep bdroot(modelName) '.elf'];
a2lPath = [pwd filesep '..' filesep bdroot(modelName) '.a2l'];
UploadAddress = get_param(modelName,'tlcXcpTcpAddress');
UploadPort = num2str(get_param(modelName,'tlcUploadPort'));
% Upload the file to the controller
cmdCommand = strcat('curl --connect-timeout 2 -i -X POST -H "Content-Type: multipart/form-data"',' -F "elfFile=@',ModelPath,'" ',' http://',UploadAddress,':',UploadPort,'/upload');
disp(cmdCommand)
system(cmdCommand);
cmdCommand = strcat('curl --connect-timeout 2 -i -X POST -H "Content-Type: multipart/form-data"',' -F "a2lFile=@',a2lPath,'" ',' http://',UploadAddress,':',UploadPort,'/upload');
disp(cmdCommand)
system(cmdCommand);

end % end of function ert_linux_target_upload()

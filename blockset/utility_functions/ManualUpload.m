%%***************************************************************************************
%% file         ManualUpload.m
%% brief        Uploads a built Linux application and its a2l file to a controller.
%%
%%---------------------------------------------------------------------------------------
%%                          C O P Y R I G H T
%%---------------------------------------------------------------------------------------
%%  Copyright 2026 (c) by GOcontroll     http://www.gocontroll.com    All rights reserved
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

function ManualUpload(modelname, ip, options)
% MANUALUPLOAD Upload a model and its a2l file to a Linux controller (Moduline M1, L4, HMI1)
%   that runs the go-upload-server.
%
%   MANUALUPLOAD(MODEL, ADDRESS) uploads the newest build of MODEL to ADDRESS, port 8001.
%   The build is taken from the folder build/ next to the model (or in the current folder),
%   where a build leaves <MODEL>_<major>.<minor>.<patch>.elf and .a2l.
%
%   MANUALUPLOAD(MODEL, ADDRESS, port=PORT) uses another port.
%
%   MANUALUPLOAD(MODEL, ADDRESS, remoteName=NAME) stores the files on the controller as
%   NAME.elf and NAME.a2l. The default is MODEL, without the version: the go-simulink
%   service starts the one elf in /usr/simulink, so the name must not change from build
%   to build. The Node-RED nodes for Simulink signals look for GOcontroll_Linux.elf; use
%   remoteName="GOcontroll_Linux" when the model has another name and you use those nodes.
%
%   Command form:  ManualUpload MyModel 192.168.19.85
%
%   The Moduline S1 is not programmed this way.

arguments
	modelname {mustBeTextScalar}
	ip {mustBeTextScalar}
	options.port {mustBeTextScalar} = '8001'
	options.remoteName {mustBeTextScalar} = ''
end

modelname = char(modelname); ip = char(ip);
port = char(options.port); remoteName = char(options.remoteName);
if isempty(remoteName)
	remoteName = modelname;
end

% A model that is open tells us its folder and its platform
searchDirs = {fullfile(pwd, 'build'), pwd};
if bdIsLoaded(modelname)
	try
		target = get_param(getActiveConfigSet(modelname), 'tlcGOcontrollTarget');
		if strcmp(go_target_platform(target), 'STM')
			error('ManualUpload:notLinux', ...
				['''%s'' is built for the %s. ManualUpload is for the Linux controllers ' ...
				 '(Moduline M1, L4, HMI1) only.'], modelname, target);
		end
	catch ME
		if strcmp(ME.identifier, 'ManualUpload:notLinux'), rethrow(ME); end
	end
	modelDir = fileparts(get_param(modelname, 'FileName'));
	if ~isempty(modelDir)
		searchDirs = [{fullfile(modelDir, 'build')}, searchDirs];
	end
end

elfPath = i_newest(searchDirs, modelname, '.elf');
if isempty(elfPath)
	error('ManualUpload:noElf', ...
		'Could not find the elf that belongs with %s. Build the model first; looked in: %s', ...
		modelname, strjoin(unique(searchDirs, 'stable'), ', '));
end
% the a2l of the same build sits next to the elf under the same name
a2lPath = [elfPath(1:end-4) '.a2l'];
if ~isfile(a2lPath)
	error('ManualUpload:noA2l', 'Could not find the a2l file that belongs with %s (%s).', modelname, a2lPath);
end

url = ['http://' ip ':' port '/upload'];
fprintf('### Uploading %s to %s as %s.elf\n', elfPath, url, remoteName);
i_post(url, 'elfFile', elfPath, [remoteName '.elf']);
fprintf('### Uploading %s to %s as %s.a2l\n', a2lPath, url, remoteName);
i_post(url, 'a2lFile', a2lPath, [remoteName '.a2l']);
fprintf('### Upload of %s finished\n', modelname);

end % end of function ManualUpload()


function file = i_newest(dirs, modelname, ext)
% The newest <model>_<version><ext> or <model><ext> in the first folder that has one.
	file = '';
	for k = 1:numel(dirs)
		found = [dir(fullfile(dirs{k}, [modelname '_*' ext])); dir(fullfile(dirs{k}, [modelname ext]))];
		% keep only <model>_<digits and dots>: another model's name may start the same way
		keep = arrayfun(@(f) ~isempty(regexp(f.name, ['^' regexptranslate('escape', modelname) '(_[\d.]+)?' regexptranslate('escape', ext) '$'], 'once')), found);
		found = found(keep);
		if ~isempty(found)
			[~, newest] = max([found.datenum]);
			file = fullfile(found(newest).folder, found(newest).name);
			return
		end
	end
end

function i_post(url, field, localFile, remoteFile)
% One multipart upload with curl; the file is sent under remoteFile.
	cmd = sprintf('curl --connect-timeout 2 --silent --show-error --fail -X POST -F "%s=@%s;filename=%s" %s', ...
		field, strrep(localFile, '\', '/'), remoteFile, url);
	[status, out] = system(cmd);
	if status ~= 0
		error('ManualUpload:uploadFailed', ...
			['Upload to %s failed (curl exit code %d): %s\nCheck the address, that the controller is ' ...
			 'reachable and that the go-upload-server service runs on it.'], url, status, strtrim(out));
	end
	if ~isempty(strtrim(out))
		disp(strtrim(out));
	end
end

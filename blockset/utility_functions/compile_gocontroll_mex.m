%get the path of this script so we can compile the mexes using it.
mfilePath = mfilename('fullpath');
if contains(mfilePath,'LiveEditorEvaluationHelper')
    mfilePath = matlab.desktop.editor.getActiveFilename;
end
%remove the filename from the end
[path, ~, ~] = fileparts(mfilePath);

%if gocontroll_mex_version doesn't exist or the result of it is false, recompile the mex files
if ~(exist('gocontroll_mex_version', "file") == 3) || ~gocontroll_mex_version()
	disp("Compiling c mex functions...");
	version = ['-DVERSION=''"' ert_linux_target_version() '"'''];
	include_scanutil = ['-I' fullfile(matlabroot,'toolbox','shared','can','src','scanutil')];
	include_can_data = ['-I' fullfile(matlabroot, 'toolbox', 'rtw', 'targets', 'common', 'can', 'datatypes')];
	can_msg = fullfile(matlabroot, 'toolbox', 'rtw', 'targets', 'common', 'can', 'datatypes', 'can_msg.c');
	can_util = fullfile(matlabroot, 'toolbox', 'rtw', 'targets', 'common', 'can', 'datatypes', 'sfun_can_util.c');
	% compile mex files
	d = dir(fullfile(path, '..', 'blocks'));
	files = {d.name};
	for idx = 1:length(files)
		name = char(files(1,idx));
		if contains(name, ".c") && contains(name, "sfcn")
			[~, fname, ~] = fileparts(name);
			%compile the level 2 S functions
			fprintf("\nCompiling %s\n", fname);
			mex(include_scanutil, include_can_data, fullfile(path, '..', 'blocks', [fname '.c']), can_msg, can_util, '-outdir', fullfile(path, '..', 'blocks'));
			fprintf("Compiled %s\n", fname);
		end
	end
	% compile version mex last so on failure it doesnt exist and prevent recompilation
	fprintf("\nCompiling gocontroll_mex_version\n");
	mex(fullfile(path, '..', 'blocks', 'gocontroll_mex_version.c'), '-outdir', fullfile(path, '..', 'blocks'), version);
	disp("Finished compiling c mex functions!");
	clear version include_scanutil include_can_data can_msg can_util d files idx name fname
end
clear mfilePath path 
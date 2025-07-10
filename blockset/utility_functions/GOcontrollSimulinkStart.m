%check if the toolbox is installed and enabled

try
	%make sure the toolbox is enabled so the compiler paths can be taken from it
	if ~matlab.addons.isAddonEnabled('GOcontroll-Simulink')
		matlab.addons.enableAddon('GOcontroll-Simulink');
	end
catch
	%The addon has to be installed to use its compiler(s)
	error("The GOcontroll-Simulink toolbox must be installed to properly load this project, please install it first");
end

%correct the compiler paths in the getInstallationLocation.mlx file for the computer opening this project

%get the path of this script so we can compile the mexes using it.
mfilePath = mfilename('fullpath');
if contains(mfilePath,'LiveEditorEvaluationHelper')
    mfilePath = matlab.desktop.editor.getActiveFilename;
end
%remove the filename from the end
[path, ~, ~] = fileparts(mfilePath);
root = fullfile(path, '..', '..');
%'Rename' the function so it no longer takes precedence over the one that is in the toolbox
movefile(fullfile(root, '+GOcontroll_Simulink_2023b_dev'), fullfile(root, 'temp'));
%Convert the getInstallationLocation file to .m so we can read/edit it
matlab.internal.liveeditor.openAndConvert(fullfile(root, 'temp', 'getInstallationLocation.mlx'), fullfile(pwd, 'temp', 'temp.m'));
%Get the toolbox compiler paths
try
	zig_x86 = GOcontroll_Simulink_2023b_dev.getInstallationLocation('Zig-x86');
catch
	zig_x86 = '';
end
try
	zig_aarch64 = GOcontroll_Simulink_2023b_dev.getInstallationLocation('Zig-aarch64');
catch
	zig_aarch64 = '';
end
try
	gnu = GOcontroll_Simulink_2023b_dev.getInstallationLocation('aarch64-none-linux-gnu-gcc');
catch
	gnu = '';
end
%Move the function back on the path
movefile(fullfile(root, 'temp'), fullfile(root, '+GOcontroll_Simulink_2023b_dev'));
%Create the new table
new = sprintf("'aarch64-none-linux-gnu-gcc', '%s';'Zig-aarch64', '%s';'Zig-x86','%s'", gnu, zig_aarch64, zig_x86);
%Read the previously created .m file
file = fileread(fullfile(root, '+GOcontroll_Simulink_2023b_dev', 'temp.m'));
%Replace the table with the new one
new_file = replaceBetween(file, '{', '}', new);
%Open the .m file for writing, write the new contents and close it
file = fopen(fullfile(root, '+GOcontroll_Simulink_2023b_dev', 'temp.m'), 'w');
fwrite(file, new_file);
fclose(file);
%Convert the .m file back to a .mlx so it works with the toolbox
matlab.internal.liveeditor.openAndSave(fullfile(root, '+GOcontroll_Simulink_2023b_dev', 'temp.m'), fullfile(pwd, '+GOcontroll_Simulink_2023b_dev', 'getInstallationLocation.mlx'));
%Delete the temporary .m file
delete(fullfile(root, '+GOcontroll_Simulink_2023b_dev', 'temp.m'));

%disable the toolbox so the project environment is used instead

matlab.addons.disableAddon("GOcontroll-Simulink");
%cleanup
clear file gnu new new_file zig_aarch64 zig_x86 root path mfilePath
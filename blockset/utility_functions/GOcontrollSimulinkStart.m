try
    %make sure the toolbox is enabled so the compiler paths can be taken from it
    if ~matlab.addons.isAddonEnabled('GOcontroll-Simulink')
        matlab.addons.enableAddon('GOcontroll-Simulink');
    end
catch
    %The addon has to be installed to use its compiler(s)
    error("The GOcontroll-Simulink toolbox must be installed to properly load this project, please install it first");
end

%'Rename' the function so it no longer takes precedence over the one that is in the toolbox
movefile('+GOcontroll_Simulink_2023b_dev', 'temp');
%Convert the getInstallationLocation file to .m so we can read/edit it
matlab.internal.liveeditor.openAndConvert(fullfile(pwd, 'temp', 'getInstallationLocation.mlx'), fullfile(pwd, 'temp', 'temp.m'));
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
movefile('temp','+GOcontroll_Simulink_2023b_dev');
%Create the new table
new = sprintf("'aarch64-none-linux-gnu-gcc', '%s';'Zig-aarch64', '%s';'Zig-x86','%s'", gnu, zig_aarch64, zig_x86);
%Read the previously created .m file
file = fileread(fullfile(pwd, '+GOcontroll_Simulink_2023b_dev', 'temp.m'));
%Replace the table with the new one
new_file = replaceBetween(file, '{', '}', new);
%Open the .m file for writing, write the new contents and close it
file = fopen(fullfile(pwd, '+GOcontroll_Simulink_2023b_dev', 'temp.m'), 'w');
fwrite(file, new_file);
fclose(file);
%Convert the .m file back to a .mlx so it works with the toolbox
matlab.internal.liveeditor.openAndSave(fullfile(pwd, '+GOcontroll_Simulink_2023b_dev', 'temp.m'), fullfile(pwd, '+GOcontroll_Simulink_2023b_dev', 'getInstallationLocation.mlx'));
%Delete the temporary .m file
delete(fullfile(pwd, '+GOcontroll_Simulink_2023b_dev', 'temp.m'));
%Disable the toolbox
matlab.addons.disableAddon("GOcontroll-Simulink");
%cleanup
clear file gnu new new_file zig_aarch64 zig_x86
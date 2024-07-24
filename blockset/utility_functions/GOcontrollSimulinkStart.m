if ~matlab.addons.isAddonEnabled('GOcontroll-Simulink')
	matlab.addons.enableAddon('GOcontroll-Simulink');
end

movefile('+GOcontroll_Simulink_2023b_dev', 'temp');
matlab.internal.liveeditor.openAndConvert(fullfile(pwd, 'temp', 'getInstallationLocation.mlx'), fullfile(pwd, 'temp', 'temp.m'));
file = fileread(fullfile(pwd, 'temp', 'temp.m'));
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
new = sprintf("'aarch64-none-linux-gnu-gcc', '%s';'Zig-aarch64', '%s';'Zig-x86','%s'", gnu, zig_aarch64, zig_x86);
new_file = replaceBetween(file, '{', '}', new);
file = fopen(fullfile(pwd, 'temp', 'temp.m'), 'w');
fwrite(file, new_file);
fclose(file);
matlab.internal.liveeditor.openAndSave(fullfile(pwd, 'temp', 'temp.m'), fullfile(pwd, 'temp', 'getInstallationLocation.mlx'));
delete(fullfile(pwd, 'temp', 'temp.m'));
movefile('temp','+GOcontroll_Simulink_2023b_dev');
matlab.addons.disableAddon("GOcontroll-Simulink");

clear file gnu new new_file zig_aarch64 zig_x86
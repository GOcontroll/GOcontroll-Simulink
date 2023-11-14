%% Executing this script automatically compiles the System_Config_*s.
%% It will open the simulink model, load the configuration from the file, compile it and then move the products to their folder in the builds directory.

tic

OS = computer();

load_system("GOcontroll_Linux");
set_param("GOcontroll_Linux","tlcAutoUpload", "Do not upload");

if isfolder('builds')
	rmdir('builds','s');
end
mkdir('builds');

c = dir("System_Config_*");
configs = {c.name};
for i = 1:length(configs)
	filename = char(configs(1,i));
	% configname = split(split(filename, "\.")(1,1), "_")(1,3);
	extension_split = split(filename, ".");
	name_split = split(extension_split(1,1),"_");
	configname = char(name_split(3,1));
	fprintf("\nCompiling the %s config...\n\n",configname);
	if OS=="GLNXA64"
		config_path = sprintf("%s/%s", pwd, filename);
		dest_path = sprintf("builds/%s", configname);
	else
		config_path = sprintf("%s\\%s",pwd, filename);
		dest_path = sprintf("builds\\%s", configname);
	end
	run(config_path);
	rtwbuild('GOcontroll_Linux');
	mkdir(dest_path);
	copyfile('GOcontroll_Linux.elf', dest_path);
	copyfile('GOcontroll_Linux.a2l', dest_path);
end

toc
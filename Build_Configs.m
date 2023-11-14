%% Executing this script automatically compiles the System_Config_*s.
%% It will open the simulink model, load the configuration from the file, compile it and then move the products to their folder in the builds directory.

tic % start timer

load_system("GOcontroll_Linux"); % Open the Simulink model
set_param("GOcontroll_Linux","tlcAutoUpload", "Do not upload"); % disable auto uploading

if isfolder('builds') % if there already is a builds directory
	rmdir('builds','s'); % delete it and its contents
end
mkdir('builds'); % remake the builds folder

c = dir("System_Config_*"); % gather all the files matching System_Config_*
configs = {c.name}; % get the file names
for i = 1:length(configs) 
	filename = char(configs(1,i)); % turn the filename into a string
	extension_split = split(filename, "."); % extract the configuration name without System_Config_ and .m, split of the file extension
	name_split = split(extension_split(1,1),"_"); % take the first index with is the full filename without extension and then split at _
	configname = char(join(name_split(3:end,1), "_")); % take every element from the 3rd element (matlab counts from 1 instead of 0)

	fprintf("\nCompiling the %s config...\n\n",configname); % print some info

	config_path = [pwd filesep filename]; % find the full path of the config file for the run function to use
	dest_path = ['builds' filesep configname]; % find the destination path where the .elf and .a2l will be put

	run(config_path); % execute the configuration
	rtwbuild('GOcontroll_Linux'); % compile the model
	mkdir(dest_path); % make the destination folder
	copyfile('GOcontroll_Linux.elf', dest_path); % copy the compilation products
	copyfile('GOcontroll_Linux.a2l', dest_path);
end

toc % print the elapsed time
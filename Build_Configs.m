%% Executing this script automatically compiles the System_Config_*s.
%% It will open the simulink model, load the configuration from the file, compile it and then move the products to their folder in the builds directory.

tic % start timer
model_name = "GOcontroll_Linux";

model = load_system(model_name); % Open the Simulink model
set_param(model,"tlcAutoUpload", "Do not upload"); % disable auto uploading

core_info = evalc('feature(''numcores'')');
core_info_split = split(core_info, " ");
num_cores = core_info_split{12};

fprintf("\nDetected %s logical cores\n",num_cores);

set_param(model, "MakeCommand", ['make_rtw -j' num_cores]); % set the core count in the make command for faster compilation

clear num_cores core_info core_info_split

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
	rtwbuild(model); % compile the model
	mkdir(dest_path); % make the destination folder
	copyfile(strcat(model_name, '.elf'), dest_path); % copy the compilation products
	copyfile(strcat(model_name, '.a2l'), dest_path);
end

clear c configs i filename extension_split name_split configname config_path dest_path model_name ans model

toc % print the elapsed time
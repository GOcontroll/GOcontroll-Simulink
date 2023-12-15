if usejava('desktop') % check if the current session uses a gui, or is a matlab cli session
	list = {'Host', 'Client',};
	[indx,tf] = listdlg('PromptString',{'Select a configuration to load:',''},...
		'SelectionMode','single','OKString','Select','ListString',list);
	run(sprintf('System_Config_%s',list{indx}));
end

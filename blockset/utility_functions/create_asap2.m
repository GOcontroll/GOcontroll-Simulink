function create_asap2(modelName, XCPport, XCPaddress, XCPstationID, Target, MAPfile, medium)
	% CREATE_ASAP2 Generate an a2l file from the given inputs
	%
	% CREATE_ASAP2(modelName, XCPport, XCPaddress, XCPstationID, Target, MAPfile)
	%
	% parameters:
	% modelname     The name of the simulink model to generate an a2l from
	% XCPport       The XCP TCP port the xcp server listens on
	% XCPaddress    The XCP TCP ip the xcp server listens on
	% XCPstationID  The binary identifier used to check if the current a2l matches the xcp server
	% Target        Compile target string for example 'i.MX8'
	% MAPfile       The path to the .map file generated by the compilation
	%
	% sources:
	% <a href="matlab:web('https://nl.mathworks.com/help/rtw/ref/coder.asap2.getecudescriptions.html')">getEcuDescriptions</a>
	% <a href="matlab:web('https://nl.mathworks.com/help/rtw/ref/coder.asap2.export.html')">coder.asap2.export</a>
	% <a href="matlab:web('https://nl.mathworks.com/help/rtw/ug/edit_template.html')">a2l base class</a>
	fprintf('### Generating ASAP2 file\n');
	%Get the XCPstationIDAddress
	if exist(MAPfile, 'file')
		MAPfileString = fileread(MAPfile);
	else
		MAPfileString = '';
		warning('Could not find .map file, you can safely ignore this warning when using the Zig compiler');
	end
	if strcmp(Target, 'i.MX8')
		XCPstationIDAddress = regexp(MAPfileString, '\n\s+0x([0-9a-fA-F]){16}\s+uniqueIdString+','tokens');
	else
		XCPstationIDAddress = regexp(MAPfileString, '\n\s+0x([0-9a-fA-F]){8}\s+uniqueIdString+','tokens');
	end
	if ~isempty(XCPstationIDAddress)
		XCPstationIDAddress = ['0x', XCPstationIDAddress{1}{1}];
	else
		XCPstationIDAddress = '0x0000';
	end
	base = GOcontrollASAP2(XCPport, XCPaddress, XCPstationID, XCPstationIDAddress, Target, medium);
	ecuDesc = coder.asap2.getEcuDescriptions(modelName, Version='1.71', IncludeDefaultEventList=false, MapFile=['..' filesep modelName '.elf'], CustomizationObject=base);
	measurements = find(ecuDesc, 'Measurement');
	for i = 1:length(measurements)
		if contains(measurements(i),modelName)
			%remove all the junk elements that matlab adds to the a2l, all this junk seems to start with the modelname so that makes it easier.
			delete(ecuDesc, 'Measurement', measurements(i));
		end
	end
	coder.asap2.export(modelName, CustomEcuDescriptions=ecuDesc);
	fprintf('### Created %s.a2l\n',modelName);
end
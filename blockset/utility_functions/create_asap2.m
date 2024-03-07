%https://nl.mathworks.com/help/rtw/ref/coder.asap2.getecudescriptions.html
%https://nl.mathworks.com/help/rtw/ref/coder.asap2.export.html
%https://nl.mathworks.com/help/rtw/ug/edit_template.html
%%generate asap2 file using base class and ecu description.
function create_asap2(modelName, XCPport, XCPaddress, XCPstationID, Target, MAPfile)
	fprintf('### Generating ASAP2 file');
	%Get the XCPstationIDAddress
	if exist(MAPfile, 'file')
		MAPfileString = fileread(MAPfile);
	else
		DAStudio.error('Could not find .map file');
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
	base = GOcontrollASAP2(XCPport, XCPaddress, XCPstationID, XCPstationIDAddress, Target);
	ecuDesc = coder.asap2.getEcuDescriptions(modelName, Version='1.71', IncludeDefaultEventList=false, MapFile=['..' filesep modelName '.elf'], CustomizationObject=base);
	measurements = find(ecuDesc, 'Measurement');
	for i = 1:length(measurements)
		if contains(measurements(i),modelName)
			%remove all the junk elements that matlab adds to the a2l, all this junk seems to start with the modelname so that makes it easier.
			delete(ecuDesc, 'Measurement', measurements(i));
		end
	end
	coder.asap2.export(modelName, CustomEcuDescriptions=ecuDesc);
end
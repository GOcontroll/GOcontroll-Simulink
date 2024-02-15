function create_asap2(modelName)
	fprintf('### Generating ASAP2 file');
	ecuDesc = coder.asap2.getEcuDescriptions(modelName, Version='1.71', IncludeDefaultEventList=false, MapFile=['..' filesep modelName '.elf']);
	measurements = find(ecuDesc, 'Measurement');
	for i = 1:length(measurements)
		if contains(measurements(i),modelName)
			delete(ecuDesc, 'Measurement', measurements(i));
		end
	end
	coder.asap2.export(modelName, CustomEcuDescriptions=ecuDesc);
end
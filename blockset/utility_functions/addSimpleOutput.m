function addSimpleOutput(block, outputNum, datatype)
	% ADDSIMPLEOUTPUT Add a simple scalar output to the passed block
	%
	% Usage:
	%
	% ADDSIMPLEOUTPUT(block, outputNum, datatype) sets up the outputNum on block with the datatype
	block.OutputPort(outputNum).Dimensions = 1;
	block.OutputPort(outputNum).DatatypeID = double(datatype);
	block.OutputPort(outputNum).Complexity = 'Real';
	block.OutputPort(outputNum).SamplingMode = 'sample';
end
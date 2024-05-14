function addSimpleInput(block, inputNum, datatype)
	% ADDSIMPLEINPUT Add a simple scalar input to the passed block
	%
	% Usage:
	%
	% ADDSIMPLEINPUT(block, inputNum, datatype) sets up the inputNum on block with the datatype
	block.InputPort(inputNum).Dimensions = 1;
	block.InputPort(inputNum).DatatypeID = double(datatype);
	block.InputPort(inputNum).Complexity = 'Real';
	block.InputPort(inputNum).DirectFeedthrough = false;
	block.InputPort(inputNum).SamplingMode = 'sample';
end
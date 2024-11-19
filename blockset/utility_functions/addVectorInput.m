function addVectorInput(block, inputNum, datatype, dimensions)
% ADDVECTORINPUT Add a vector input to the passed block
%
% Usage:
%
% ADDVECTORINPUT(block, inputNum, datatype,dimensions) sets up the inputNum on block with the datatype and dimensions
block.InputPort(inputNum).Dimensions = dimensions;
block.InputPort(inputNum).DatatypeID = double(datatype);
block.InputPort(inputNum).Complexity = 'Real';
block.InputPort(inputNum).DirectFeedthrough = false;
block.InputPort(inputNum).SamplingMode = 'sample';
end
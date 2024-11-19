function addVectorOutput(block, outputNum, datatype,dimensions)
% ADDVECTOROUTPUT Add a vector output to the passed block
%
% Usage:
%
% ADDVECTOROUTPUT(block, outputNum, datatype, dimensions) sets up the outputNum on block with the datatype and dimensions
block.OutputPort(outputNum).Dimensions = dimensions;
block.OutputPort(outputNum).DatatypeID = double(datatype);
block.OutputPort(outputNum).Complexity = 'Real';
block.OutputPort(outputNum).SamplingMode = 'sample';
end
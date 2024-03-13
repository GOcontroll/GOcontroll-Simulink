classdef DatatypeID < double
	% DatatypeID
	% This class holds an enumeration that can be used to get datatypeIDs without having to just write a number
	% Simply write DatatypeID.Int8
	%
	% Note: this value will be of the Class DatatypeID, if you need to use this number somewhere you might have to use double(DatatypeID.Int8)
	% 
	% In an s-function you can add an output like this:
	% addSimpleOutput(block, OutputNum, DatatypeID.Uint32);
	%
	% source: edit([matlabroot,'/simulink/include/sl_types_def.h']);
	enumeration
		% DatatypeID's
		Double		(0)
		Single		(1)
		Int8		(2)
		Uint8		(3)
		Int16		(4)
		Uint16		(5)
		Int32		(6)
		Uint32		(7)
		Boolean		(8)
	end
end
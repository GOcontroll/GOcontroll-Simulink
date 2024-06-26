%https://nl.mathworks.com/help/rtw/ug/edit_template.html
%%base class for adding some custom stuff to the generated .a2l file
classdef GOcontrollASAP2 < coder.asap2.UserCustomizeBase 
	properties
		XCPport
		XCPaddress
		XCPstationId
		XCPstationIdAddress
		Target
		Medium
	end
	methods
		function obj = GOcontrollASAP2(XCPport, XCPaddress, XCPstationId, XCPstationIdAddress, Target, Medium)

			obj = obj@coder.asap2.UserCustomizeBase();
			obj.XCPport = XCPport;
			obj.XCPaddress = XCPaddress;
			obj.XCPstationId = XCPstationId;
			obj.XCPstationIdAddress = XCPstationIdAddress;
			obj.Target = Target;
			obj.Medium = Medium;
		end

		function hardwareInterface = writeHardwareInterface(obj)
			hardwareInterface = sprintf([...
				'\t/begin MOD_PAR "' 'MOD PAR Comment Goes Here' '"\n',...
				'\t\tADDR_EPK\t\t' obj.XCPstationIdAddress '\n',...
				'\t\tCPU_TYPE\t\t"' obj.Target '"\n',...
				'\t\tECU\t\t\t\t"' obj.Target '"\n',...
				'\t\tEPK\t\t\t\t"' obj.XCPstationId '"\n',...
				'\t\tSYSTEM_CONSTANT\t"XcpStationID string length"  "' int2str(length(obj.XCPstationId)) '"\n',...
				'\t\tSYSTEM_CONSTANT\t"RAM-FlashOffset"  "0"\n',...
				'\t/end MOD_PAR\n\n',...
				'\t/begin IF_DATA XCP\n',...
				'\t\t/begin XCP_ON_' obj.Medium '_IP\n',...
				'\t\t\t0x100\n',...
				'\t\t\t' int2str(obj.XCPport) '\n',...
				'\t\t\t"HOST_NAME" ' obj.Target '\n',...
				'\t\t\t"ADDRESS" ' obj.XCPaddress '\n',...
				'\t\t/end XCP_ON_' obj.Medium '_IP\n',...
				'\t/end IF_DATA XCP\n',...
				'\n',...
				'\t/begin MOD_COMMON "'  'Mod Common Comment Here' '"\n',...
				'\t\t' obj.ByteOrder '\n',...
				'\t/end MOD_COMMON'...
				]);
		end
	end
end
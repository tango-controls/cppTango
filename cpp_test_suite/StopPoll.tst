sequence ServSleep label "Configure server to test polling API"
{
	cpp user manu on device devtestmem
	{
		new ("dserver/devtest/api");
		
		command_inout("RemObjPolling",["dev/test/10","command","IOPollStr1"]);
		command_inout("RemObjPolling",["dev/test/10","command","IOPollArray2"]);
		command_inout("RemObjPolling",["dev/test/10","command","IOExcept"]);
		command_inout("RemObjPolling",["dev/test/10","command","State"]);
		command_inout("RemObjPolling",["dev/test/10","command","Status"]);
		
		command_inout("RemObjPolling",["dev/test/10","attribute","PollLong_attr"]);
		command_inout("RemObjPolling",["dev/test/10","attribute","PollString_spec_attr"]);
		command_inout("RemObjPolling",["dev/test/10","attribute","attr_wrong_type"]);
		
		command_inout("RemObjPolling",["dev/test/10","attribute","Long64_attr_rw"]);
		command_inout("RemObjPolling",["dev/test/10","attribute","ULong_spec_attr_rw"]);
		command_inout("RemObjPolling",["dev/test/10","attribute","ULong64_attr_rw"]);
		command_inout("RemObjPolling",["dev/test/10","attribute","State_spec_attr_rw"]);
		command_inout("RemObjPolling",["dev/test/10","attribute","Encoded_attr"]);
		
		delete();
	}
}

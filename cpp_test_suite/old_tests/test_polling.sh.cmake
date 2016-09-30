#!/usr/bin/env bash
check_return_value () {
if [ $1 != "0" ]
then
	echo "Polling Test Suite has FAILED !!!!!!!!!!!!!!!!!!"
	echo "Leaving test suite, try to fix it"
	date
	sleep 120
	date
	./StopPoll @DEV1@
	exit -1
fi
}

./Poll @DEV1@
ret=$?
check_return_value $ret

sleep 7

echo "\nTesting command_history and attribute_history DeviceProxy methods"
./read_hist @DEV1@
ret=$?
check_return_value $ret

echo "\nTesting polling of new attribute data types"
./poll_attr @DEV1@
ret=$?
check_return_value $ret

echo "\nTesting all DeviceProxy polling methods"
./poll_met @DEV1@ @DEV2@ @DEV1_ALIAS@
ret=$?
check_return_value $ret

echo "\nTesting polling threads pool"
./poll_pool @DEV1@ @DEV2@ @INST_NAME@
ret=$?
check_return_value $ret

./StopPoll @DEV1@
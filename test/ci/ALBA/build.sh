#!/bin/bash

# Variables to be defined in the node property file:
#
# MAKE_PATH
# OS_TYPE
# OS_SPEC
# CPU_BUS
# OBJS_DIR
# OBJS_DIR_SL
# CLEAN
#
# OMNI_BASE
# LOG4TANGO_BASE
# ZMQ_BASE


# Import node specific settings
if [ ! -z "$NODE_NAME" -a -f "$NODE_NAME" ]
then
	source "$NODE_NAME"
else
	echo "The settings file for the node $NODE_NAME does not exist!"
	echo "Create ci/$INSTITUTE/$NODE_NAME file."
	exit 1
fi


# Get Mr Jenkins job name (here: TangoLib)
JENKINS_JOB=${JOB_NAME%%\/*}


# Define directory where Tango Library will be installed
INSTALL_DIR="/siciliarep/jenkins/$NODE_NAME/jobs/$JENKINS_JOB"


# Set library type
if [ $LIBTYPE = "shared" ]
then
	LIBNAME=libtango.so
else
	LIBNAME=libtango.a
fi


# Set debug type
if [ $DEBUGMODE = "debug" ]
then
	DEBUG_MODE="debug=1"
fi


# Build library
MAKE_CMD="$MAKE_PATH INSTALL_BASE=$INSTALL_DIR $OS_SPEC $CPU_BUS $DEBUG_MODE OBJS_DIR=$OBJS_DIR OBJS_DIR_SL=$OBJS_DIR_SL OMNI_BASE=$OMNI_BASE LOG4TANGO_BASE=$LOG4TANGO_BASE ZMQ_BASE=$ZMQ_BASE $CLEAN $LIBNAME install_include install_link"
echo $MAKE_CMD
cd ../..
$MAKE_CMD

exit $?

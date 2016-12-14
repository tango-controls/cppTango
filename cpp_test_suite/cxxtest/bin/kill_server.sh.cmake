#!/bin/bash

kill_servers(){
    PIDS=`ps -e | grep DevTest | grep -v grep | awk '{print $1}'`
    echo "PIDS = $PIDS"
    for pid in $PIDS
        do
        echo "Killing process with PID $pid"
        kill $pid
    done

    sleep 2
}

kill_servers
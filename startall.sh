#!/bin/bash

# Function to display usage instructions
usage() {
    echo "Usage: $0 [start|stop]"
    echo "  start - Start the processes"
    echo "  stop  - Stop the processes"
    exit 1
}

# Function to start the processes
start_processes() {
    ./coordinator -p 9090 &
    ./tsd -c 1 -s 1 -h localhost -k 9090 -p 3010 &
    ./tsd -c 2 -s 2 -h localhost -k 9090 -p 3011 &
    ./tsd -c 3 -s 3 -h localhost -k 9090 -p 3012 &
    echo "Processes started."
}

# Function to stop the processes
stop_processes() {
    pids=$(pgrep -f "coordinator|tsd")  # Find process IDs
    if [ -n "$pids" ]; then
        echo "Stopping processes..."
        kill $pids
        echo "Processes stopped."
    else
        echo "No processes to stop."
    fi
}

# Check for user input
if [ $# -ne 1 ]; then
    usage
fi

case $1 in
    "start")
        start_processes
        ;;
    "stop")
        stop_processes
        ;;
    *)
        usage
        ;;
esac

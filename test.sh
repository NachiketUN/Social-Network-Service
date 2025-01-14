#!/bin/bash

# Declare an associative array to store process names and PIDs
declare -A process_map

# Define an array with your commands and corresponding process names
commands=(
  "./coordinator -p 9090"
  "./tsd -c 1 -s 1 -h localhost -k 9090 -p 10000"
  "./synchronizer -h localhost -k 9090 -p 10002 -i 1"
  "./tsd -c 2 -s 1 -h localhost -k 9090 -p 10003"
  "./synchronizer -h localhost -k 9090 -p 10005 -i 2"
)

# Function to run a command in a new tab and store its PID and name
run_command() {
  local name="$1"
  local command="$2"
  gnome-terminal --tab --title="$name" -- bash -c "$command; bash" &
  local pid=$!
  if [ $? -eq 0 ]; then
    process_map["$name"]=$pid
  else
    echo "Error starting command '$command'."
  fi
}

# Run the first command with a delay of 5 seconds
process_name="coordinator"
run_command "$process_name" "${commands[0]}"
sleep 2  # Introduce a 5-second delay

# Run the remaining commands
for ((i=1; i<${#commands[@]}; i++)); do
  process_name="server$i"
  run_command "$process_name" "${commands[i]}"
  sleep 3
done

# Optionally, you can wait for all processes to finish
for process_name in "${!process_map[@]}"; do
  wait "${process_map[$process_name]}"
done
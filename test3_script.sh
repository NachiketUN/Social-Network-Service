#!/bin/bash

# Declare an associative array to store process names and PIDs
declare -A process_map

# Define an array with your commands and corresponding process names
commands=(
  "./coordinator -p 9090"
  "./tsd -c 1 -s 1 -h localhost -k 9090 -p 10000"
  "./tsd -c 1 -s 2 -h localhost -k 9090 -p 10001"
  "./synchronizer -h localhost -k 9090 -p 9001 -i 1"
  "./tsd -c 2 -s 1 -h localhost -k 9090 -p 20000"
  "./tsd -c 2 -s 2 -h localhost -k 9090 -p 20001"
  "./synchronizer -h localhost -k 9090 -p 9002 -i 2"
  "./tsd -c 3 -s 1 -h localhost -k 9090 -p 30001"
  "./tsd -c 3 -s 2 -h localhost -k 9090 -p 30002"
  "./synchronizer -h localhost -k 9090 -p 9003 -i 3"
  "./tsc -h localhost -k 9090 -u 1"
  "./tsc -h localhost -k 9090 -u 2"
  "./tsc -h localhost -k 9090 -u 3"
)

cluster1=(
  "./tsd -c 1 -s 1 -h localhost -k 9090 -p 10000"
  "./tsd -c 1 -s 2 -h localhost -k 9090 -p 10001"
  "./synchronizer -h localhost -k 9090 -p 9001 -i 1"
)

cluster2=(
  "./tsd -c 2 -s 1 -h localhost -k 9090 -p 20000"
  "./tsd -c 2 -s 2 -h localhost -k 9090 -p 20001"
  "./synchronizer -h localhost -k 9090 -p 9002 -i 2"
)

cluster3=(
  "./tsd -c 3 -s 1 -h localhost -k 9090 -p 30001"
  "./tsd -c 3 -s 2 -h localhost -k 9090 -p 30002"
  "./synchronizer -h localhost -k 9090 -p 9003 -i 3"
)

clients=(
  "./tsc -h localhost -k 9090 -u 1"
  "./tsc -h localhost -k 9090 -u 2"
  "./tsc -h localhost -k 9090 -u 3"
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
rm *.txt
rm *.csv
# Run the first command with a delay of 5 seconds
process_name="coordinator"
run_command "$process_name" "${commands[0]}"
sleep 2  # Introduce a 5-second delay

# # Run the remaining commands
# for ((i=1; i<${#commands[@]}; i++)); do
#   process_name="server$i"
#   run_command "$process_name" "${commands[i]}"
#   sleep 3
# done

for ((i=0; i<${#cluster1[@]}; i++)); do
  process_name="cluster1 server"
  if [ $i -eq 2 ]; then
    run_command "Syncer 1" "${cluster1[i]}"
  else
    run_command "$process_name" "${cluster1[i]}"
  fi
  sleep 3
done

for ((i=0; i<${#cluster2[@]}; i++)); do
  process_name="cluster2 server"
  if [ $i -eq 2 ]; then
    run_command "Syncer 2" "${cluster2[i]}"
  else
    run_command "$process_name" "${cluster2[i]}"
  fi
  sleep 3
done

for ((i=0; i<${#cluster3[@]}; i++)); do
  process_name="cluster3 server"
  if [ $i -eq 2 ]; then
    run_command "Syncer 3" "${cluster3[i]}"
  else
    run_command "$process_name" "${cluster3[i]}"
  fi
  sleep 3
done

for ((i=0; i<${#clients[@]}; i++)); do
  process_name="client $((i+1))"
  run_command "$process_name" "${clients[i]}"
  sleep 3
done

# Optionally, you can wait for all processes to finish
for process_name in "${!process_map[@]}"; do
  wait "${process_map[$process_name]}"
done
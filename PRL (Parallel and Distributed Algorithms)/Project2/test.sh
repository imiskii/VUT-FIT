#!/bin/bash

# Arguments check
if [ $# -ne 1 ]; then
  echo "Usage: $0 <string>"
  exit 1
fi

input="$1"
length=${#input}

# No input string (empty)
if [ "$length" -eq 0 ]; then
  echo "Error: Input string is empty."
  exit 2
fi

# If input length is 1, set procNum to 1
if [ "$length" -eq 1 ]; then
  procNum=1
else
  procNum=$((2 * (length - 1)))
fi

mpic++ --prefix /usr/local/share/OpenMPI -o vuv vuv.cpp
mpirun --oversubscribe --prefix /usr/local/share/OpenMPI -np "$procNum" ./vuv "$input"
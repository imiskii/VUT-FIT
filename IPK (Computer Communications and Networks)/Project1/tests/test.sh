#!/usr/bin/env bash

#
# file: test.sh
# author: Michal Ľaš
# date: 14.03.2023
#


function printHelp()
{
    echo "Test script for ipkcpc server."
    echo "Usage: ./test.sh <host> <port> <mode>"
    echo "This script has to be placed in directory with TCP and UDP directiories with test cases"
    exit 0
}


# Replace error messages
function replaceERRs()
{
    local input=$(cat $OUTPUT)
    local output=""
    local line=""

    while read -r line; do
        if [[ "$line" == ERR:* ]]; then
            line="${line%%:*}"
        fi
        output="$output$line\n"
    done <<< "$input"

    echo -e "$output" > $OUTPUT
}


ARGC=$#

OUTPUT="tmp.out"
RETURN_CODE=0
EX_RETURN_CODE=0

HOST=$1
PORT=$2
MODE=$3

REDCOLOR='\033[1;31m'
GREENCOLOR='\033[1;32m'
BLUECOLOR='\033[1;34m'
NOCOLOR='\033[0m'


#Check for help argument
while [[ $# -gt 0 ]]; do
  key="$1"
  case $key in
    -h|--help)
      printHelp
      exit 0
      ;;
    *)
      # unknown option
      ;;
  esac
  shift # past argument or value
done


# Check number of arguments
if [[ $ARGC != 3 ]]; then 
    echo -e $REDCOLOR"Wrong number of arguments !\n"$NOCOLOR
    printHelp
    exit 1
fi


# Change directory based on tested mode
if [[ $MODE == 'udp' ]]; then
    cd UDP/
elif [[ $MODE == 'tcp' ]]; then
    cd TCP/
elif [[ $MODE == 'arg' ]]; then
    cd ARG/
else
    echo -r $REDCOLOR"Unknow mode: $MODE !"$NOCOLOR
    printHelp
    exit 1
fi




if [[ $MODE == 'arg' ]]; then # Test arguments
    for src_file in *.src; do
        echo -e $BLUECOLOR"TEST: $src_file"$NOCOLOR
        out_file="${src_file%.src}.out"
        command="../../"$(cat $src_file)
        EX_RETURN_CODE=$(cat $out_file)
        # execute
        $command > /dev/null 2>&1
        RETURN_CODE=$?
        if [[ $RETURN_CODE != $EX_RETURN_CODE ]]; then
            echo -e $REDCOLOR"Expected error code: $EX_RETURN_CODE but error code $RETURN_CODE was given"$NOCOLOR
        else
            echo -e $GREENCOLOR"OK: $out_file"$NOCOLOR
        fi
    done
else # Test TCP and UDP
    for src_file in *.src; do
        echo -e $BLUECOLOR"TEST: $src_file"$NOCOLOR
        out_file="${src_file%.src}.out"
        ../.././ipkcpc -h $HOST -p $PORT -m $MODE < $src_file > $OUTPUT 2>&1
        RETURN_CODE=$?
        replaceERRs
        if [[ $RETURN_CODE == 0 ]]; then
            diff $OUTPUT $out_file
            if [[ $? != 0 ]]; then
                echo -e $REDCOLOR"FAIL: $out_file"$NOCOLOR
            else
                echo -e $GREENCOLOR"OK: $out_file"$NOCOLOR
            fi
        else
            $EX_RETURN_CODE=$(cat $out_file)
            if [[ $RETURN_CODE != $EX_RETURN_CODE ]]; then
                echo -e $REDCOLOR"Expected error code: $EX_RETURN_CODE but error code $RETURN_CODE was given"$NOCOLOR
            else
                echo -e $GREENCOLOR"OK: $out_file"$NOCOLOR
            fi
        fi
    done

    rm $OUTPUT
fi

cd ../
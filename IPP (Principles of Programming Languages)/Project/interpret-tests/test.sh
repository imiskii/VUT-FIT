#!/usr/bin/env bash

#
# file: test.sh
# author: Michal Ľaš xlasmi00
# date: 01.04.2023
#


ARGC=$#

OUTPUT="../tmp.out"
RETURN_CODE=0
EX_RETURN_CODE=0

REDCOLOR='\033[1;31m'
GREENCOLOR='\033[1;32m'
BLUECOLOR='\033[1;34m'
NOCOLOR='\033[0m'


# Function print help message
function printHelp()
{
    echo "Test script for interpret.py"
    echo "Certain tests suits could be executed with name specified in arguments"
    echo "  example: ./test.sh <test suit name> <...>"
    echo "If no program arguments are specified then all tests are executed"
    echo "Help message could be printed with single argument '--help'"
    exit 0
}


# Run test suit
function runTests()
{
    for xml_file in *.xml; do
        echo -e $BLUECOLOR"TEST: $xml_file"$NOCOLOR
        out_file="${xml_file%.xml}.out"
        if [[ $suit == "read/" ]]; then
            in_file="${xml_file%.xml}.in"
            python3.10 ../../../interpret.py --source=$xml_file --input=$in_file > $OUTPUT 2>&1
        else
            python3.10 ../../../interpret.py --source=$xml_file > $OUTPUT 2>&1
        fi
        RETURN_CODE=$?

        if [[ $RETURN_CODE != 0 ]]; then
            EX_RETURN_CODE=$(cat $out_file)

            if [[ $RETURN_CODE == $EX_RETURN_CODE ]]; then
                echo -e $GREENCOLOR"OK: $out_file"$NOCOLOR
            else
                echo -e $REDCOLOR"Expected error code: $EX_RETURN_CODE but error code $RETURN_CODE was given"$NOCOLOR
            fi
        else
            diff $OUTPUT $out_file
            if [[ $? != 0 ]]; then
                echo -e $REDCOLOR"FAIL: $out_file"$NOCOLOR
            else
                echo -e $GREENCOLOR"OK: $out_file"$NOCOLOR
            fi
        fi
    done
}


# Change directory to tests
cd tests/

# Arguments are specified
if [[ ARGC -gt 0 ]]; then

    # Check for '--help' argument
    if [[ $1 == '--help' ]] && [[ ARGC -eq 1 ]]; then
        printHelp
        exit 0
    fi

    # Iterate through arguments
    for suit in "$@"; do
        cd $suit
        runTests
        cd ../
    done

# No arguments are specified
else
    # Iterate through all test suits
    for suit in */; do
    echo "$suit"
        cd $suit
        runTests
        cd ../
    done
fi

rm tmp.out  # $OUTPUT

cd ../
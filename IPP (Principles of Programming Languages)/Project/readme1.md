Documentation for the implementation of project 1 for IPP 2022/2023  
Name and surname: Michal  Ľaš  
Login: xlasmi00

#  parse.php

The parse.php script reads a program in the IPPcode23 language from the STDIN and returns an XML representation of this program to STDOUT.
  
This script also supports statistical data collection via extension (STATP).


## Processing program arguments

At the start of the program, arguments are checked and validated using the function ```processArgs()```. This function is implemented as a simple switch statement in loop iterating through given arguments. As return function give list of statistical parameters to perform.

## Loading and processing input

The program loads the IPPcode23 line by line from the STDIN. Each line is processed using the ```parseLine()``` function, which removes comments and extra whitespace, checks whether it is an empty line or a line containing a header, and returns a list of words in the line. (groups of characters separated by a whitespace character).

## Validation and creation of XML representation

First, it is checked whether the given source code has the necessary header, and then the individual instructions and their arguments are processed. The ```INSTRUCTIONS``` constant contains a list/dictionary of instructions. Each instruction contains the number and type of its arguments (the type of the argument is a regular expression defined as a constant). Using this constant (regular expression) and the ```check_and_write()``` function, all instructions and their arguments are validated. According to the operation code (opcode) of the instruction, an item from the INSTRUCTIONS constant is selected. The number of arguments to the statement is checked, and each argument is matched with a regular expression that defines the type of the argument.
  
Subsequently, the resulting representation in XML code is generated using library XML Writer.

## Performing statistics

Statistics are performed by the ```doStatistic()``` function, which calculates statistical data according to the specified program parameters and stores them in the STATS global variable. This function is located in the program in different places, depending on where the statistical data is monitored.
  
In order to calculate statistics for back jumps, forward jumps and bad jumps, the function contains two static fields ```existingLabels``` and ```expectedLabels``` that store the labels that occurred in the source code.
  
If parameters for making statistics ware given, then the statistics information are written to selected files. This operation is performed with function ```writeStats()```.
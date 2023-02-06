# Design of Computer Systems

## Description

### Project 1

The goal of this project was to create a description of a processor in the VHDL language that will be able to execute a program 
written in the BrainF*ck programming language. 

The processor contained only eight instructions:
* __\>__ incrementing the pointer value 
* __\<__ decrementation the pointer value
* __\+__ incrementing the value of the current memory index
* __\-__ decrementation the value of the current memory index
* __[__ start of while loop
  __]__ end of while loop
* __(__ start of do while loop
  __)__ end of do while loop
* __.__ print value of current memory index
* __,__ load value to current memory index
* __null__ stop program


### Project 2

This project was focused on chained instruction processing. The task was to write code in assembler implementing a simplified version of coding using the Vernam cipher.
And using the [EduMIPS64](https://edumips.org) simulator, it was possible to monitor the execution of individual instructions.

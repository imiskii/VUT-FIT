Documentation for the implementation of project 2 for IPP 2022/2023  
Name and surname: Michal Ľaš  
Login: xlasmi00  


# interpret.py

The interpreter loads the XML representation of IPPcode23, validates this XML code, and processes each instruction one by one.

  

## Quick transition through the interpreter


**Argument parsing** => **Input processing** => **Creating of the instructions** => **Executing instructions**


## Brief description
  
### Program arguments processing and input processing

The program starts by loading and checking arguments using the `programArguments` class. This class is using `argparse` module to parse program arguments.
  
Then the XML representation of IPPcode23 is loaded using the module `xml.etree.ElementTree` and checks whether the loaded XML is correctly formatted. Loading and validating the XML program source is done by the `InputProcessor` class. This class also provide a method that check if input of interpreted program is on STDIN or in given file.

### Program memory representation

The memory model of interpreted is represented by the `Environment` class, which contains: 
+ **Labels**: This dictionary stores data about all labels in the interpreted program. Labels are represented by `Label` class and they are added to this dictionary when *LABEL* instruction is created. 
+ **callStack**: Stack for storing the return position (order of CALL instructions) that were added when *CALL* instruction was executed. Stack is popped when *RETURN* instruction is executed
+ **dataStack**: data stack for instructions that are working over stack. For each stored data, there is a list of 2 items `[value, type_of_value]`. 
+ **Frames**: Instances of the `Frames` class. The `Frames` class represents the frames Global Frame, Local Frame and Temporary Frame (GF, LF, TF) in which program variables are stored. Program variables are represented by `Variable` class
  
`Environment` is designed as the Singleton class. It is because almost every instruction works with the memory model of the interpreted program. So it is easier to access interpreted program memory model, and also it makes sense that interpreted program has only one memory model representation.

### Instructions

Base class `Instruction` contains methods and attributes common to all instructions.

Subclasses of this class represent all instructions in IPPcode23. Each of these subclasses contains an `execute()` method, which represents the action that is invoked during instruction execution. Some of the individual instructions (subclasses of `Instruction class`) have also other methods which represents their behavior. 
`execute()` method does not require any parameters and does not return any value, but there are some exceptions such as:

+ **READ**: for this instruction, the `execute()` method require a `programInput` parameter which is either `None` (if program input is on STDIN) or contains list with input given in file.
+ **CALL**, **RETURN**, **JUMP**, **ConditionalJumps**, and **StackConditionalJumps**: their `execute()` method returns the position (order) of the `Label` where should program jump.

### Interpreting

Works with the `Interpreter` class. First, using the `createInstructions()` and `__sortOutInstructions()` methods, individual instructions are created and stored in the instruction list.
Subsequently, the instructions are executed in the order in which they were created.
  
If any jump instruction occurs, the index to instructions list is changed to the value which is returned from the jump instruction.


## Extensions

### FLOAT
The implementation supports `float` numbers, even in hexadecimal form. The `INT2FLOAT` and `FLOAT2INT` instructions are also implemented and arithmetic, relational, boolean, `READ`, `WRITE`, `TYPE` instructions also work with float numbers.

### STACK
The functions that work with the stack have also been implemented:
`CLEARS`, `ADDS`, `SUBS`, `MULS`, `IDIVS`, `LTS`, `GTS`, `EQS`, `ANDS`, `ORS`, `NOTS`, `INT2CHARS`, `STRI2INTS` and `JUMPIFEQS`, `JUMPIFNEQS`

### STATI
The interpreter allows performing statistics using the `Statistics` class. The implementation allows the use of several `--stats` arguments, where the selected statistics located after the `--stats` argument will be written to the provided files.  
The `Statistics` class is implemented using the Singleton design pattern and for each statistic contains a method that executes it. In case of adding a new argument for performing additional statistics, it is enough to create a new method in this class, call this method in the program at the place where the statistics are to be performed and add a possible new argument to the program.

## Class Diagram

With this design, new instructions can be added at any time. It is enough to create a new class that will inherit properties from `Instruction` class.


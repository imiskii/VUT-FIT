##
# @file: interpret.py
# @brief: script interpreting IPPcode23
# @Author: Michal Ľaš
# @date: 14.02.2023



import xml.etree.ElementTree as ET

from InputProcessor import InputProcessor
from ArgumentParser import programArguments
from instructions import *
from stati import Statistics


# *************************** INTERPRET *************************** #


class Interpret:
    """ 
    Singleton class used to represent program Frames
    ----------------------------------------------
    Attributes:
    programXML : ET.ElementTree
        XML representation of IPPcode23
    programInput : str|None
        input to interpreted program
    environment : Environment
        interpreter environment (Environment object)
    _stati : Statistic
        Instance of class for collectin statistic information
    
    ----------------------------------------------
    Methods:
    __sortOutInstructions : Instruction
        Based on the opcode decide which Instruction objec will be created
    createInstructions : list
        Create list of instructions in order how they are in XML representation of interpreted program
    interpretInstructions : None
        Interpret given list of instruction / execute every instruction
    """

    __instance = None

    def __new__(cls, programXMLSource:ET.ElementTree, programInput:str):
        """
        Parameters
        ----------
        programXMLSource : ET.ElementTree
            XML representation of program (IPPcode23)
        programInput : None|str
            Input to interpreted program
        """
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)

            if programInput is None:
                cls.programInput = programInput
            else:
                try:
                    f = open(programInput, 'r')
                    cls.programInput = f.read()
                    cls.programInput = cls.programInput.split('\n')
                    f.close()
                except OSError:
                    printError(11, f"ERROR: Unable to open input file: {programInput}")

            cls.programXML = programXMLSource
            cls.environment = Environment()
            cls._stati = Statistics()

        return cls.__instance


    def __sortOutInstructions(self, order:int, opcode:str, args:list[InstructionArgument]) -> Instruction:
        """
        Creates certain instruction object based on opcode
        
        Parameters
        ----------
        order : int
            Code order of instraction
        opcode : str
            Operation code
        args : list[InstructionArgument]
            List of Instruction argument

        Return
        ----------
        Initialized Instruction object
        """
        match opcode:
            case "MOVE": return MOVE(order, args)
            case "CREATEFRAME": return CREATEFRAME(order, args)
            case "PUSHFRAME": return PUSHFRAME(order, args)
            case "POPFRAME": return POPFRAME(order, args)
            case "DEFVAR": return DEFVAR(order, args)
            case "CALL": return CALL(order, args)
            case "RETURN": return RETURN(order, args)
            case "LABEL": return LABEL(order, args)
            case "PUSHS": return PUSHS(order, args)
            case "POPS": return POPS(order, args)
            case "ADD" | "SUB" | "MUL" | "IDIV": return ArithmeticInstructions(order, opcode, args)
            case "LT" | "GT" | "EQ": return RelationalInstructions(order, opcode, args)
            case "AND" | "OR" | "NOT": return BooleanInstructions(order, opcode, args)
            case "INT2CHAR": return INT2CHAR(order, args)
            case "STRI2INT": return STRI2INT(order, args)
            case "READ": return READ(order, args)
            case "WRITE": return WRITE(order, args)
            case "CONCAT": return CONCAT(order, args)
            case "STRLEN": return STRLEN(order, args)
            case "GETCHAR": return GETCHAR(order, args)
            case "SETCHAR": return SETCHAR(order, args)
            case "TYPE": return TYPE(order, args)
            case "JUMP": return JUMP(order, args)
            case "JUMPIFEQ" | "JUMPIFNEQ": return ConditionalJumps(order, opcode, args)
            case "EXIT": return EXIT(order, args)
            case "DPRINT": return DPRINT(order, args)
            case "BREAK": return BREAK(order, args)
            case "INT2FLOAT": return INT2FLOAT(order, args)
            case "FLOAT2INT": return FLOAT2INT(order, args)
            case "CLEARS": return CLEARS(order, args)
            case "ADDS" | "SUBS" | "MULS" | "IDIVS": return StackArithmeticInstructions(order, opcode, args)
            case "LTS" | "GTS" | "EQS": return StackRelationalInstructions(order, opcode, args)
            case "ANDS" | "ORS" | "NOTS": return StackBooleanInstructions(order, opcode, args)
            case "INT2CHARS": return INT2CHARS(order, args)
            case "STRI2INTS": return STRI2INTS(order, args)
            case "JUMPIFEQS" | "JUMPIFNEQS": return StackConditionalJumps(order, opcode, args)


    def createInstructions(self) -> list:
        """
        Crete list of Instruction
        
        Parameters
        ----------
        None

        Return
        ----------
        List of initialized Instruction objects
        """
        root = self.programXML.getroot()
        instructions = []
        order = 1
        for instruction in root:
            arg_order = 0
            arg_list = []
            for arg in instruction:
                arg_list.append(InstructionArgument(arg_order, arg.get("type"), arg.text))
                arg_order += 1

            createdInstruction = self.__sortOutInstructions(order, instruction.get("opcode").upper(), arg_list)
            order += 1

            self._stati.StatsFreq(createdInstruction.opcode)
            # LABELs are special, they are executed immediately, i.e. new LABEL are added to environment
            if type(createdInstruction) == LABEL:
                createdInstruction.createLabel()

            # Add instruciton to list of all instructions
            instructions.append(createdInstruction)
                
        return instructions


    def interpretInstructions(self, instructions:list) -> None:
        """
        Interpret program written in IPPcode23
        
        Parameters
        ----------
        instructions : list
            List of instructions that will be interpreted

        Return
        ----------
        None
        """
        exePos = 0 # index of code in instructions
        instructionCount = len(instructions)

        while exePos < instructionCount:
            self._stati.StatsInsts(instructions[exePos].opcode)
            self._stati.StatsHot(instructions[exePos].order, instructions[exePos].opcode)
            self._stati.StatsVars(instructions[exePos])
            if instructions[exePos].opcode in ["CALL", "RETURN", "JUMP", "JUMPIFEQ", "JUMPIFNEQ", "JUMPIFEQS", "JUMPIFNEQS"]:
                newPos = instructions[exePos].execute()
                if newPos is not None:
                    exePos = newPos
                else:
                    exePos += 1
            elif instructions[exePos].opcode == "READ":
                self.programInput = instructions[exePos].execute(self.programInput)
                exePos += 1
            else:
                instructions[exePos].execute()
                exePos += 1

                


# ****************************** MAIN ***************************** #


if __name__ == "__main__":
    
    # Process arguments
    PA = programArguments()
    PA.validateArgs()
    PA.parseArgs()
    args = PA.getArgsDict()

    # Load source and input
    IP = InputProcessor(args["source"], args["input"])
    xml = IP.loadSource()
    programInput = IP.inputFile

    # Check loaded xml
    IP.validateXML(xml)

    # Interpret
    intr = Interpret(xml, programInput)
    instructions:list = intr.createInstructions()
    intr.interpretInstructions(instructions)

    intr._stati.StatsWrite()

### END OF FILE ###    
##
# @file: stati.py
# @brief: classes and functions for preforming statistic information in interpret.py
# @Author: Michal Ľaš
# @date: 03.04.2023

from ArgumentParser import programArguments
from instructions import Instruction, Environment
from exitError import printError

class Statistics():
    """ 
    Singleton class for performing statistic information in interpret.py
    ----------------------------------------------
    Attributes:
    __toPerforme : dict
        dictionary with statistic that was declared in program arguments
    __stats_list : list
        list with statistic parameters and order in wich they should be printed
    __insts : int
        Counter for executed instructions
    __hotInstruction : str
        Name of instruction which is executed most times
    __hotCount : int
        Number of executions of the most executed instruction
    __hotDict : dict
        Dictionary with instructions name and thier number of executions
    __varsMax : int
        Maximum number of initialized variables in one time
    __varsCurrent : int
        Current number of initialized variables
    __definedVars : list
        List with name of initialized variables and their frames
    __freq : dict
        Dictionary with instruction in source code and their number of occurrences
    __maxFreq : int
        Maximum occurrences of instruction
        
    ----------------------------------------------
    Methods:
    StatsInsts : None
        Counts the number of executed instructions
    StatsHot : None
        count for the statistics the value of the 'order' attribute for the executed instruction, which has been executed the most times and has the lowest value of the 'order' attribute
    StatsVars : None
        count for the statistic the maximum number of initialized variables present at one time
    StatsFreq : None
        Counts the occurrence of individual instructions
    StatsWrite : None
        Write statistic information to given file
    """

    __instance = None

    def __new__(cls):
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)

            pa = programArguments()
            cls.__toPerforme = pa.getArgsDict()
            cls.__stats_list = pa.stats_args

            # number of executed instruction
            cls.__insts = 0
            # order of instruction that was executed the most times
            cls.__hotInstruction = None
            cls.__hotCount = 0
            cls.__hotDict = {}
            # maximum number of initialized variables in one time
            cls.__varsMax = 0
            cls.__varsCurrent = 0
            cls.__definedVars = []
            # frequency of individual instructions
            cls.__freq = {}
            cls.__maxFreq = 0

        return cls.__instance
    

    def StatsInsts(self, instruction:str) -> None:
        """
        Performs statistics for 'insts'\n
        Counts the number of executed instructions 
        
        Parameters
        ----------
        instruction : str
            Name of instruction

        Return
        ----------
        None
        """
        if self.__toPerforme["insts"]:
            if instruction not in ["LABEL", "DPRINT", "BREAK"]:
                self.__insts += 1
    

    def StatsHot(self, order:int, instruction:str) -> None:
        """
        Performs statistics for 'hot'\n
        count for the statistics the value of the 'order' attribute for the executed instruction,\n
        which has been executed the most times and has the lowest value of the 'order' attribute
        
        Parameters
        ----------
        order:int
            order of the instruction
        instruction : str
            Name of instruction

        Return
        ----------
        None
        """
        if self.__toPerforme["hot"]:
            if instruction not in self.__hotDict:
                self.__hotDict[instruction] = (1,order)
                if self.__hotCount < 1:
                    self.__hotCount = 1
                    self.__hotInstruction = instruction
            else:
                item = self.__hotDict[instruction]
                cnt = item[0] + 1
                ord = item[1]
                if ord > order:
                    ord = order
                self.__hotDict[instruction] = (cnt, ord)
                if self.__hotCount < item[0] or (self.__hotCount == item[0] and item[1] < self.__hotDict[self.__hotInstruction][1]):
                    self.__hotCount = item[0]
                    self.__hotInstruction = instruction


    def StatsVars(self, instruction:Instruction) -> None:
        """
        Performs statistics for 'vars'\n
        count for the statistic the maximum number of initialized variables present at one time
        
        Parameters
        ----------
        instruction : str
            Name of instruction

        Return
        ----------
        None
        """
        if self.__toPerforme["vars"]:
            if instruction.opcode == "CREATEFRAME" or instruction.opcode == "POPFRAME":
                for var in self.__definedVars:
                    env = Environment()
                    tmp_frame = env.Frames._getTF()
                    if tmp_frame is not None:
                        if var in tmp_frame:
                            self.__definedVars.pop(self.__definedVars.index(var))
                            self.__varsCurrent -= 1
            elif instruction.opcode in ["MOVE", "POPS", "ADD", "SUB", "MUL", "IDIV", "LT", "GT", "EQ", "AND", "OR", "NOT", 
                                        "INT2CHAR", "STRI2INT", "READ", "CONCAT", "STRLEN", "GETCHAR", "SETCHAR", "TYPE"]:
                if instruction.opcode not in self.__definedVars:
                    self.__definedVars.append(instruction.getArg(0).value)
                    self.__varsCurrent += 1
                    if self.__varsCurrent > self.__varsMax:
                        self.__varsMax = self.__varsCurrent


    def StatsFreq(self, instruction:str) -> None:
        """
        Performs statistics for 'frequent'\n
        Counts the occurrence of individual instructions
        
        Parameters
        ----------
        instruction : str
            Name of instruction

        Return
        ----------
        None
        """
        if self.__toPerforme["frequent"]:
            if instruction not in self.__freq:
                self.__freq[instruction] = 1
            else:
                self.__freq[instruction] += 1
            if self.__maxFreq < self.__freq[instruction]:
                self.__maxFreq = self.__freq[instruction]


    def StatsWrite(self) -> None:
        """
        Write statistic information to given file
        
        Parameters
        ----------
        None

        Return
        ----------
        None
        """
        index = 0
        while index < len(self.__stats_list):
            match self.__stats_list[index]:
                case "--stats":
                    if index != 0:
                        f.close()
                    try:
                        f = open(self.__stats_list[index+1], "w")
                    except OSError:
                        printError(12, f"Enable to write statistic to file: {self.__stats_list[index+1]}")
                    index += 1
                case "--insts":
                    f.write(f"{self.__insts}\n")
                case "--hot":
                    f.write(f"{self.__hotDict[self.__hotInstruction][1]}\n")
                case "--vars":
                    f.write(f"{self.__varsMax}\n")
                case "--eol":
                    f.write("\n")
                case "--print":
                    f.write(f"{self.__stats_list[index+1]}\n")
                    index += 1
                case "--frequent":
                    comma = False
                    for instr,occr in self.__freq.items():
                        if occr == self.__maxFreq:
                            f.write(f",{instr}") if comma else f.write(f"{instr}")
                            comma = True
                    f.write("\n")
            index += 1

        if index != 0:
            f.close()
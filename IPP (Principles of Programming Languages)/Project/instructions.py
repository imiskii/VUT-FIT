##
# @file: instructions.py
# @brief: InstructionArgument, Instruction, Variable, Label, Frames, Environment classes for interpret.py
# @Author: Michal Ľaš
# @date: 01.04.2023


import sys
from constants import Constants as C
from exitError import printError


class InstructionArgument:
    """ 
    Class used to represent Instruction Argument
    ----------------------------------------------
    Attributes:
    order : int
        Number 1, 2 or 3 representing argument's order
    type : str
        String representing type of argument ('var', 'type', 'bool', 'int', 'string', 'nil', 'label')
    value : any
        Value stored in argument
    frame : str|None
        if variable is type of VAR, then there are stored frame in which variable is located (GF, LF, TF) else frame attribute is None
    ----------------------------------------------
    Methods
    __setValue : str
        Cast (retypes) the Instruction argument value according to the type of the argument
    evalEscapeSeq : str
        Method evaluates all escape seqeunces in given string
    """
    
    def __init__(self, order:int, arg_type:str, value:str) -> None:
        """
        Parameters
        ----------
        order : int
            Order of instruction argument (values : 0, 1 or 2)
        arg_type : str
            String representation of instruction argument type
        value : str
            String representation of instruction argument value
        """
        self.order = order
        self.type = arg_type
        if arg_type == C.VAR:
            tmpPos = value.find("@")
            self.value = value[tmpPos+1:]
            self.frame = value[:tmpPos]
        else:
            self.frame = None
            self.value = self.__setValue(value, arg_type)


    def __setValue(self, value:str, type:str) -> any:
        """
        Cast (retypes) the Instruction argument value according to the type of the argument
        
        Parameters
        ----------
        value : str
            String representation of Instruction Argument value
        type : str
            String representation of Instruction Argument type
            
        Return
        ----------
        Retyped value according to the specified type
        """
        match type:
            case C.INT:
                return int(value, 0)
            case C.FLOAT:
                return float.fromhex(value) if value[:2] == "0x" or value[1:3] == "0x" else float(value)                    
            case C.BOOL:
                return (True if value == "true" else False)
            case C.NIL:
                return type
            case C.STR:
                if value is None:
                    return ""
                else:
                    # TODO: escape sequences
                    return self.evalEscapeSeq(value)
            case _:
                return value
    

    @staticmethod
    def evalEscapeSeq(string:str) -> str:
        """
        Method evaluates all escape seqeunces in given string
        
        Parameters
        ----------
        string : str
            string to be proccessed

        Return
        ----------
        String with replaced escape sequences
        """
        i = 0
        result = ''
        while i < len(string):
            if string[i] == '\\':
                result = result + chr(int(string[i+1:i+4]))
                i += 3
            else:
                result = result + string[i]
            i += 1

        return result
        


class Instruction:
    """ 
    Class used to represent Instruction
    ----------------------------------------------
    Attributes:
    order : int
        Number representing the order of the Instruction
    opcode : str
        Operation code of Instruction
    args : list[InstructionArgument]
        List of Instruction arguments
    
    ----------------------------------------------
    Methods
    getArg : InstructionArgument
        Return Instruction argument by given order
    _checkArgType : None
        Check if given type and expected type are equal
    """

    def __init__(self, order:str, opcode:str, args:list[InstructionArgument]) -> None:
        """
        Parameters
        ----------
        order : int
            Order of Instruction
        opcode : str
            Opeartion code of Instruction
        args : list[InstructionArgument]
            List of Instruction arguments
        
        """
        self.order = int(order)
        self.opcode = opcode
        self.args = args


    def getArg(self, index:int) -> InstructionArgument:
        """
        Return Instruction argument by given index (order) (values : 0, 1 or 2)
        
        Parameters
        ----------
        index:
            Number 0, 1 or 2 indicating the index (order) of the argument

        Return
        ----------
        Instruction Argument on given index
        """
        return self.args[index]
    

    def _checkArgType(self, instructionOpcode:str, argType:str, expectedTypes:list) -> None:
        """
        Check if given type and expected type are equal
        In case the types do not match method print error message on STDERR and exit with exit code 53
        
        Parameters
        ----------
        instructionOpcode:
            Operation code of current instruction
        argType:
            Instruction argument type
        expectType:
            Expected Instruction argument types

        Return
        ----------
        None
        """
        if argType not in expectedTypes:
            printError(53, f"ERROR: Invalid arguments type ! Instruction {instructionOpcode} type: {argType} expected: {expectedTypes}")



class Variable:
    """ 
    Class used to represent program Variable
    ----------------------------------------------
    Attributes:
    name : str
        Name of variable
    frame : str
        Frame in which variable is located
    type : str
        String representation of variable type
    value : any
        Data stored in variable
    
    ----------------------------------------------
    Methods:
    """

    def __init__(self, name:str, frame:str, type:str=None, value:any=None) -> None:
        """
        Parameters
        ----------
        name : str
            Name of variable
        frame : str
            String representation of frame where variable is located
        type : str
            String representation of variable type
        value : any
            Data stored in variable
        """
        self.name = name
        self.frame = frame
        self.type = type
        self.value = value


class Label:
    """ 
    Class used to represent program Label
    ----------------------------------------------
    Attributes:
    codePosition : int
        Number representing the order of the Label
    name : str
        Name of Label
    
    ----------------------------------------------
    Methods:
    """
    def __init__(self, codePosition:int, name:str) -> None:
        """
        Parameters
        ----------
        codePosition : int
            position (order) of LABEL instruction that definde this label
        name : str
            Name of Label
        """
        self.position = codePosition
        self.name = name      



class Frames:
    """ 
    Class used to represent program Frames
    ----------------------------------------------
    Attributes:
    __GlobalFrame : dict
        Dictionary of variables in Gloval Frame
    __LocalFrameStack : list[dict]
        List of dictionaries with Local Frame variables
     __TemporaryFrame : dict
        Dictionary of variables in Temporary Frame
    
    ----------------------------------------------
    Methods:
    _getGF : dict
        Return Global Frame
    _getLFs : list[dict]
        Return list with Local Frames
    _getTF : dict
        Return Temporary Frame
    createTF : None
        Create/Clear Temporary Frame
    pushTF : None
        Push Temporary Frame to Local Frame Stack
    popLF : None
        Pop last Local Frame from Local Fram Stack
    isVarInFrame : bool
        Return true if given variable name is in given frame else return false
    addVarToFrame : None
        Add  new Variable to frame
    getVarFromFrame : Variable
        Return Variable with given name and frame
    updateVarInFrame : None
        Update existing Variable
    """    

    __GlobalFrame = {}
    __LocalFrameStack = [dict]
    __TemporaryFrame = None

    def __init__(self) -> None:
        """
        Parameters
        ----------
        None
        """
        self.__GlobalFrame.clear()
        self.__LocalFrameStack.clear()
        self.__TemporaryFrame = None


    def _getGF(self) -> dict:
        """
        Return Global Frame
        
        Parameters
        ----------
        None

        Return
        ----------
        Global Frame
        """
        return self.__GlobalFrame
    

    def _getLFs(self) -> list[dict]:
        """
        Return List of Local Frames
        
        Parameters
        ----------
        None

        Return
        ----------
        List of Local Frames
        """
        return self.__LocalFrameStack
    

    def _getTF(self) -> dict:
        """
        Return Temporary Frame
        
        Parameters
        ----------
        None

        Return
        ----------
        Temporary Frame
        """
        return self.__TemporaryFrame

    
    def createTF(self) -> None:
        """
        Create new Temporary frame or clear existing one
        
        Parameters
        ----------
        None

        Return
        ----------
        None
        """
        if self.__TemporaryFrame is None:
            self.__TemporaryFrame = {}
        else:
            self.__TemporaryFrame.clear()


    def pushTF(self) -> None:
        """
        Push Temporary Frame to Local Frame Stack
        Temporary Fram has to be initialized
        
        Parameters
        ----------
        None

        Return
        ----------
        None
        """
        if self.__TemporaryFrame is not None:
            # Change variables frames
            for name in self.__TemporaryFrame:
                self.__TemporaryFrame[name].frame = C.LF

            self.__LocalFrameStack.append(self.__TemporaryFrame)
            self.__TemporaryFrame = None
        else:
            printError(55, "ERROR: Using uninitialized Temporary Frame !")

    
    def popLF(self) -> None:
        """
        Pop last Local Frame from Local Frame Stack
        If Local Frame Stack is empty method print error mesage on STDERR and exit with error code 55
        
        Parameters
        ----------
        None

        Return
        ----------
        None
        """
        if self.__LocalFrameStack:
            self.__TemporaryFrame = self.__LocalFrameStack.pop()
        else:
            printError(55, "ERROR: Attempt to pop empty Local Frame stack !")


    def isVarInFrame(self, var_name:str, frame:str) -> bool:
        """
        Seach for Variable name in certain frame
        
        Parameters
        ----------
        var_name : str
            Name of Variable
        frame : str
            Frame in which the variable will be searched

        Return
        ----------
        True if Variable with given name is in given frame
        False if Variable is not in frame
        """
        match(frame):
            case C.GF:
                return var_name in self.__GlobalFrame
            case C.TF:
                if self.__TemporaryFrame is not None:
                    return var_name in self.__TemporaryFrame
                else:
                    printError(55, "ERROR: Using uninitialized Temporary Frame !")
            case C.LF:
                try:
                    return var_name in self.__LocalFrameStack[-1]
                except Exception:
                    printError(55, "ERROR: Attempt to access non-existenting Local Frame. Local Frame stack is epmty !")
            case _:
                printError(55, "ERROR: Given frame name does not exist !")


    def addVarToFrame(self, var:Variable) -> None:
        """
        Add Variable to Frame
        
        Parameters
        ----------
        var : Variable
            Variable that will be added to Frame

        Return
        ----------
        None
        """
        if not self.isVarInFrame(var.name, var.frame):
            match (var.frame):
                case C.GF:
                    self.__GlobalFrame[var.name] = var
                case C.LF:
                    self.__LocalFrameStack[-1][var.name] = var
                case C.TF:
                    self.__TemporaryFrame[var.name] = var
                    
        else:
            printError(52, f"ERROR: Redefinition of variable: {var.name}")

    
    def getVarFromFrame(self, var_name:str, frame:str) -> Variable:
        """
        Get Variable from Frame
        If Variable is not found method print error mesage on STDERR and exit with exit code 52
        
        Parameters
        ----------
        var_name : str
            Name of Variable
        frame : str
            Frame in which the variable is located

        Return
        ----------
        Variable
        """
        if self.isVarInFrame(var_name, frame):
            match (frame):
                case C.GF:
                    return self.__GlobalFrame[var_name]
                case C.LF:
                    return self.__LocalFrameStack[-1][var_name]
                case C.TF:
                    return self.__TemporaryFrame[var_name]
        else:
            printError(52, f"ERROR: Using non-existenting variable {var_name}")


    def updateVarInFrame(self, var:Variable) -> None:
        """
        Updata existing Variable
        If Variable is not found method print error mesage on STDERR and exit with exit code 52
        
        Parameters
        ----------
        var : Variable
            Variable that will be updated

        Return
        ----------
        None
        """
        if self.isVarInFrame(var.name, var.frame):
            match (var.frame):
                case C.GF:
                    self.__GlobalFrame[var.name] = var
                case C.LF:
                    self.__LocalFrameStack[-1][var.name] = var
                case C.TF:
                    self.__TemporaryFrame[var.name] = var
                    
        else:
            printError(52, f"ERROR: Using non-existenting variable {var.name}")


class Environment():
    """ 
    Singleton class used to represent program Frames
    ----------------------------------------------
    Attributes:
    Frames : Frames
        Frames Object
    __Labels : dict
        List of labels in program
    __callStack : list
        List/Stack of positions (order of LABEL instruction) that was called with instruction CALL
    __dataStack : list
        List/Stack of data stored on stack
    
    ----------------------------------------------
    Methods:
    getLabel : Label
        Return Label by given name
    pushCallStack : None
        Push order number of Label that was called
    popCallStack : int
        Pop last number from callStack and return that number
    pushDataStack : None
        Push value to dataStack
    clearDataStack : None
        Deletes all items in Data Stack
    popDataStack : any
        Pop last value from dataStack and return that value
    addVariable : None
        Add Variable to Frames
    addLabel : None
        Add Label to Labels (dict)
    """

    __instance = None

    def __new__(cls):
        """
        Parameters
        ----------
        None
        """
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)
            cls.__Labels = {}
            cls.__callStack = []
            cls.__dataStack = []
            cls.Frames = Frames()
        return cls.__instance
    

    def getLabel(self, name:str) -> Label:
        """
        Get existing Label from Labels (dict)
        If Lable is not found method print error mesage on STDERR and exit with exit code 52
        
        Parameters
        ----------
        name : str
            Name of Label

        Return
        ----------
        Label with given name
        """
        try:
            return self.__Labels[name]
        except Exception:
            printError(52, f"ERROR: Attempt to jump on non-existing LABEL ({name}) !")


    def pushCallStack(self, position:int) -> None:
        """
        Push code jump position (order) to callStack
        
        Parameters
        ----------
        position : int
            Position (order) that will be pushed to stack

        Return
        ----------
        None
        """
        self.__callStack.append(position)


    def popCallStack(self) -> int:
        """
        Pop code jump position (order) from callStack
        
        Parameters
        ----------
        None

        Return
        ----------
        Position (order) of last value in callStack
        """
        try:
            return self.__callStack.pop()
        except Exception:
            printError(56, "ERROR: Attempt to RETURN without a previous CALL !")


    def pushDataStack(self, value:any) -> None:
        """
        Push data to dataStack
        
        Parameters
        ----------
        value : any
            Value that will be pushed to dataStack

        Return
        ----------
        None
        """
        self.__dataStack.append(value)


    def popDataStack(self) -> any:
        """
        Pop data From dataStack
        
        Parameters
        ----------
        None

        Return
        ----------
        Last value in dataStack
        """
        try:
            return self.__dataStack.pop()
        except Exception:
            printError(56, "ERROR: Attempt to pop empty stack !")

    
    def clearDataStack(self) -> None:
        """
        Deletes all items in Data Stack

        Parameters
        ----------
        None

        Return
        ----------
        None
        """
        self.__dataStack.clear()

    
    def addVariable(self, name:str, frame:str) -> None:
        """
        Add Variable to Environment Frames (Frames Object)
        
        Parameters
        ----------
        name : str
            Name of variable
        frame : str
            Name of frame

        Return
        ----------
        None
        """
        self.Frames.addVarToFrame(Variable(name, frame))


    def addLabel(self, codePosition:int, name:str) -> None:
        """
        Add Label to Environment Frames (Frames Object)
        
        Parameters
        ----------
        codePosition : int
            Code order (position) of LABEL instruction
        name : str
            Name of Label

        Return
        ----------
        None
        """
        if not name in self.__Labels:
            self.__Labels[name] = Label(codePosition, name)
        else:
            printError(52, f"ERROR: Attempt to create LABEL with name {name} that already exist ! ")


# ************************** INSTRUCTIONS ************************* #

# There are subclasses for each instruction from IPPcode23
# Each object has an execute() method that performs its operation
# Each object inherit from Instruction class



class MOVE(Instruction):
    """
    MOVE ⟨var⟩ ⟨symb⟩\n
    Assigning a value to a variable\n
    Copies the value of ⟨symb⟩ to ⟨var⟩.
    """

    def __init__(self, order:str, args:list[InstructionArgument]) -> None:
        super().__init__(order, "MOVE", args)


    def execute(self) -> None:
        env = Environment()

        # Destination
        destVar = env.Frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)

        # Source
        sourceVarType = self.getArg(1).type
        
        if sourceVarType == C.VAR:
            sourceVar = env.Frames.getVarFromFrame(self.args[1].value, self.getArg(1).frame)

            sourceVarType = sourceVar.type
            sourceVarValue = sourceVar.value

        else:
            sourceVarType = self.getArg(1).type
            sourceVarValue = self.getArg(1).value

        # Set and push
        destVar.value = sourceVarValue
        destVar.type = sourceVarType
        env.Frames.updateVarInFrame(destVar)


class CREATEFRAME(Instruction):
    """
    CREATEFRAME\n
    Creates a new temporary frame and discards any contents of the original temporary frame.
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "CREATEFRAME", args)

    
    def execute(self) -> None:
        env = Environment()
        env.Frames.createTF()


class PUSHFRAME(Instruction):
    """
    PUSHFRAME\n
    Move the temporary frame to the frame stack
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "PUSHFRAME", args)

    
    def execute(self) -> None:
        env = Environment()
        env.Frames.pushTF()


class POPFRAME(Instruction):
    """
    POPFRAME\n
    Move the current frame to the temporary one
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "POPFRAME", args)


    def execute(self) -> None:
        env = Environment()
        env.Frames.popLF()


class DEFVAR(Instruction):
    """
    DEFVAR ⟨var⟩\n
    Define a new variable in the frame
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "DEFVAR", args)

    def execute(self) -> None:
        env = Environment()
        var:InstructionArgument = self.getArg(0)
        env.addVariable(var.value, var.frame)


class CALL(Instruction):
    """
    CALL ⟨label⟩\n
    Jump on the label with return possibility to return
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "CALL", args)


    def execute(self) -> int:
        env = Environment()
        label:Label = env.getLabel(self.getArg(0).value)
        env.pushCallStack(self.order)
        return (label.position - 1)


class RETURN(Instruction):
    """
    RETURN\n
    Return to the position saved by the CALL instruction
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "RETURN", args)


    def execute(self) -> int:
        env = Environment()
        return env.popCallStack()


class PUSHS(Instruction):
    """
    PUSHS ⟨symb⟩\n
    Put the value on the top of the data stack
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "PUSHS", args)

    
    def execute(self) -> None:
        env = Environment()
        item = []
        if self.getArg(0).type == C.VAR:
            var = env.Frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
            item.append(var.value)
            item.append(var.type)    
            env.pushDataStack(item)
        else:
            item.append(self.getArg(0).value)
            item.append(self.getArg(0).type)
            env.pushDataStack(item)


class POPS(Instruction):
    """
    POPS ⟨var⟩\n
    Remove the value from the top of the data stack
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "POPS", args)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        var = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        item = env.popDataStack()
        var.value = item[0]
        var.type = item[1]
        frames.updateVarInFrame(var)


class ArithmeticInstructions(Instruction):
    """
    ADD/SUB/MUL/IDIV ⟨var⟩ ⟨symb1 ⟩ ⟨symb2 ⟩\n
    The arithmetics operation of two numerical values\n
    ADD/SUB/MUL/IDIV ⟨symb1 ⟩ and ⟨symb2 ⟩ and stores the resulting value in a variable ⟨var⟩.

    -----------------------------------
    Methods:
    ArithmeticOperation : Tuple
        Do ADD, SUB, MUL and IDIV operations with val1 and val2
    """

    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    @staticmethod
    def ArithmeticOperation(operation:str, val1:int|float, val2:int|float) -> tuple:
        """
        Do ADD, SUB, MUL and IDIV operations with val1 and val2
        
        Parameters
        ----------
        operation : str
            type of operation ADD/SUB/MUL/IDIV
        val1 : int|float
            first operand
        val2 : int|float
            second operand
            
        Return
        ----------
        Tuple (result of operation, string representation of type of result ("int" or "float"))
        """
        match operation:
            case "ADD":
                result = val1 + val2
            case "SUB":
                result = val1 - val2
            case "MUL":
                result = val1 * val2
            case "IDIV":
                if val2 == 0:
                    printError(57, "ERROR: Division by zero !")
                else:
                    result = val1 / val2
        
        if type(result) is float:
            resultType = C.FLOAT
        else:
            resultType = C.INT

        return (result, resultType)

    
    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        # arg1
        if (self.getArg(1).type == C.VAR):
            val1 = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, val1.type, [C.INT, C.FLOAT])
            val1 = val1.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.INT, C.FLOAT])
            val1 = self.getArg(1).value

        # arg2
        if (self.getArg(2).type == C.VAR):
            val2 = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            self._checkArgType(self.opcode, val2.type, [C.INT, C.FLOAT])
            val2 = val2.value
        else:
            self._checkArgType(self.opcode, self.getArg(2).type, [C.INT, C.FLOAT])
            val2 = self.getArg(2).value
        
        # Operation
        result = self.ArithmeticOperation(self.opcode, val1, val2)

        dest.value = result[0]
        dest.type = result[1]
        frames.updateVarInFrame(dest)



class RelationalInstructions(Instruction):
    """
    LT/GT/EQ ⟨var⟩ ⟨symb1 ⟩ ⟨symb2 ⟩\n
    Relational operators less, greater, equal\n
    The instruction evaluates the relational operator between ⟨symb1 ⟩ and ⟨symb2 ⟩ and stores the resulting value in a variable ⟨var⟩.
    
    -----------------------------------
    Methods:
    RelationalOperation : bool
        Do LT(<), GT(>), EQ(=) operations with val1 and val2
    """

    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    @staticmethod
    def RelationalOperation(operation:str, val1, val1Type:str, val2, val2Type:str) -> bool:
        """
        Do LT(<), GT(>), EQ(=) operations with val1 and val2
        
        Parameters
        ----------
        operation : str
            type of operation ADD/SUB/MUL/IDIV
        val1
            first operand
        val2
            second operand
            
        Return
        ----------
        Result of selected relation operation (True/False)
        """
        # Check arguments
        if not((val1Type == C.FLOAT or val1Type == C.INT) and (val2Type == C.FLOAT or val2Type == C.INT)):
            if (val1Type != val2Type) and operation != "EQ":
                printError(53, f"ERROR: Instruction {operation}: Attempt to compare {val1Type} and {val2Type} !")
            elif (operation == "EQ" and val1Type != val2Type) and not(val1Type == C.NIL or val2Type == C.NIL):
                printError(53, f"ERROR: Instruction {operation}: Attempt to compare {val1Type} and {val2Type} !")

        # Operation
        match operation:
            case "LT":
                return (val1 < val2)
            case "GT":
                return (val1 > val2)
            case "EQ":
                return (val1 == val2)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        # arg1
        if (self.getArg(1).type == C.VAR):
            val1 = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            val1Type = val1.type
            val1 = val1.value
        else:
            val1Type = self.getArg(1).type
            val1 = self.getArg(1).value

        # arg2
        if (self.getArg(2).type == C.VAR):
            val2 = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            val2Type = val2.type
            val2 = val2.value
        else:
            val2Type = self.getArg(2).type
            val2 = self.getArg(2).value
        
        dest.value = self.RelationalOperation(self.opcode ,val1, val1Type, val2, val2Type)
        dest.type = C.BOOL
        frames.updateVarInFrame(dest)


class BooleanInstructions(Instruction):
    """
    AND/OR/NOT ⟨var⟩ ⟨symb1 ⟩ ⟨symb2 ⟩\n
    Basic Boolean operators\n
    Applies conjunction (logical AND)/disjunction (logical OR) to operands of type bool ⟨symb1 ⟩ and ⟨symb2 ⟩ or the negation of ⟨symb1 ⟩\n
    Resul value is sotred in a variable ⟨var⟩

    -----------------------------------
    Methods:
    BooleanOperation : bool
        Do AND, OR, NOT operations with val1 and val2
    """

    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    @staticmethod
    def BooleanOperation(operation:str, val1:bool, val2:bool) -> bool:
        """
        Do AND, OR, NOT operations with val1 and val2
        
        Parameters
        ----------
        operation : str
            type of operation ADD/SUB/MUL/IDIV
        val1
            first operand
        val2
            second operand
            
        Return
        ----------
        Result of selected relation operation (True/False)
        """

        match operation:
            case "AND":
                return (val1 and val2)
            case "OR":
                return (val1 or val2)
            case "NOT":
                return (not val1)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        # arg1
        if (self.getArg(1).type == C.VAR):
            val1 = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, val1.type, [C.BOOL])
            val1 = val1.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.BOOL])
            val1 = self.getArg(1).value

        # arg2 - NOT has only 1 argument
        if self.opcode != "NOT":
            if (self.getArg(2).type == C.VAR):
                val2 = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
                self._checkArgType(self.opcode, val2.type, [C.BOOL])
                val2 = val2.value
            else:
                self._checkArgType(self.opcode, self.getArg(2).type, [C.BOOL])
                val2 = self.getArg(2).value
        else:
            val2 = None
        
        # Operation
        dest.value = self.BooleanOperation(self.opcode, val1, val2)        
        dest.type = C.BOOL
        frames.updateVarInFrame(dest)


class INT2CHAR(Instruction):
    """
    INT2CHAR ⟨var⟩ ⟨symb⟩\n
    Converting an integer to a character\n
    According to Unicode, the numeric value ⟨symb⟩ is converted to a character that forms a one-character string assigned to ⟨var⟩.

    -----------------------------------
    Methods:
    IntToChar : str
        Convert integer to a character
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "INT2CHAR", args)


    @staticmethod
    def IntToChar(source:int) -> str:
        """
        Convert integer to a character
        
        Parameters
        ----------
        source : int
            integer to be converted
            
        Return
        ----------
        Character converted from integer
        """
        try:
            val = chr(source)
        except Exception:
            printError(58, f"ERROR: Instruction INT2CHAR: Invalide code of Unicode character (code: {source}) !")

        return val

    
    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)

        if self.getArg(1).type == C.VAR:
            source = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, source.type, [C.INT])
            source = source.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.INT])
            source = self.getArg(1).value


        dest.value = self.IntToChar(source)
        dest.type = C.STR
        frames.updateVarInFrame(dest)



class STRI2INT(Instruction):
    """
    STRI2INT ⟨var⟩ ⟨symb1 ⟩ ⟨symb2 ⟩\n
    The ordinal value of the character\n
    In ⟨var⟩, stores the ordinal value of the character (according to Unicode) in the string ⟨symb1⟩ at the position ⟨symb2⟩.

    -----------------------------------
    Methods:
    StrToInt : int
        Convert character in string on given index to its integer value
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "STRI2INT", args)


    @staticmethod
    def StrToInt(source:str, index:int) -> int:
        """
        Convert character in string on given index to its integer value
        
        Parameters
        ----------
        source : str
            source string
        index : int
            index of character in string
            
        Return
        ----------
        Integer representation of choosen character in string
        """
        try:
            val = ord(source[index])
        except Exception:
            printError(58, f"ERROR: Instruction STRI2INT: index value {index} out of string {source} !")

        return val


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        
        # arg1
        if self.getArg(1).type == C.VAR:
            source = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, source.type, [C.STR])
            source = source.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.STR])
            source = self.getArg(1).value

        # arg2
        if self.getArg(2).type == C.VAR:
            indexPos = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            self._checkArgType(self.opcode, indexPos.type, [C.INT])
            indexPos = indexPos.value
        else:
            self._checkArgType(self.opcode, self.getArg(2).type, [C.INT])
            indexPos = self.getArg(2).value
        
        dest.value = self.StrToInt(source, indexPos)
        dest.type = C.INT
        frames.updateVarInFrame(dest)


class READ(Instruction):
    """
    READ ⟨var⟩ ⟨type⟩\n
    Reading value from standard input\n
    It reads one value according to the specified type ⟨type⟩ ∈ {int, string, bool} and stores this value in variable ⟨var⟩.
    """
    
    def __init__(self, order: str, args: list[InstructionArgument]) -> str:
        super().__init__(order, "READ", args)

    
    def execute(self, programInput:list|None):
        env = Environment()
        # Get Input
        if programInput is None:
            try:
                inpt = input()
            except Exception:
                inpt = C.NIL
                inptType = C.NIL
        else:
            try:
                inpt = programInput.pop(0)
            except Exception:
                inpt = C.NIL
                inptType = C.NIL

        if inpt != C.NIL:
            # Change type
            match self.getArg(1).value:
                case "int":
                    try:
                        inpt = int(inpt)
                        inptType = C.INT
                    except Exception:
                        inpt = C.NIL
                        inptType = C.NIL
                case "float":
                    try:
                        inpt = float.fromhex(inpt) if inpt[:2] == "0x" or inpt[1:3] == "0x" else float(inpt)
                        inptType = C.FLOAT
                    except Exception:
                        inpt = C.NIL
                        inptType = C.NIL
                case "string":
                    try:
                        inpt = InstructionArgument.evalEscapeSeq(str(inpt))
                        inptType = C.STR
                    except Exception:
                        inpt = C.NIL
                        inptType = C.NIL
                case "bool":
                    inpt = True if inpt.lower() == "true" else False
                    inptType = C.BOOL

        # Write
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        dest.value = inpt
        dest.type = inptType
        frames.updateVarInFrame(dest)

        return programInput


class WRITE(Instruction):
    """
    WRITE ⟨symb⟩\n
    Value output to standard output\n
    Prints the value of ⟨symb⟩ to standard output.
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "WRITE", args)


    def execute(self) -> None:
        env = Environment()

        if self.getArg(0).type == C.VAR:
            frames = env.Frames
            var = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
            varType = var.type
            var = var.value
        else:
            varType = self.getArg(0).type
            var = self.getArg(0).value

        # Print
        if varType == C.BOOL:
            print("true" if var else "false", end='')
        elif varType == C.NIL:
            print("", end='')
        else:
            print(var, end='')


class CONCAT(Instruction):
    """
    CONCAT ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩\n
    Concatenation of two strings\n
    Stores in the variable ⟨var⟩ the string created by the concatenation of two string operands ⟨symb1⟩ and ⟨symb2⟩
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "CONCAT", args)

    
    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        # arg1
        if (self.getArg(1).type == C.VAR):
            val1 = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, val1.type, [C.STR])
            val1 = val1.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.STR])
            val1 = self.getArg(1).value

        # arg2
        if (self.getArg(2).type == C.VAR):
            val2 = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            self._checkArgType(self.opcode, val2.type, [C.STR])
            val2 = val2.value
        else:
            self._checkArgType(self.opcode, self.getArg(2).type, [C.STR])
            val2 = self.getArg(2).value

        dest.type = C.STR
        dest.value = val1 + val2
        frames.updateVarInFrame(dest)


class STRLEN(Instruction):
    """
    STRLEN ⟨var⟩ ⟨symb⟩\n
    Find the length of the string\n
    Finds the number of characters (length) of the string in ⟨symb⟩ and this length is stored as an integer in ⟨var⟩.
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "STRLEN", args)

    
    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)

        if (self.getArg(1).type == C.VAR):
            val = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, val.type, [C.STR])
            val = val.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.STR])
            val = self.getArg(1).value

        dest.type = C.INT
        dest.value = len(val)
        frames.updateVarInFrame(dest)



class GETCHAR(Instruction):
    """
    GETCHAR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩\n
    Return the string character\n
    In ⟨var⟩, it stores a string from one character in the string ⟨symb1⟩ at position ⟨symb2⟩
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "GETCHAR", args)

    
    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        
        # arg1
        if self.getArg(1).type == C.VAR:
            source = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, source.type, [C.STR])
            source = source.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.STR])
            source = self.getArg(1).value

        # arg2
        if self.getArg(2).type == C.VAR:
            indexPos = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            self._checkArgType(self.opcode, indexPos.type, [C.INT])
            indexPos = indexPos.value
        else:
            self._checkArgType(self.opcode, self.getArg(2).type, [C.INT])
            indexPos = self.getArg(2).value

        try:
            if indexPos < 0:
                raise Exception
            newValue = source[indexPos]
        except Exception:
            printError(58, f"ERROR: Instruction GETCHAR: index value {indexPos} out of string {source} !")
        
        dest.value = newValue
        dest.type = C.STR
        frames.updateVarInFrame(dest)


class SETCHAR(Instruction):
    """
    SETCHAR ⟨var⟩ ⟨symb1⟩ ⟨symb2⟩\n
    Change the string character\n
    Modifies the character of the string stored in the variable ⟨var⟩ at position ⟨symb1⟩ to the character in the string ⟨symb2⟩
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "SETCHAR", args)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        self._checkArgType(self.opcode, dest.type, [C.STR])
        destValue = dest.value
        
        # arg1
        if self.getArg(1).type == C.VAR:
            indexPos = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, indexPos.type, [C.INT])
            indexPos = indexPos.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.INT])
            indexPos = self.getArg(1).value

        # arg2
        if self.getArg(2).type == C.VAR:
            source = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            self._checkArgType(self.opcode, source.type, [C.STR])
            source = source.value
        else:
            self._checkArgType(self.opcode, self.getArg(2).type, [C.STR])
            source = self.getArg(2).value

        try:
            newChar = source[0]
        except Exception:
            printError(58, f"ERROR: SETCHAR Empty string as source !")
        
        if indexPos >= len(destValue) or indexPos < 0:
            printError(58, f"ERROR: Instruction SETCHAR: index value {indexPos} out of string {destValue} !")
        
        destValue = destValue[:indexPos] + newChar + destValue[indexPos+1:]
        
        dest.value = destValue
        frames.updateVarInFrame(dest)


class TYPE(Instruction):
    """
    TYPE ⟨var⟩ ⟨symb⟩\n
    Find out the type of the given symbol\n
    It dynamically detects the symbol type ⟨symb⟩ and writes a string indicating this type to ⟨var⟩.
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "TYPE", args)

    
    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
        
        if self.getArg(1).type == C.VAR:
            sourceType = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            sourceType = sourceType.type
        else:
            sourceType = self.getArg(1).type

        match sourceType:
            case C.STR:
                dest.value = "string"
            case C.FLOAT:
                dest.value = "float"
            case C.INT:
                dest.value = "int"
            case C.BOOL:
                dest.value = "bool"
            case C.NIL:
                dest.value = "nil"
            case None:
                dest.value = ""

        dest.type = C.STR
        frames.updateVarInFrame(dest)


class LABEL(Instruction):
    """
    LABEL ⟨label⟩\n
    Definition of signs
    """

    def __init__(self, order:int, args:list[InstructionArgument]) -> None:
        super().__init__(order, "LABEL", args)


    def execute(self) -> None:
        pass
    

    def createLabel(self) -> None:
        env = Environment()
        env.addLabel(self.order, self.getArg(0).value)


class JUMP(Instruction):
    """
    JUMP ⟨label⟩\n
    Unconditional jump on the label
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "JUMP", args)

    
    def execute(self) -> int:
        env = Environment()
        label:Label = env.getLabel(self.getArg(0).value)
        return (label.position - 1)
    

class ConditionalJumps(Instruction):
    """
    JUMPIFEQ/JUMPIFNEQ ⟨label⟩ ⟨symb1⟩ ⟨symb2⟩\n
    Conditional jump on the sign in case of equality/inequality
    """

    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)

    
    def execute(self) -> int|None:
        env = Environment()
        frames = env.Frames
        # arg1
        if (self.getArg(1).type == C.VAR):
            val1 = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            val1Type = val1.type
            val1 = val1.value
        else:
            val1Type = self.getArg(1).type
            val1 = self.getArg(1).value

        # arg2
        if (self.getArg(2).type == C.VAR):
            val2 = frames.getVarFromFrame(self.getArg(2).value, self.getArg(2).frame)
            val2Type = val2.type
            val2 = val2.value
        else:
            val2Type = self.getArg(2).type
            val2 = self.getArg(2).value
        
        # Check arguments
        if val1Type == val2Type or (val1Type == C.NIL or val2Type == C.NIL):
            if self.opcode == "JUMPIFEQ" and val1 == val2:
                return env.getLabel(self.getArg(0).value).position
            elif self.opcode == "JUMPIFNEQ" and val1 != val2:
                return env.getLabel(self.getArg(0).value).position
            else:
                return None
        else:
            printError(53, f"ERROR: Instruction {self.opcode}: invalid argument types {val1Type} and {val2Type} !")
        


class EXIT(Instruction):
    """
    EXIT ⟨symbol⟩\n
    Termination of interpretation with return code
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "EXIT", args)


    def execute(self) -> None:
        env = Environment()        
        if self.getArg(0).type == C.VAR:
            frames = env.Frames
            var = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
            self._checkArgType(self.opcode, var.type, [C.INT])
            exitCode = var.value
        else:
            self._checkArgType(self.opcode, self.getArg(0).type, [C.INT])
            exitCode = self.getArg(0).value

        if 0 <= exitCode <= 49:
            exit(exitCode)
        else:
            printError(57, f"ERROR: Instruction EXIT: Invalid exit code {exitCode} !")
        


class DPRINT(Instruction):
    """
    DPRINT ⟨symb⟩\n
    Dump the value to stderr
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "DPRINT", args)


    def execute(self) -> None:
        env = Environment()
        if self.getArg(0).type == C.VAR:
            frames = env.Frames
            val = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)
            val = val.value
        else:
            val = self.getArg(0).value

        print(val, file=sys.stderr, end='')


class BREAK(Instruction):
    """
    BREAK\n
    Dump interpreter status to stderr
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "BREAK", args)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        print("******************************************************", file=sys.stderr)
        print(f"Current code order {self.order}", file=sys.stderr)
        print("******************************************************", file=sys.stderr)
        print(f"Current Global Frame Variables:", file=sys.stderr)
        for var,val in frames._getGF().items():
            print(f"{var} => {val.value}", file=sys.stderr)
        print("******************************************************", file=sys.stderr)
        print(f"Current Temporary Frame Variables:", file=sys.stderr)
        try:
            for var,val in frames._getTF().items():
                print(f"{var} => {val.value}", file=sys.stderr)
        except Exception:
            print("Uninitialized Temporary Frame", file=sys.stderr)
        print("******************************************************", file=sys.stderr)
        print(f"Current Local Frame Variables:", file=sys.stderr)
        cnt = 0
        for lf in frames._getLFs():
            print(f"Local Frame index: {cnt}", file=sys.stderr)
            cnt += 1
            for var,val in lf.items():
                print(f"{var} => {val.value}", file=sys.stderr)


class INT2FLOAT(Instruction):
    """
    INT2FLOAT ⟨var⟩ ⟨symb⟩\n
    Converting an integer to a float number\n
    According to Unicode, the numeric value ⟨symb⟩ is converted to a float assigned to ⟨var⟩.
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "INT2FLOAT", args)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)

        if self.getArg(1).type == C.VAR:
            source = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, source.type, [C.INT])
            source = source.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.INT])
            source = self.getArg(1).value

        newValue = float(source)        
        dest.value = newValue
        dest.type = C.FLOAT
        frames.updateVarInFrame(dest)
        

class FLOAT2INT(Instruction):
    """
    FLOAT2INT ⟨var⟩ ⟨symb⟩\n
    Converting an float to a integer number\n
    According to Unicode, the numeric value ⟨symb⟩ is converted to a integer assigned to ⟨var⟩.
    """

    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "FLOAT2INT", args)


    def execute(self) -> None:
        env = Environment()
        frames = env.Frames
        dest = frames.getVarFromFrame(self.getArg(0).value, self.getArg(0).frame)

        if self.getArg(1).type == C.VAR:
            source = frames.getVarFromFrame(self.getArg(1).value, self.getArg(1).frame)
            self._checkArgType(self.opcode, source.type, [C.FLOAT])
            source = source.value
        else:
            self._checkArgType(self.opcode, self.getArg(1).type, [C.FLOAT])
            source = self.getArg(1).value


        newValue = int(source)
        dest.value = newValue
        dest.type = C.INT
        frames.updateVarInFrame(dest)


#######################################################
################# Stack Instructions ##################
#######################################################

class CLEARS(Instruction):
    """
    CLEARS\n
    Delete all items/data from stack\n
    """
    
    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "CLEARS", args)

    
    def execute(self) -> None:
        env = Environment()
        env.clearDataStack()


class StackArithmeticInstructions(Instruction):
    """
    ADD/SUB/MUL/IDIV\n
    The arithmetics operation of two numerical values\n
    ADD/SUB/MUL/IDIV ⟨symb2⟩ and ⟨symb1⟩ and stores the resulting value top of the stack.
    """
    
    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    def execute(self) -> None:
        env = Environment()

        # Check type of value 2
        item = env.popDataStack()
        self._checkArgType(self.opcode, item[1], [C.INT, C.FLOAT])
        val2 = item[0]

        # Check type of value 1
        item = env.popDataStack()
        self._checkArgType(self.opcode, item[1], [C.INT, C.FLOAT])
        val1 = item[0]

        result = ArithmeticInstructions.ArithmeticOperation(self.opcode[:-1], val1, val2)
        env.pushDataStack([result[0], result[1]])


class StackRelationalInstructions(Instruction):
    """
    LT/GT/EQ\n
    Relational operators less, greater, equal\n
    The instruction evaluates the relational operator between ⟨symb2⟩ and ⟨symb1⟩ and stores the resulting on top of the stack.
    """
    
    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    def execute(self) -> None:
        env = Environment()
    
        # value 2
        item = env.popDataStack()
        val2 = item[0]
        val2Type = item[1]

        # value 1
        item = env.popDataStack()
        val1 = item[0]
        val1Type = item[1]

        result = RelationalInstructions.RelationalOperation(self.opcode[:-1], val1, val1Type, val2, val2Type)
        env.pushDataStack([result, C.BOOL])


class StackBooleanInstructions(Instruction):
    """
    AND/OR/NOT\n
    Basic Boolean operators\n
    Applies conjunction (logical AND)/disjunction (logical OR) to operands of type bool ⟨symb2⟩ and ⟨symb1⟩ or the negation of ⟨symb1⟩\n
    Resul value is pushed on top of the stack
    """
    
    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    def execute(self) -> None:
        env = Environment()

        # Check type of value 2
        item = env.popDataStack()
        self._checkArgType(self.opcode, item[1], [C.BOOL])
        val2 = item[0]

        if self.opcode != "NOTS":
            # Check type of value 1
            item = env.popDataStack()
            self._checkArgType(self.opcode, item[1], [C.BOOL])
            val1 = item[0]
        else:
            val1 = None

        result = BooleanInstructions.BooleanOperation(self.opcode[:-1], val1, val2)
        env.pushDataStack([result, C.BOOL])


class INT2CHARS(Instruction):
    """
    INT2CHAR\n
    Converting an integer to a character\n
    The numeric value ⟨symb1⟩ is converted to a character that forms a one-character string is pushed to stack ⟨var2⟩.
    """
    
    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "INT2CHARS", args)

    
    def execute(self) -> None:
        env = Environment()

        item = env.popDataStack()
        val = item[0]
        type = item[1]
        self._checkArgType(self.opcode, type, [C.INT])

        result = INT2CHAR.IntToChar(val)
        env.pushDataStack([result, C.STR])


class STRI2INTS(Instruction):
    """
    STRI2INT\n
    The ordinal value of the character\n
    Push to Stack the ordinal value of the character (according to Unicode) in the string ⟨symb2⟩ at the position ⟨symb1⟩.
    """
    
    def __init__(self, order: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, "STRI2INTS", args)

    
    def execute(self) -> None:
        env = Environment()

        # Check type of value 2
        item = env.popDataStack()
        self._checkArgType(self.opcode, item[1], [C.STR])
        val2 = item[0]

        # Check type of value 1
        item = env.popDataStack()
        self._checkArgType(self.opcode, item[1], [C.INT])
        val1 = item[0]

        result = STRI2INT.StrToInt(val2, val1)
        env.pushDataStack([result, C.INT])


class StackConditionalJumps(Instruction):
    """
    JUMPIFEQS/JUMPIFNEQS\n
    Conditional jump on the sign in case of equality/inequality
    """

    def __init__(self, order: str, opcode: str, args: list[InstructionArgument]) -> None:
        super().__init__(order, opcode, args)


    def execute(self) -> int:
        env = Environment()

        # Check type of value 2
        item = env.popDataStack()
        val2 = item[0]
        val2Type = item[1]

        # Check type of value 1
        item = env.popDataStack()
        val1 = item[0]
        val1Type = item[1]

        # Label to jump
        item = env.popDataStack()
        label = item[0]
        self._checkArgType(self.opcode, item[1], [C.LABEL, C.STR])

        # Check arguments
        if val1Type == val2Type or (val1Type == C.NIL or val2Type == C.NIL):
            if self.opcode == "JUMPIFEQS" and val1 == val2:
                return env.getLabel(label).position
            elif self.opcode == "JUMPIFNEQS" and val1 != val2:
                return env.getLabel(label).position
            else:
                return None
        else:
            printError(53, f"ERROR: Instruction {self.opcode}: invalid argument types {val1Type} and {val2Type} !")
##
# @file: InputProcessor.py
# @brief: classes and functions for interpret input files 
# @Author: Michal Ľaš
# @date: 01.04.2023


import sys
import re
import pathlib
import xml.etree.ElementTree as ET
from constants import Constants as C
from exitError import printError


class InputProcessor:
    """ 
    Singleton class used to represent (interpert.py) Source and Input
    ----------------------------------------------
    Attributes:
    sourceFile : str|None
        path/name of Source file
    inputFile : str|None
        path/name of Input file
    SourceData : ET.ElementTree
        Program Source (XML representation of IPPcode23)
    ----------------------------------------------
    Methods
    _validateFilePath : bool
        Validate path of input file
    loadSource : ET.ElementTree
        Process source given as file or on stdin to XML Elemten Tree
    validateXML : None
        Validate loaded XML program source
    """

    __instance = None

    def __new__(cls, sourceFile:str|None, inputFile:str|None):
        """
        Parameters
        ----------
        sourceFile : str|None
            path to source file or None if source is on STDIN
        inputFile : str|None
            path to input file or None if input is on STDIN
        """
        if cls.__instance is None:
            cls.__instance = super().__new__(cls)

            cls.sourceFile = sourceFile
            cls.inputFile = inputFile
            cls.SourceData = None

            if not cls._validateFilePath(sourceFile):
                printError(11, f"Unexisting source file: {sourceFile}")
            if not cls._validateFilePath(inputFile):
                printError(11, f"Unexisting input file: {inputFile}")

        return cls.__instance
        

    @classmethod
    def _validateFilePath(cls, path:str|None) -> bool:
        """
        Validate path of input file

        Parameters
        ----------
        path : str|None
            file path to be checked

        Return
        ----------
        True if file path is valid or file path is None
        False if file path is invalid
        """
        if path is None:
            return True
        else:
            file = pathlib.Path(path)
            if file.exists():
                return True
            else:
                return False


    def loadSource(self) -> ET.ElementTree:
        """
        Method load and process XML Source of program

        Parameters
        ----------
        None

        Return
        ----------
        XML Element Tree of given Source data
        """
        if self.sourceFile is None:
            self.SourceData= ET.parse(sys.stdin)
            return self.SourceData
        else:
            # open file
            try:
                try:
                    src = open(self.sourceFile, "r", encoding="utf-8")
                except OSError:
                    printError(11, f"ERROR: Unable to open source file: {self.sourceFile}")

                # parse file
                try:
                    self.SourceData = ET.parse(src)
                    src.close()
                    return self.SourceData
                except ET.ParseError:
                    src.close()
                    printError(31, "ERROR: Incorrect XML format in the source file !")
            except OSError:
                printError(31, "ERROR: Could not open/read source file !")


    def validateXML(self, xml:ET.ElementTree) -> None:
        """
        Method validate loaded program Source (XML Element Tree)
        In case of invalid XML representation of IPPcode23 method print error message to STDERR and exit with error code 32
        
        Parameters
        ----------
        xml : ET.ElementTree
            XML representation of Source program IPPcode23  

        Return
        ----------
        None
        """
        root = xml.getroot()

        # Check program
        
        if root.tag != "program":
            printError(32, "ERROR: Unexpected XML structure !")
        attribs = root.attrib.keys()
        if not("language" in attribs) or len(attribs - ["language", "name", "description"]) > 0:
            printError(32, "ERROR: Invalid attributes in program element !")
        elif not(re.match(r"(?i)(^ippcode23$)", root.get("language"))):
            printError(32, "ERROR: Invalid 'language' specification !")

        # Check instructions
        orderCounter = 0
        for instruction in root:
            if instruction.tag != "instruction":
                printError(32, "ERROR: Unexpected XML structure !")
            attribs = instruction.attrib.keys()
            if not("order" in attribs) or not("opcode" in attribs) or len(attribs) > 2:
                printError(32, "ERROR: Invalid instruction attributes !")
            elif (int(instruction.get("order")) <= orderCounter):
                printError(32, "ERROR: Invalid order argument value !")
            # Check arguments
            else:
                for argument in instruction:
                    if not(re.match(r"^(arg[123])$", argument.tag)):
                        printError(32, "ERROR: Unexpected XML structure !")
                    attribs = argument.attrib.keys()
                    if not("type" in attribs) or 0 < len(attribs) > 1:
                        printError(32, "ERROR: Invalid argument attributes !")

            orderCounter = int(instruction.get("order"))

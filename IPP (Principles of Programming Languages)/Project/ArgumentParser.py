##
# @file: ArgumentParser.py
# @brief: classes and functions for program argument parsing
# @Author: Michal Ľaš
# @date: 01.04.2023


import sys
import re
import argparse
from exitError import printError


class programArguments:
    """ 
    Singleton class used to represent arguments of program
    ----------------------------------------------
    Attributes:
    _args_dict : dict
        Dictionary of program arguments (name of argument => value)
    _args_list : list
        List with declared program arguments
    stats_args : list
        list with statistic argumest given in program arguments
    parser : ArgumentParser
        Class from module argparse used for parsing program arguments
    ----------------------------------------------
    Methods
    parseArgs : dict
        Fill _args_dict with program arguments and their values
    validateArgs : None
        Check if given arguments are correct according to specification
    getArgsDict : dict
        Return _args_dict (dictionary of program arguments and their values)
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

            cls._args_list = sys.argv[1:]

            # fill statistic arguments
            cntr = True
            for item in cls._args_list:
                if re.match(r'^(-s|--source|-i|--input)', item):
                    cntr = False
                elif cntr and not(re.match(r'^(--source=.*|--input=.*)', item)):
                    if '=' in item:
                        i = item.find('=')
                        cls.stats_args.append(item[:i])
                        cls.stats_args.append(item[i+1:])
                    else:
                        cls.stats_args.append(item)
                else:
                    cntr = True

            # set up arguments
            cls.parser = argparse.ArgumentParser(
            description="Loads the XML representation of the program and this program \
                        interprets and generates output using the input according to the command line parameters.", 
            epilog="At least one of the parameters --source or --input must always be specified. If one of them \
                    missing, the missing data is loaded from the standard input.")

            cls.parser.add_argument("-s", "--source", nargs=1, help="Input file with XML representation of the source code.", action="append")
            cls.parser.add_argument("-i", "--input", nargs=1, help="A file with inpputs for the actual interpretation of the specified source code.", action="append")
            cls.parser.add_argument("-t", "--stats", nargs=1, help="A file where statistic information will be placed.", action="append")
            cls.parser.add_argument("-n", "--insts", help="", action="store_true")
            cls.parser.add_argument("-o", "--hot", help="", action="store_true")
            cls.parser.add_argument("-v", "--vars", help="", action="store_true")
            cls.parser.add_argument("-f", "--frequent", help="", action="store_true")
            cls.parser.add_argument("-p", "--print", nargs=1, help="", action="append")
            cls.parser.add_argument("-e", "--eol", help="", action="store_true")

            cls._args = cls.parser.parse_args()

        return cls.__instance
    

    _args_dict = {}
    _args_list = []
    stats_args = []


    def parseArgs(self) -> dict:
        """
        Redistributes program arguments to _args_dict (program arguments dictionary)

        Parameters
        ----------
        None

        Return
        ----------
        Dictionary of parsed program arguments
        """
        try:
            for tuple in self._args._get_kwargs():
                if (type(tuple[1]) == bool):
                    self._args_dict[tuple[0]] = tuple[1]
                elif (tuple[1] is not None):
                    self._args_dict[tuple[0]] = tuple[1][0][0]
                else:
                    self._args_dict[tuple[0]] = None

            return self._args_dict
        except SystemExit:
            if "--help" in self._args_list:
                exit(0)
            else:
                exit(10)


    def validateArgs(self) -> None:
        """
        Validate program arguments.
        If any program argument is invalid, the function print error mesage to STDERR and calls exit() with exit code 10

        Parameters
        ----------
        None

        Return
        ----------
        None
        """
        if (self._args.source is None and self._args.input is None):
            printError(10, "ERROR: At least one of the --source or --input argument must be specified !")
        elif (self._args.source is not None and len(self._args.source) > 1 or self._args.input is not None and len(self._args.input) > 1):
            printError(10, "ERROR: Only one occurrence of the --source and --input arguments is allowed !")
        
        # check argument before stats
        if self.stats_args:
            if self.stats_args[0] != "--stats" and self.stats_args[0] != "-t":
                printError(10, "ERROR: Argument for preforming statistic declared before '--stats'/'-t' program argument !")

    def getArgsDict(self) -> dict:
        """
        Method to get program arguments.
        If arguments ware not parsed before (parseArgs()) then return empty dictionary

        Parameters
        ----------
        None

        Return
        ----------
        Dictionary of parsed program arguments
        """
        return self._args_dict

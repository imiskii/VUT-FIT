##
# @file: exitError.py
# @brief: classes and functions for handeling errors in interpret.py
# @Author: Michal Ľaš
# @date: 01.04.2023


import sys

def printError(code:int, *args) -> None:
    print(*args, file=sys.stderr)
    exit(code)
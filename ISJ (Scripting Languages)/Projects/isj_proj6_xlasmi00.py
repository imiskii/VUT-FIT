#!/usr/bin/env python3

class Polynomial:
    """ 
    Polynomial class. Represents a mathematical polynomial and methods manipulating with polynomials 
    ----------------
    Attributes:
    polynom : list
        representation of polymonial [n^0, n^1, n^2,...]
    ----------------
    Methods:
    __format_polynom_to_min_length:
        remove 0s at the end of self.polymon
    __repr__:
        Vector representation of polynom
    __str__:
        Transfrom polynom to it's human readable way
    __eq__:
        Checks if 2 polynoms are equal
    __add__:
        Addition of two polynoms
    __pow__:
        Count polynom to given power
    derivative:
        Count first derivation of polynom
    at_value:
        Count polynom value at x = num
    """
    def __init__(self, *plist, **xints) -> None:
        """
        Initialize polymonial 
        ----------------
        Arguments:
            plist : list|tuple
                values that initialize polymonial
            xints : dict
                values that initialize polymonial
        """
        self.polynom = [] # initialize polynom
        # it is either list or tuple of numbers
        if plist:
            if isinstance(plist[0], list):
                self.polynom = plist[0]
            else:
                for num in plist:
                    if not isinstance(num, int):
                        raise TypeError(f"Expected type int, but got {type(num)} in Polynomial parameter {num}")
                    self.polynom.append(num)
        # it is dictionary with values
        else:
            for key,val in xints.items():
                # get index
                try:
                    index = int(key.replace('x', ''))
                except ValueError or TypeError as e:
                    raise Exception(f"Unable to get index of polynom parameter {key}: {e}")
                # check value
                if not isinstance(val, int):
                    raise TypeError(f"Expected type int, but got {type(val)} in Polynomial parameter {val}")
                # resizing list (fill space with 0)
                for p_index in range(len(self.polynom), index + 1):
                    self.polynom.append(0)
                self.polynom[index] = val

        self.__format_polynom_to_min_length()
            

    def __format_polynom_to_min_length(self):
        """ Remove zeros at the od of polymonial """
        if len(self.polynom) >= 1:
            while self.polynom[-1] == 0 and len(self.polynom) > 1:
                del self.polynom[-1]
            if self.polynom[0] == 0 and len(self.polynom) == 1:
                del self.polynom[0]



    def __repr__(self) -> str:
        """ Vector representation of polynom """
        return str(self.polynom)
    

    def __str__(self) -> str:
        """ Transfrom polynom to it's human readable way """
        result = "" # initialize result string
        # empty polynom
        if len(self.polynom) == 0:
            return "0"
        # write polynom
        else:
            for index, val in enumerate(self.polynom):
                if val == 0:
                    continue
                # write power
                if index > 1:
                    result = "^" + str(index) + result
                # write number
                if index != 0:
                    result = "x" + result
                if index == 0 or abs(val) != 1:
                    result = str(abs(val)) + result
                # write sign (expect on last index)
                if index != len(self.polynom) - 1:
                    if val < 1:
                        result = " - " + result
                    else:
                        result = " + " + result
                # last value with '-'
                else:
                    if val < 1:
                        result = "-" + result
            return result


    def __eq__(self, second_p) -> bool:
        """ Checks if 2 polynoms are equal """
        # check if it is polynom
        if isinstance(second_p, Polynomial):
            # check size
            if len(self.polynom) != len(second_p.polynom):
                return False
            # compare all values in polynoms
            for index,val in enumerate(self.polynom):
                if val != second_p.polynom[index]:
                    return False
            # everithing is OK
            return True
        # second_p is not polynom
        else:
            second_p = Polynomial(second_p)
            return second_p == self
        
    
    def __add__(self, second_p):
        """ Addition of two polynoms """
        # check if it is polynom
        if isinstance(second_p, Polynomial):
            # Determine shorter and longer polynom
            if len(self.polynom) > len(second_p.polynom):
                longer = self.polynom
                shorter = second_p.polynom
            else:
                longer = second_p.polynom
                shorter = self.polynom
            # list of result values
            result = [val for val in longer]
            for index,val in enumerate(shorter):
                result[index] += val
            # make it polynom
            result = Polynomial(result)
            return result
        # second_p is not polynom
        else:
            second_p = Polynomial(second_p)
            return second_p + self


    def __pow__(self, power):
        """ Count polynom to given power """
        if power == 0:
            return Polynomial([1])
        elif power == 1:
            return self
        else:
            poly_copy = self.polynom.copy()
            for pw in range(2,power+1):
                # initialize result list
                result = [0] * ((len(self.polynom)-1)*pw+1)
                # multyply and add each
                for indx_1,val_1 in enumerate(self.polynom):
                    for indx_2, val_2 in enumerate(poly_copy):
                        result[indx_1+indx_2] += (val_1 * val_2)
                poly_copy = result.copy()
            # make it polynom
            result = Polynomial(result)
            return result



    def derivative(self):
        """ Count first derivation of polynom """
        # copy polynom list
        result = self.polynom.copy()
        # delete first (so x^0)
        del result[0]
        for index,val in enumerate(result):
            result[index] = val * (index+1)
        # make it polynom
        result = Polynomial(result)
        return result
    

    def at_value(self, num:int|float, *nums:int|float) -> int|float:
        """ 
        Count polynom value at x = num or if the second arguments is given count substraction of polymonial valiu at x = nums - x = num
        ----------------
        Arguments:
        num : int|float
            count value at 'num'
        nums : int|float
            if given substract from value at value at num
        """
        result = 0
        sec_result = 0
        for index,val in enumerate(self.polynom):
            result += (num**index) * val
        if nums:
            for index,val in enumerate(self.polynom):
                sec_result += (nums[0]**index) * val
            result = sec_result - result
        return result


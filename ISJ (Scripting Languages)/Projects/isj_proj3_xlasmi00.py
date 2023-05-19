#!/usr/bin/env python3


def first_odd_or_even(numbers):
    """Returns 0 if there is the same number of even numbers and odd numbers
       in the input list of ints, or there are only odd or only even numbers.
       Returns the first odd number in the input list if the list has more even
       numbers.
       Returns the first even number in the input list if the list has more odd 
       numbers.

    >>> first_odd_or_even([2,4,2,3,6])
    3
    >>> first_odd_or_even([3,5,4])
    4
    >>> first_odd_or_even([2,4,3,5])
    0
    >>> first_odd_or_even([2,4])
    0
    >>> first_odd_or_even([3])
    0
    """
    odd = []
    even = []
    for num in numbers:
        odd.append(num) if num % 2 == 1 else even.append(num)

    len_odd = len(odd)
    len_even = len(even)
    if len_odd == len_even or not len_odd or not len_even:
        return 0
    elif len_even > len_odd:
        return odd[0]
    else:
        return even[0]



def to_pilot_alpha(word):
    """Returns a list of pilot alpha codes corresponding to the input word

    >>> to_pilot_alpha('Smrz')
    ['Sierra', 'Mike', 'Romeo', 'Zulu']
    """

    pilot_alpha = ['Alfa', 'Bravo', 'Charlie', 'Delta', 'Echo', 'Foxtrot',
        'Golf', 'Hotel', 'India', 'Juliett', 'Kilo', 'Lima', 'Mike',
        'November', 'Oscar', 'Papa', 'Quebec', 'Romeo', 'Sierra', 'Tango',
        'Uniform', 'Victor', 'Whiskey', 'Xray', 'Yankee', 'Zulu']

    alphabet = {}
    for alpha in pilot_alpha:
        alphabet[alpha[0].lower()] = alpha

    pilot_alpha_list = [alphabet.get(letter) for letter in word.lower()]
     
    return pilot_alpha_list


if __name__ == "__main__":
    import doctest
    doctest.testmod()

#!/usr/bin/env python3

import collections

my_counter = collections.Counter()


def log_and_count(key=None, counts=None):
    if counts is None:
        raise TypeError('log_and_count() missing 1 required positional argument: \'counts\'')
    def outer(func):
        def inner(*args, **kwargs):
            print('called ' + func.__name__ + ' with {} and {}'.format(args, kwargs))
            ret = func(*args, **kwargs)
            counts[key_arg] += 1
            return ret
        if key is None:
            key_arg = func.__name__
        else:
            key_arg = key
        return inner
    return outer



@log_and_count(key = 'basic functions', counts = my_counter)
def f1(a, b=2):
    return a ** b

@log_and_count(key = 'basic functions', counts = my_counter)
def f2(a, b=3):
    return a ** 2 + b

@log_and_count(counts = my_counter)
def f3(a, b=5):
    return a ** 3 - b


f1(2)
f2(2, b=4)
f1(a=2, b=4)
f2(4)
f2(5)
f3(5)
f3(5,4)

print(my_counter)
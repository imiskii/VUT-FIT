# Parallel and Distributed Algorithms

Both project are utilizing [OpenMPI C++ library](https://www.open-mpi.org/faq/?category=mpi-apps)

## Project 1

Algorithm for sorting numbers [Odd-even sort](https://en.wikipedia.org/wiki/Odd%E2%80%93even_sort). The algorithm can be run using the ```test.sh``` script, which uses the ```dd``` utility to create a file with set of bytes, each byte representing a single number from 0-255. The output is an ordered sequence of numbers (one number on each line).

## Project 2

Algorithm for determining the level of vertices in a given binary tree. The algorithm uses Euler tour, Adjacency list and Suffix sum algorithms. The algorithm can be used with a ```test.sh``` script, which must be given a tree in the form of a string e.g. the string ```ABC``` represents a tree, where ```A``` is the root and ```BC``` is its two leaves.

```
./test.sh ABC
A:0,B:1,C:1
```
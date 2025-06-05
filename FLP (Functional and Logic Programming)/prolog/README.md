# Rubik's Cube Solver

**Author:** Michal Ľaš (xlasmi00)  
**Date:** 11.04.2025

---

### Solution description

The solution consists of two parts (+ input processing). **In the first part are defined all possible rotations of the cube**, i.e. all 9 possible clockwise rotations and the counterclockwise (reverse) rotations. Each rotation has a label, the algorithm for finding the solution creates a list of these labels, which represents solution steps for the cube solution. I decided to store the rotation labels instead of the whole cube states for easier debugging (it is more straightforward to follow the steps). So the rotations for the cube look as follows:

```
rotate_cube(<label>, <cube>, <rotated_cube>)
```

It is easy to see that if the input cube is placed to `<rotated_cube>`, then in `<cube>` will be reverse rotation. That is how all reverse rotations are made.


**In the second part is defined the actual algorithm for finding the solution of the Rubik's cube.** The algorithm works on the principle of a systematic level-by-level search. Where level in this case is the number of rotations needed to solve the cube. I chose this approach because it is very straightforward and easy to implement in the Prolog programming language. However, the problem is repetitive states that are unnecessarily searched, which gets worse with reaching deeper levels. This algorithm is complete, so it always finds a solution.  

The algorithm works as follows:
1. Check if the cube is solved
2. Try to solve the cube in one rotation, if cube is solved return the solving rotation (find the rotation that will lead from the given cube to the solved one)
3. Go to the next level. Select a rotation of the given cube and repeat the step 2., repeat until all rotations have been tried, then repeat step 3.


### Usage example

The various test inputs are in the `./tests/inputs` directory. The tests are labeled as: `<num_of_steps>_move_<id>.in`. Or some specific labels, but all test inputs must have `.in` extension. There is also a test for each rotation to verify that all defined rotations are correct.

```
./flp24-log < ./tests/inputs/assignment_example.in
553
553
554
225 322 644 111
115 322 633 444
115 322 633 444
662
661
661

555
555
555
222 333 444 111
111 222 333 444
111 222 333 444
666
666
666

555
555
555
111 222 333 444
111 222 333 444
111 222 333 444
666
666
666
```

```
./flp24-log < ./tests/inputs/already_solved.in
555
555
555
111 222 333 444
111 222 333 444
111 222 333 444
666
666
666
```

### Tests

There are also automatic test that can be run with `make test`. **Python3 is required**.

```
make test
python3 ./tests/test.py ./flp24-log ./tests/inputs
[PASS]: 1_move_1.in, runtime: 0.02088s
[PASS]: 1_move_2.in, runtime: 0.01920s
[PASS]: 1_move_3.in, runtime: 0.01939s
[PASS]: 1_move_4.in, runtime: 0.01907s
[PASS]: 1_move_5.in, runtime: 0.02243s
[PASS]: 1_move_6.in, runtime: 0.02146s
[PASS]: 1_move_7.in, runtime: 0.02327s
[PASS]: 1_move_8.in, runtime: 0.02541s
[PASS]: 1_move_9.in, runtime: 0.02134s
[PASS]: 2_move_1.in, runtime: 0.02350s
[PASS]: 3_move_1.in, runtime: 0.02372s
[PASS]: 4_move_1.in, runtime: 0.04728s
[PASS]: 5_move_1.in, runtime: 0.20686s
[PASS]: 6_move_1.in, runtime: 3.40319s
[PASS]: 7_move_1.in, runtime: 56.92440s
[PASS]: already_solved.in, runtime: 0.02132s
[PASS]: assignment_example.in, runtime: 0.02433s
```

## Alternative

The previous solution had the problem of processing states that had been processed before, so I tried to improve the solution by storing states that had already been processed and checking that they were not processed repeatedly.

For this solution I used dynamic predicates. In the dynamic predicate `visited(Cube)`, I stored a **hash** of the cube state that had already been processed.

Next, I performed a simple **BFS** using a queue. Where in the queue were stored pairs [_cube state_, _path from initial cube to this state_]. This way when a solution is found, it is enough to just reverse the stored path.

The solution is complete, but according to the tests it is much slower than the previous solution. This is probably due to the fact that a lot of data is stored and memory slows down the computation. Anyway, I wanted to give this solution since I already made the effort to make it.


An alternative solution can be built using `make alt` and tested with `make test` afterwards.

Output tests of the alternative solution (only after the cube, which can be solved in 4 rotations, since the tests took too long, at the same time for replication it will be necessary to overwrite the maximum time for one test (TIMEOUT constant) in the [test script](tests/test.py)):

```
python3 ./tests/test.py ./flp24-log ./tests/inputs
[PASS]: 1_move_1.in, runtime: 0.01623s
[PASS]: 1_move_2.in, runtime: 0.01808s
[PASS]: 1_move_3.in, runtime: 0.01993s
[PASS]: 1_move_4.in, runtime: 0.01988s
[PASS]: 1_move_5.in, runtime: 0.01861s
[PASS]: 1_move_6.in, runtime: 0.02111s
[PASS]: 1_move_7.in, runtime: 0.02266s
[PASS]: 1_move_8.in, runtime: 0.02143s
[PASS]: 1_move_9.in, runtime: 0.02267s
[PASS]: 2_move_1.in, runtime: 0.02536s
[PASS]: 3_move_1.in, runtime: 1.13394s
[PASS]: 4_move_1.in, runtime: 241.28668s
```
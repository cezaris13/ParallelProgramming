# Parallel programming laboratory assignments which were done in Vilnius university 2022 Autumn semester.
The repository constists of the laboratory work (described below) and some additional tasks to learn openMP and MPI for c++. Could not find the description of the practice tasks. 

### Introduction of laboratory work

The set $A$ consists of geographic points with latitude and longitude coordinates. From this set, a set of points $X$ must be chosen such that the sum of the distances from each point in $A$ to its nearest point in $X$ is minimal, where $X \subset A$.

The file `lab_data.dat` contains 50000 geographic points, where one line describes the coordinates of one geographic point.

The file `lab_01_2_algorithm.cpp` contains a program that finds a set $X$ of given $n$ points satisfying the constraint of the problem using the *Pure Random Search* (PRS) algorithm.

#### The main parameters of the algorithm (global variables):
- `num_points`: size of the data set $A$ (max 50000)
- `num_variables`: size of the set $X$ of points to be searched
- `num_iterations`: number of iterations to find a solution (the more iterations, the better the probability of finding a better solution)

At the start of the algorithm, a distance matrix is created, which stores the distances in kilometres between the points, calculated according to the Haversine formula. Given that the distance from point $a$ to point $b$ is equal to the distance from point $b$ to point $a$, only half of the matrix is filled. The distances stored in this matrix are used to search for points in the set $X$.

### 1 Task
1. Choose the size of the dataset and the number of iterations of the algorithm so that the calculation of the distance matrix takes at least 10 seconds and the time to find the solution is at least 20 seconds.
2. Consider data loading and distance matrix computation as the sequential part of the algorithm and solution search as the parallel part. Evaluate the theoretical possible speed-ups of the algorithm using 2 and 4 processors and the maximum possible speed-up.
3. Consider data loading and distance matrix computation as a sequential part of the algorithm, design a parallel shared memory algorithm and experimentally investigate its speedup using 2 and 4 processors.
4. Design a parallel shared memory algorithm for distance matrix computation and experimentally investigate its speedup using 2 and 4 processors.
5. Investigate the dependence of the algorithm's speedup on the number of processors when the matrix computation and the solution search are parallelised.

### 2 Task
Transform the shared-memory parallel algorithm into a distributed-memory algorithm and investigate its speedup using 2, 4 and 8 processors (using the computing resources of uosis.mif.vu.lt). In your report, describe how you have distributed the work among the compute nodes and how the communication between them has taken place.

In a single graph, show the linear speedup and the experimentally determined dependencies of the speedup of the distance matrix calculation, the speedup of the solution calculation, and the speedup of the whole algorithm (when the matrix calculation and the search for the solution are aligned) on the number of processors.

### 3 Task

Run the resulting distributed memory parallel algorithm on a MIF supercomputer and perform the computation using 2, 4 and 8 or more processors.

In a single graph, plot the linear speedup and the experimentally determined speedup of the distance matrix calculation, the speedup of the solution calculation, and the speedup of the whole algorithm (when the matrix calculation and the search for the solution are aligned) versus the number of processors.

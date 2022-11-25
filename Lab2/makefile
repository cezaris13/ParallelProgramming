##
# Project Title
#
# @file
# @version 0.1
build:
	mpic++ lab2.cpp -o program.o
run:
	mpic++ lab2.cpp -o program.o
	mpirun --mca opal_warn_on_missing_libcuda 0 -n 2 program.o
rundebug:
	mpic++ lab2.cpp -o program.o
# 	g++  lab2.cpp -o program.o
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	mpirun -n 2 program.o >> responses.txt
	./check_if_deterministic.sh
runbenchmark:
	mpic++ lab2.cpp -o program.o
	# g++-12 -fopenmp lab1.cpp -o program.o
# 	g++-12  lab1.cpp -o program.o
	echo -e "2 cores \n" >> results.txt
	mpirun -n 2 program.o >> results.txt
	mpirun -n 2 program.o >> results.txt
	mpirun -n 2 program.o >> results.txt
	echo -e "4 cores \n" >> results.txt
	mpirun -n 4 program.o >> results.txt
	mpirun -n 4 program.o >> results.txt
	mpirun -n 4 program.o >> results.txt
	echo -e "8 cores \n" >> results.txt
	mpirun -n 8 program.o >> results.txt
	mpirun -n 8 program.o >> results.txt
	mpirun -n 8 program.o >> results.txt

# end

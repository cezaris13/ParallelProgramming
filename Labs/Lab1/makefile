mainFile = lab1.cpp
output = lab1.o
resultsFile = responses.txt

build:
	g++ -fopenmp ${mainFile} -o ${output}
run:
	g++ -fopenmp ${mainFile} -o ${output}
	./output
rundebug:
	g++ -fopenmp ${mainFile} -o ${output}
	for ((i=1; i <= 9; ++i)) do \
		./${output} >> ${resultsFile}; \
	done
	./check_if_deterministic.sh

runBenchmark:
	g++-12 -fopenmp ${mainFile} -o ${output}
	for ((i=1; i <= 3; ++i)) do \
		./${output}; \
	done

mainFile = lab2.cpp
output =  program.o
resultsFile = results.txt
powers = 2 4 8

build:
	mpic++ ${mainFile} -o  ${output}

run:
	mpic++ ${mainFile} -o  ${output}
	mpirun --mca opal_warn_on_missing_libcuda 0 -n 2  ${output}

runBenchmark:
	# mpic++ ${mainFile} -o  ${output}
	for i in ${powers}; \
	do \
		echo "$$i cores \n"; >> ${resultsFile}; \
		for ((j=1; j <= 6; ++j)) do \
			mpirun -n $$i ${output} >> ${resultsFile} \
			echo "aa"; \
		done \
	done

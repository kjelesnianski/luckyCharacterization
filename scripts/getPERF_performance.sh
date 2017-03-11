#!/bin/bash
# Records Various PERF DATA
# >>all LLC misses (Load misses, Store misses, Prefetch misses)
# >>performance in MIPS

#!!!!! IMPORTANT
# Perf stat - provides an overall count of the events
# Perf record - performs a sampling recording where those hardware events occur
# http://developerblog.redhat.com/2014/03/10/determining-whether-an-application-has-poor-cache-performance-2/
#http://www.bernardi.cloud/2012/08/07/playing-around-with-perf/

#LLCMS = LLC misses per second
#MIPS = Millions of instr per second

#FORMAT IS: BENCHMARK,NumThreads,RUN,LLC-load-misses,LLC-store-misses,LLC-prefetch-misses,Cache-miss(ARM),total-instructions,time,LLCMS,MIPS

WORKDIR=/home/bielsk1/___luckyCharacterization
ARCH=$(uname -m)
echo "I am $ARCH!"

SCRIPTS=$WORKDIR/scripts
PERF=perf
RESULTS=$WORKDIR/results
TMP=curr.txt

#benchmark stuff
SUITE=npb
#CLASS=(A B C)
#BENCHMARKS=(bt cg dc ep ft is lu mg sp ua)
N_THREADS=(1 2 4 6)
CLASS=(A)
BENCHMARKS=(cg)
#N_THREADS=(6)

BENCHPATH=$WORKDIR/benchmarks/SNU_NPB-1.0.3/NPB3.3-OMP-C/bin

#echo "CLASS:${CLASS[*]}"
#echo "BENCH:${BENCHMARKS[*]}"

for class in ${CLASS[*]}; do
  # create new log file
  log=${RESULTS}/data.${SUITE}.${class}.csv
  # CLEAN DIR
  #rm -rf ${RESULTS}/*.csv
  #touch $log
  for b in ${BENCHMARKS[*]}; do
		for threadnum in ${N_THREADS[*]}; do
	    for run in `seq 1 10`; do
				echo ">Status CLASS:$class, BENCH:$b, RUN:$run"
			
				#run perf stat
				if [ "$ARCH" == "x86_64" ]
				then
					echo "ARCH IS: x86_64"
					OMP_NUM_THREADS=$threadnum $SCRIPTS/$PERF stat -a -e LLC-load-misses,LLC-store-misses,LLC-prefetch-misses,task-clock,cpu-clock,instructions --output $TMP $BENCHPATH/$b.$class.x
				elif [ "$ARCH" == "aarch64" ]
				then
					echo "ARCH IS: aarch64"
					#Events specific to XGENE!!!!
					OMP_NUM_THREADS=$threadnum $SCRIPTS/$PERF stat -a -e cache-misses,L1-dcache-load-misses,L1-dcache-store-misses,instructions --output $TMP $BENCHPATH/$b.$class.x OMP_NUM_THREADS=$threadnum
				else
					echo "[ERROR]: Unsupported Architecture"
					exit 1
				fi
				#Parse
				#Call Python Script actually
				python parsePerf.py $TMP $b $run $threadnum $ARCH
			done # ITERATIONS
		done # OMP_NUM_THREADS
  done # done benchmarks
done # done CLASS


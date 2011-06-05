DIR=$(shell pwd)
CC=g++
CFLAGS=-g -I$(DIR)/include -L$(DIR)/lib -lgsl -lgslcblas -lrt -Wall \
		-DTESTSIZE=30000000 -DITERATIONS=1
DATE = $(shell date +'%y%m%d%H%M%S')
GSL_RNG_SEED=110604190045 #$(DATE)
GSL_RNG_TYPE=taus

ENV= GSL_RNG_SEED=$(GSL_RNG_SEED) GSL_RNG_TYPE=$(GSL_RNG_TYPE)
LD_LIBRARY_PATH=$(DIR)/lib

BUCKETTYPES = map btree sorted unsorted
BUCKETSIZES = 32 64 128 256 512 1024 2048 4096
NUMTHREADS  = 1 2 4 8 16 32

all: seq_tests ts_tests
run: seq_run ts_run

# Sequential tests
seq_run: seq_tests
	cat /proc/cpuinfo > ./logs/seq_$(DATE).log
	$(ENV) find ./tests/bin/seq/ -type f -executable | sed 's/.*/time -v & >> .\/logs\/seq_$(DATE).log 2>\&1/g' | /bin/sh

seq_tests: seq_map seq_trie 
seq_map: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -o tests/bin/seq/$@

seq_exec = $(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_$(btype)_bucket \
		   -DBUCKETSIZE=$(bsize)  -o tests/bin/seq/seq_trie_$(btype)_$(bsize) ;
seq_trie: tests/seq_test.c++
	$(foreach bsize, $(BUCKETSIZES), $(foreach btype, $(BUCKETTYPES), $(seq_exec)))

# Threaded tests
ts_tests: ts_random ts_shakespeare
ts_run: ts_tests
	cat /proc/cpuinfo > ./logs/ts_$(DATE).log
	$(ENV) find ./tests/bin/ts/ -type f -executable | sed 's/.*/time -v & >> .\/logs\/ts_$(DATE).log 2>\&1/g' | /bin/sh

ts_exec = $(CC) $(CFLAGS) tests/ts_test.c++ -DBUCKETTYPE=ts_$(btype)_bucket -DNODETYPE=ts_locked_node_2 \
		-DBUCKETSIZE=$(bsize) -DNUM_THREADS=$(nt) -o tests/bin/ts/ts_b$(btype)_$(bsize)_t$(nt)
ts_random: tests/ts_test.c++
	$(foreach bsize, $(BUCKETSIZES), $(foreach btype, $(BUCKETTYPES), $(foreach nt, $(NUMTHREADS), $(ts_exec) ; )))

shakespeare_exec = $(ts_exec)_shakespeare -DDATASET
ts_shakespeare: tests/ts_test.c++
	$(foreach bsize, $(BUCKETSIZES), $(foreach btype, $(BUCKETTYPES), $(foreach nt, $(NUMTHREADS), $(shakespeare_exec) ; )))

clean:
	rm -r -f tests/bin/
	mkdir tests/bin
	mkdir tests/bin/seq
	mkdir tests/bin/ts

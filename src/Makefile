DIR=$(shell pwd)
CC=g++
CFLAGS=-g -I$(DIR)/include -L$(DIR)/lib -lgsl -lgslcblas -lrt -Wall \
		-DTESTSIZE=1000000 -DITERATIONS=3
DATE = $(shell date +'%y%m%d%H%M%S')
GSL_RNG_SEED=$(DATE)
GSL_RNG_TYPE=taus

ENV= GSL_RNG_SEED=$(GSL_RNG_SEED) GSL_RNG_TYPE=$(GSL_RNG_TYPE)
LD_LIBRARY_PATH=$(DIR)/lib

# Sequential tests
seq_run: seq_tests
	cat /proc/cpuinfo > ./logs/seq_$(DATE).log
	$(ENV) find ./tests/bin/seq/ -type f -executable | sed 's/.*/time -v & >> .\/logs\/seq_$(DATE).log 2>\&1/g' | /bin/sh

seq_tests: seq_map seq_trie 
seq_map: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -o tests/bin/test_map
seq_trie: seq_trie_map seq_trie_unsorted seq_trie_sorted seq_trie_btree

seqsorted=tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_sorted_bucket
seq_trie_sorted: tests/seq_test.c++
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=32   -o tests/bin/seq/seq_trie_sorted_32
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=64   -o tests/bin/seq/seq_trie_sorted_64
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=128  -o tests/bin/seq/seq_trie_sorted_128
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=256  -o tests/bin/seq/seq_trie_sorted_256
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=512  -o tests/bin/seq/seq_trie_sorted_512
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=1024 -o tests/bin/seq/seq_trie_sorted_1024
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=2048 -o tests/bin/seq/seq_trie_sorted_2048
	$(CC) $(CFLAGS) $(seqsorted) -DBUCKETSIZE=4096 -o tests/bin/seq/seq_trie_sorted_4096

sequnsorted=tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket
seq_trie_unsorted: tests/seq_test.c++
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=32   -o tests/bin/seq/seq_trie_unsorted_32
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=64   -o tests/bin/seq/seq_trie_unsorted_64
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=128  -o tests/bin/seq/seq_trie_unsorted_128
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=256  -o tests/bin/seq/seq_trie_unsorted_256
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=512  -o tests/bin/seq/seq_trie_unsorted_512
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=1024 -o tests/bin/seq/seq_trie_unsorted_1024
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=2048 -o tests/bin/seq/seq_trie_unsorted_2048
	$(CC) $(CFLAGS) $(sequnsorted) -DBUCKETSIZE=4096 -o tests/bin/seq/seq_trie_unsorted_4096

seqmap=tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_map_bucket
seq_trie_map: tests/seq_test.c++
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=32   -o tests/bin/seq/seq_trie_map_32
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=64   -o tests/bin/seq/seq_trie_map_64
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=128  -o tests/bin/seq/seq_trie_map_128
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=256  -o tests/bin/seq/seq_trie_map_256
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=512  -o tests/bin/seq/seq_trie_map_512
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=1024 -o tests/bin/seq/seq_trie_map_1024
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=2048 -o tests/bin/seq/seq_trie_map_2048
	$(CC) $(CFLAGS) $(seqmap) -DBUCKETSIZE=4096 -o tests/bin/seq/seq_trie_map_4096

seqbtree=tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_btree_bucket
seq_trie_btree: tests/seq_test.c++
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=32   -o tests/bin/seq/seq_trie_btree_32
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=64   -o tests/bin/seq/seq_trie_btree_64
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=128  -o tests/bin/seq/seq_trie_btree_128
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=256  -o tests/bin/seq/seq_trie_btree_256
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=512  -o tests/bin/seq/seq_trie_btree_512
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=1024 -o tests/bin/seq/seq_trie_btree_1024
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=2048 -o tests/bin/seq/seq_trie_btree_2048
	$(CC) $(CFLAGS) $(seqbtree)	-DBUCKETSIZE=4096 -o tests/bin/seq/seq_trie_btree_4096


shakespeare= tests/ts_test.c++ -DDATASET=./datasets/shakespeare.txt -DBUCKETTYPE=ts_map_bucket -DNODETYPE=ts_locked_node_2
shakespeare_ts: tests/ts_test.c++
	$(CC) $(CFLAGS) $(shakespeare) -DBUCKETSIZE=32 -DNUM_THREADS=1 -o tests/bin/ts/shakespeare_ts_1
	$(CC) $(CFLAGS) $(shakespeare) -DBUCKETSIZE=32 -DNUM_THREADS=2 -o tests/bin/ts/shakespeare_ts_2
	$(CC) $(CFLAGS) $(shakespeare) -DBUCKETSIZE=32 -DNUM_THREADS=4 -o tests/bin/ts/shakespeare_ts_4
	$(CC) $(CFLAGS) $(shakespeare) -DBUCKETSIZE=32 -DNUM_THREADS=8 -o tests/bin/ts/shakespeare_ts_8
	$(CC) $(CFLAGS) $(shakespeare) -DBUCKETSIZE=32 -DNUM_THREADS=32 -o tests/bin/ts/shakespeare_ts_32
	$(CC) $(CFLAGS) $(shakespeare) -DBUCKETSIZE=32 -DNUM_THREADS=1024 -o tests/bin/ts/shakespeare_ts_1024

t_btree_flags=-DBUCKETSIZE=32 -DBUCKETTYPE=ts_btree_bucket -DNODETYPE=ts_locked_node_2 tests/ts_test.c++
thread_test_btree: tests/ts_test.c++
	$(CC) $(CFLAGS) $(t_btree_flags) -DNUM_THREADS=1 -o tests/bin/thread_test_btree_1
	$(CC) $(CFLAGS) $(t_btree_flags) -DNUM_THREADS=2 -o tests/bin/thread_test_btree_2
	$(CC) $(CFLAGS) $(t_btree_flags) -DNUM_THREADS=4 -o tests/bin/thread_test_btree_4
	$(CC) $(CFLAGS) $(t_btree_flags) -DNUM_THREADS=8 -o tests/bin/thread_test_btree_8
	$(CC) $(CFLAGS) $(t_btree_flags) -DNUM_THREADS=16 -o tests/bin/thread_test_btree_16
	$(CC) $(CFLAGS) $(t_btree_flags) -DNUM_THREADS=32 -o tests/bin/thread_test_btree_32

thread_test_btree_run: thread_test_btree
	$(ENV) ./tests/bin/thread_test_btree_1
	$(ENV) ./tests/bin/thread_test_btree_2
	$(ENV) ./tests/bin/thread_test_btree_4
	$(ENV) ./tests/bin/thread_test_btree_8
	$(ENV) ./tests/bin/thread_test_btree_16
	$(ENV) ./tests/bin/thread_test_btree_32

t_map_flags=-DBUCKETSIZE=256 -DNODETYPE=ts_locked_node_2 -DBUCKETTYPE=ts_map_bucket tests/ts_test.c++
thread_test_map: tests/ts_test.c++
	$(CC) $(CFLAGS) $(t_map_flags) -DNUM_THREADS=1 -o tests/bin/thread_test_map_1
	$(CC) $(CFLAGS) $(t_map_flags) -DNUM_THREADS=2 -o tests/bin/thread_test_map_2
	$(CC) $(CFLAGS) $(t_map_flags) -DNUM_THREADS=4 -o tests/bin/thread_test_map_4
	$(CC) $(CFLAGS) $(t_map_flags) -DNUM_THREADS=8 -o tests/bin/thread_test_map_8
	$(CC) $(CFLAGS) $(t_map_flags) -DNUM_THREADS=16 -o tests/bin/thread_test_map_16
	$(CC) $(CFLAGS) $(t_map_flags) -DNUM_THREADS=32 -o tests/bin/thread_test_map_32

thread_test_map_run: thread_test_map
	$(ENV) ./tests/bin/thread_test_map_1
	$(ENV) ./tests/bin/thread_test_map_2
	$(ENV) ./tests/bin/thread_test_map_4
	$(ENV) ./tests/bin/thread_test_map_8
	$(ENV) ./tests/bin/thread_test_map_16
	$(ENV) ./tests/bin/thread_test_map_32

ttest_lockfree_map_flags=-DBUCKETSIZE=256 -DNODETYPE=ts_lockfree_node -DBUCKETTYPE=ts_map_bucket tests/ts_test.c++
thread_lockfree_test_map: tests/ts_test.c++
	$(CC) $(CFLAGS) $(ttest_lockfree_map_flags) -DNUM_THREADS=1 -o tests/bin/thread_test_lf_map_1
	$(CC) $(CFLAGS) $(ttest_lockfree_map_flags) -DNUM_THREADS=2 -o tests/bin/thread_test_lf_map_2
	$(CC) $(CFLAGS) $(ttest_lockfree_map_flags) -DNUM_THREADS=4 -o tests/bin/thread_test_lf_map_4
clean:
	rm -r -f tests/bin/
	mkdir tests/bin
	mkdir tests/bin/seq
	mkdir tests/bin/ts

DIR=$(shell pwd)
CC=g++
CFLAGS=-g -I$(DIR)/include -L$(DIR)/lib -O2 -DTESTSIZE=7500000 -lgsl -lgslcblas -lrt -Wall
GSL_RNG_SEED=`date +'%y%m%d%H%M%S'`
GSL_RNG_TYPE=uni
ENV=
LD_LIBRARY_PATH=$(DIR)/lib

tests: test_map test_trie 
#gsl_test thread_test

test_map: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -o tests/bin/test_map

test_trie: test_trie_map test_trie_unsorted test_trie_sorted test_trie_btree

test_trie_unsorted: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=32 -o tests/bin/test_trie_unsorted_32
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=32 -o tests/bin/test_trie_unsorted_64
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=128 -o tests/bin/test_trie_unsorted_128
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=256 -o tests/bin/test_trie_unsorted_256
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=512 -o tests/bin/test_trie_unsorted_512
test_trie_map:
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_map_bucket\
					-DBUCKETSIZE=128 -o tests/bin/test_trie_map_128
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_map_bucket\
					-DBUCKETSIZE=256 -o tests/bin/test_trie_map_256
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_map_bucket\
					-DBUCKETSIZE=512 -o tests/bin/test_trie_map_512
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_map_bucket\
					-DBUCKETSIZE=4096 -o tests/bin/test_trie_map_4096

test_trie_btree: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_btree_bucket\
					-DBUCKETSIZE=32 -o tests/bin/test_trie_btree_32
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_btree_bucket\
					-DBUCKETSIZE=64 -o tests/bin/test_trie_btree_64
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_btree_bucket\
					-DBUCKETSIZE=128 -o tests/bin/test_trie_btree_128
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_btree_bucket\
					-DBUCKETSIZE=256 -o tests/bin/test_trie_btree_256
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_btree_bucket\
					-DBUCKETSIZE=512 -o tests/bin/test_trie_btree_512

test_trie_sorted: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_sorted_bucket\
					-DBUCKETSIZE=32 -o tests/bin/test_trie_sorted_64
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_sorted_bucket\
					-DBUCKETSIZE=128 -o tests/bin/test_trie_sorted_128
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_sorted_bucket\
					-DBUCKETSIZE=256 -o tests/bin/test_trie_sorted_256
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_sorted_bucket\
					-DBUCKETSIZE=512 -o tests/bin/test_trie_sorted_512


run: tests
	$(ENV) find ./tests/bin/ -executable | /bin/sh

time: tests
	$(ENV) time -v ./tests/bin/test_map 2> logs/time.log
	echo "\n" >> logs/time.log
	$(ENV) time -v ./tests/bin/test_trie_unsorted 2>> logs/time.log
	echo "\n" >> logs/time.log
	$(ENV) time -v ./tests/bin/test_trie_sorted 2>> logs/time.log
	echo "\n" >> logs/time.log
	$(ENV) time -v ./tests/bin/test_trie_btree 2>> logs/time.log
	cat logs/time.log | most


shakespeare_ts: tests/shakespeare_ts_test.c++
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=1 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_1
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=2 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_2
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=4 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_4
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=8 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_8
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=32 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_32
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=1024 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_1024

ttest_btree_flags=-DBUCKETSIZE=32 -DBUCKETTYPE=ts_btree_bucket tests/ts_test.c++
thread_test_btree: tests/ts_test.c++
	$(CC) $(CFLAGS) $(ttest_btree_flags) -DNUM_THREADS=1 -o tests/bin/thread_test_btree_1
	$(CC) $(CFLAGS) $(ttest_btree_flags) -DNUM_THREADS=2 -o tests/bin/thread_test_btree_2
	$(CC) $(CFLAGS) $(ttest_btree_flags) -DNUM_THREADS=4 -o tests/bin/thread_test_btree_4
	$(CC) $(CFLAGS) $(ttest_btree_flags) -DNUM_THREADS=8 -o tests/bin/thread_test_btree_8
	$(CC) $(CFLAGS) $(ttest_btree_flags) -DNUM_THREADS=16 -o tests/bin/thread_test_btree_16
	$(CC) $(CFLAGS) $(ttest_btree_flags) -DNUM_THREADS=32 -o tests/bin/thread_test_btree_32

thread_test_btree_run: thread_test_btree
	./tests/bin/thread_test_btree_1
	./tests/bin/thread_test_btree_2
	./tests/bin/thread_test_btree_4
	./tests/bin/thread_test_btree_8
	./tests/bin/thread_test_btree_16
	./tests/bin/thread_test_btree_32

ttest_map_flags=-DBUCKETSIZE=128 -DBUCKETTYPE=ts_map_bucket tests/ts_test.c++
thread_test_map: tests/ts_test.c++
	$(CC) $(CFLAGS) $(ttest_map_flags) -DNUM_THREADS=1 -o tests/bin/thread_test_map_1
	$(CC) $(CFLAGS) $(ttest_map_flags) -DNUM_THREADS=2 -o tests/bin/thread_test_map_2
	$(CC) $(CFLAGS) $(ttest_map_flags) -DNUM_THREADS=4 -o tests/bin/thread_test_map_4
	$(CC) $(CFLAGS) $(ttest_map_flags) -DNUM_THREADS=8 -o tests/bin/thread_test_map_8
	$(CC) $(CFLAGS) $(ttest_map_flags) -DNUM_THREADS=16 -o tests/bin/thread_test_map_16
	$(CC) $(CFLAGS) $(ttest_map_flags) -DNUM_THREADS=32 -o tests/bin/thread_test_map_32

thread_test_map_run: thread_test_map
	./tests/bin/thread_test_map_1
	./tests/bin/thread_test_map_2
	./tests/bin/thread_test_map_4
	./tests/bin/thread_test_map_8
	./tests/bin/thread_test_map_16
	./tests/bin/thread_test_map_32

clean:
	rm -r -f tests/bin/
	mkdir tests/bin

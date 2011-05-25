CC=g++
CFLAGS=-g -O2 -DTESTSIZE=1000000 -lgsl -lgslcblas -lrt -Wall
GSL_RNG_SEED=`date +'%y%m%d%H%M%S'`
GSL_RNG_TYPE=uni
ENV=

tests: test_map test_trie 
#gsl_test thread_test

test_map: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -o tests/bin/test_map

test_trie: test_trie_map test_trie_unsorted test_trie_sorted test_trie_btree

test_trie_unsorted: tests/seq_test.c++
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=32 -o tests/bin/test_trie_unsorted_32
	$(CC) $(CFLAGS) tests/seq_test.c++ -DTRIE -DBUCKETTYPE=seq_unsorted_bucket\
					-DBUCKETSIZE=64 -o tests/bin/test_trie_unsorted_64
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
					-DBUCKETSIZE=64 -o tests/bin/test_trie_sorted_64
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

clean:
	rm -r -f tests/bin/
	mkdir tests/bin

shakespeare_ts: tests/shakespeare_ts_test.c++
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=1 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_1
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=2 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_2
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=4 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_4
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=8 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_8
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=32 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_32
	$(CC) $(CFLAGS) -DBUCKETSIZE=32 -DNUM_THREADS=1024 tests/shakespeare_ts_test.c++ -o tests/bin/shakespeare_ts_1024

thread_test: tests/ts_test.c++
	$(CC) $(CFLAGS) -DBUCKETSIZE=64 -DBUCKETTYPE=ts_btree_bucket -DNUM_THREADS=1 tests/ts_test.c++ -o tests/bin/thread_test_1
	$(CC) $(CFLAGS) -DBUCKETSIZE=64 -DBUCKETTYPE=ts_btree_bucket -DNUM_THREADS=2 tests/ts_test.c++ -o tests/bin/thread_test_2
	$(CC) $(CFLAGS) -DBUCKETSIZE=64 -DBUCKETTYPE=ts_btree_bucket -DNUM_THREADS=4 tests/ts_test.c++ -o tests/bin/thread_test_4
	$(CC) $(CFLAGS) -DBUCKETSIZE=64 -DBUCKETTYPE=ts_btree_bucket -DNUM_THREADS=8 tests/ts_test.c++ -o tests/bin/thread_test_8
	$(CC) $(CFLAGS) -DBUCKETSIZE=64 -DBUCKETTYPE=ts_btree_bucket -DNUM_THREADS=32 tests/ts_test.c++ -o tests/bin/thread_test_32
	$(CC) $(CFLAGS) -DBUCKETSIZE=64 -DBUCKETTYPE=ts_btree_bucket -DNUM_THREADS=1024 tests/ts_test.c++ -o tests/bin/thread_test_1024

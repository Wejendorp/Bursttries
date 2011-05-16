CC=g++
CFLAGS=-O2 -DTESTSIZE=500000 -DSEARCH=0 -lgsl -lgslcblas -lrt
ENV=GSL_RNG_SEED=15616 GSL_RNG_TYPE=uni

tests: test_map test_trie gsl_test thread_test

test_map: test.c++
	$(CC) $(CFLAGS) test.c++ -DTRIE=0 -DBUCKETTYPE=seq_unsorted_bucket -o test_map

test_trie: test.c++
	$(CC) $(CFLAGS) test.c++ -DTRIE=1 -DBUCKETTYPE=seq_unsorted_bucket -o test_trie_unsorted
	$(CC) $(CFLAGS) test.c++ -DTRIE=1 -DBUCKETTYPE=seq_sorted_bucket -o test_trie_sorted
	$(CC) $(CFLAGS) test.c++ -DTRIE=1 -DBUCKETTYPE=seq_btree_bucket -o test_trie_btree

massif: tests
	$(ENV) valgrind --tool=massif --massif-out-file=massif.map.out ./test_map
	$(ENV) valgrind --tool=massif --massif-out-file=massif.trie.unsorted.out ./test_trie_unsorted
	$(ENV) valgrind --tool=massif --massif-out-file=massif.trie.sorted.out ./test_trie_sorted
	$(ENV) valgrind --tool=massif --massif-out-file=massif.trie.btree.out ./test_trie_btree

massif_show:
	ms_print massif.map.out | head -n 30
	ms_print massif.trie.out | head -n 30
	rm -f ms_print.tmp.*

run: tests
	$(ENV) ./test_map
	$(ENV) ./test_trie

time: tests
	$(ENV) time -v ./test_map 2> time.log
	echo "\n" >> time.log
	$(ENV) time -v ./test_trie_unsorted 2>> time.log
	echo "\n" >> time.log
	$(ENV) time -v ./test_trie_sorted 2>> time.log
	echo "\n" >> time.log
	$(ENV) time -v ./test_trie_btree 2>> time.log
	cat time.log | most

clean:
	rm -f test_map test_trie *.o massif.map.out massif.trie.out

gsl_test: gsl_test.c++
	$(CC) $(CFLAGS) gsl_test.c++ -o gsl_test

thread_test: shakespeare_seq_test.c++
	$(CC) $(CFLAGS) shakespeare_seq_test.c++ -o thread_test

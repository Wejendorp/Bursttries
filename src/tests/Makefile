CC=g++
CFLAGS=-O2 -lgsl -lgslcblas -lpthread

tests: gsl_test thread_test

gsl_test: gsl_test.c++
	$(CC) $(CFLAGS) gsl_test.c++ -o gsl_test

thread_test: shakespeare_test.c++
	$(CC) $(CFLAGS) shakespeare_test.c++ -o thread_test

run: tests
	./gsl_test
	./thread_test

clean:
	rm -f thread_test gsl_test

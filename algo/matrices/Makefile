CC = mpicc
CFLAGS = -Wall -O2

all: matrix_multiplication_distributed_test

matrix_multiplication_distributed_test: matrix_multiplication_distributed_test.o matrix_multiplication_distributed.o matrix.o array.o
	$(CC) $(CFLAGS) -o matrix_multiplication_distributed_test matrix_multiplication_distributed_test.o matrix_multiplication_distributed.o matrix.o array.o
 
matrix_multiplication_distributed.o: matrix_multiplication_distributed.c matrix_multiplication_distributed.h matrix.h array.h
	$(CC) $(CFLAGS) -c matrix_multiplication_distributed.c -o matrix_multiplication_distributed.o

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c -o matrix.o

array.o: array.c array.h
	$(CC) $(CFLAGS) -c array.c -o array.o

.PHONY: clean
clean:
	rm -f *.o matrix_multiplication_distributed_test

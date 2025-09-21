CCSEQ = gcc
CCDIS = mpicc
CFLAGS = -Wall -O2

all: matrix_multiplication_distributed_test matrix_multiplication_serial_test

build-serial: matrix_multiplication_serial_test

build-distributed: matrix_multiplication_distributed_test

matrix_multiplication_distributed_test: matrix_multiplication_distributed_test.o matrix_multiplication_distributed.o matrix.o array.o
	$(CCDIS) $(CFLAGS) -o matrix_multiplication_distributed_test matrix_multiplication_distributed_test.o matrix_multiplication_distributed.o matrix.o array.o
 
matrix_multiplication_serial_test: matrix_multiplication_serial_test.o matrix_multiplication.o matrix.o array.o 
	$(CCSEQ) $(CFLAGS) -o matrix_multiplication_serial_test matrix_multiplication_serial_test.o matrix_multiplication.o matrix.o array.o 

matrix_multiplication_distributed_test.o: matrix_multiplication_distributed_test.c matrix.h array.h 
	$(CCDIS) $(CFLAGS) -c matrix_multiplication_distributed_test.c -o matrix_multiplication_distributed_test.o

matrix_multiplication_serial_test.o: matrix_multiplication_serial_test.c matrix_multiplication.h matrix.h array.h
	$(CCSEQ) $(CFLAGS) -c matrix_multiplication_serial_test.c -o matrix_multiplication_serial_test.o

matrix_multiplication_distributed.o: matrix_multiplication_distributed.c matrix_multiplication_distributed.h matrix.h array.h
	$(CCDIS) $(CFLAGS) -c matrix_multiplication_distributed.c -o matrix_multiplication_distributed.o

matrix_multiplication.o: matrix_multiplication.c matrix_multiplication.h matrix.h array.h
	$(CCSEQ) $(CFLAGS) -c matrix_multiplication.c -o matrix_multiplication.o

matrix.o: matrix.c matrix.h
	$(CCSEQ) $(CFLAGS) -c matrix.c -o matrix.o

array.o: array.c array.h
	$(CCSEQ) $(CFLAGS) -c array.c -o array.o

.PHONY: clean
clean:
	rm -f *.o matrix_multiplication_distributed_test matrix_multiplication_serial_test

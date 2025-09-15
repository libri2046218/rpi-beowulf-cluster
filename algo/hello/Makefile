CC = mpicc
CFLAGS = -Wall -O2

mpi_hello: mpi_hello.o
	$(CC) $(CFLAGS) -o mpi_hello mpi_hello.o

mpi_hello.o: mpi_hello.c
	$(CC) $(CFLAGS) -c mpi_hello.c

.PHONY: clean
clean:
	rm -f *.o mpi_hello

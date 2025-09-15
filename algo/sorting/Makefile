CC = mpicc
CFLAGS = -Wall -O2

all: merge_sort_distributed

merge_sort_distributed: merge_sort_distributed.o merge_sort.o array.o
	$(CC) $(CFLAGS) -o merge_sort_distributed merge_sort_distributed.o merge_sort.o array.o

merge_sort.o: merge_sort.c merge_sort.h 
	$(CC) $(CFLAGS) -c merge_sort.c -o merge_sort.o 

array.o: array.c array.h 
	$(CC) $(CFLAGS) -c array.c -o array.o 

.PHONY: clean
clean:
	rm -f *.o merge_sort_distributed

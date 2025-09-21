CCSEQ = gcc
CCDIS = mpicc
CFLAGS = -Wall -O2

all: merge_sort_distributed_test merge_sort_serial_test

build-serial: merge_sort_serial_test

build-distributed: merge_sort_distributed_test

merge_sort_distributed_test: merge_sort_distributed_test.o merge_sort_distributed.o merge_sort.o array.o
	$(CCDIS) $(CFLAGS) -o merge_sort_distributed_test merge_sort_distributed_test.o merge_sort_distributed.o merge_sort.o array.o

merge_sort_serial_test: merge_sort_serial_test.o merge_sort.o array.o 
	$(CCSEQ) $(CFLAGS) -o merge_sort_serial_test merge_sort_serial_test.o merge_sort.o array.o

merge_sort_distributed_test.o: merge_sort_distributed_test.c merge_sort_distributed.h merge_sort.h array.h
	$(CCDIS) $(CFLAGS) -c merge_sort_distributed_test.c -o merge_sort_distributed_test.o

merge_sort_serial_test.o: merge_sort_serial_test.c merge_sort.h array.h
	$(CCSEQ) $(CFLAGS) -c merge_sort_serial_test.c -o merge_sort_serial_test.o

merge_sort_distributed.o: merge_sort_distributed.c merge_sort_distributed.h merge_sort.h array.h 
	$(CCDIS) $(CFLAGS) -c merge_sort_distributed.c -o merge_sort_distributed.o 

merge_sort.o: merge_sort.c merge_sort.h 
	$(CCSEQ) $(CFLAGS) -c merge_sort.c -o merge_sort.o 

array.o: array.c array.h 
	$(CCSEQ) $(CFLAGS) -c array.c -o array.o 

.PHONY: all clean
clean:
	rm -f *.o merge_sort_distributed_test merge_sort_serial_test

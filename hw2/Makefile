CC = mpicc
CFLAGS =
LIBS = -lm
OBJECTS = main.o hw2harness.o cgsolve.o daxpy.o matvec.o ddot.o
EXECUTABLE = cgsolve

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o $(EXECUTABLE)
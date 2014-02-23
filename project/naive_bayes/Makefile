CC = gcc
CFLAGS = 
OBJECTS = main.o naive_bayes.o
EXECUTABLE = naivebayes

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXECUTABLE)
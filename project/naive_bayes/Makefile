CC = g++
CFLAGS = -std=c++0x
OBJECTS = main.o naive_bayes.o category_probabilities.o validator.o
EXECUTABLE = naivebayes

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXECUTABLE)
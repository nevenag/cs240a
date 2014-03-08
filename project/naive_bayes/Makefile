CC = g++
CFLAGS = -std=c++0x -I /home/ucsb-train22/boost/boost_1_55_0 -L /home/ucsb-train22/boost/boost_1_55_0/stage/lib -lboost_regex
OBJECTS = main.o naive_bayes.o category_probabilities.o validator.o
EXECUTABLE = naivebayes

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o $(EXECUTABLE)
GCC        = g++ -std=c++11
GCC_DEBUG  = -g -Wall -Weffc++

CPP_FILES  = main
CPP_FILES += Objet

OBJECTS    = $(addsuffix .o, $(CPP_FILES))

##########################

%.o : %.cpp
	$(GCC) $(GCC_DEBUG) -c $^

all: $(OBJECTS)
	$(GCC) $(GCC_DEBUG) $^

clean:
	rm -rf *.o a.out

##########################

.PHONY: all clean

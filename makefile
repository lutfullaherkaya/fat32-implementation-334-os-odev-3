OBJS	= main.o SISKO32.o Kabuk.o Bilimum.o
SOURCE	= main.cpp SISKO32.cpp Kabuk.cpp Bilimum.cpp
HEADER	= main.h fat32.h SISKO32.h Kabuk.h Bilimum.h
OUT	= hw3
CC	 = g++
FLAGS	 = -g -c
LFLAGS	 =

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp -std=c++11

SISKO32.o: SISKO32.cpp
	$(CC) $(FLAGS) SISKO32.cpp -std=c++11

Kabuk.o: Kabuk.cpp
	$(CC) $(FLAGS) Kabuk.cpp -std=c++11

Bilimum.o: Bilimum.cpp
	$(CC) $(FLAGS) Bilimum.cpp -std=c++11


clean:
	rm -f $(OBJS) $(OUT)

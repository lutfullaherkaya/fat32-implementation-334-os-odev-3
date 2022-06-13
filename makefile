OBJS	= main.o SISKO32.o Kabuk.o
SOURCE	= main.cpp SISKO32.cpp Kabuk.cpp
HEADER	= main.h fat32.h SISKO32.h Kabuk.h
OUT	= hw3
CC	 = g++
FLAGS	 = -g -c -Wall
LFLAGS	 =

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

SISKO32.o: SISKO32.cpp
	$(CC) $(FLAGS) SISKO32.cpp

Kabuk.o: Kabuk.cpp
	$(CC) $(FLAGS) Kabuk.cpp


clean:
	rm -f $(OBJS) $(OUT)

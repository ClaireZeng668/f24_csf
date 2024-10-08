CC = gcc
CFLAGS = -Wall -Wextra -pedantic
SRC_DIR = src
INC_DIR = include
OBJ = csim.o cache.o trace.o

all: csim

csim: $(OBJ)
    $(CC) $(CFLAGS) -o csim $(OBJ)

csim.o: $(SRC_DIR)/csim.c $(INC_DIR)/cache.h $(INC_DIR)/trace.h
    $(CC) $(CFLAGS) -c $(SRC_DIR)/csim.c

cache.o: $(SRC_DIR)/cache.c $(INC_DIR)/cache.h
    $(CC) $(CFLAGS) -c $(SRC_DIR)/cache.c

trace.o: $(SRC_DIR)/trace.c $(INC_DIR)/trace.h
    $(CC) $(CFLAGS) -c $(SRC_DIR)/trace.c

clean:
    rm -f *.o csim
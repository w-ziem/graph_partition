CC = gcc
CFLAGS =  -Wall -Wextra -O2 

SRC = graph.c error_handling.c io.c partition.c main.c
OBJ = $(SRC:.c=.o)
EXEC = graph_partition

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean

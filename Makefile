CC = gcc
CFLAGS =  -Wall -Wextra -O2 -lm

SRC = graph.c error_handling.c io.c partition.c main.c
OBJ = $(SRC:.c=.o)
EXEC = graph_partition

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -lm -g

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean

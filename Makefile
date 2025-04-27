CC = gcc
CFLAGS =  -Wall -Wextra -O2
LDFLAGS = -lm

SRC = graph.c error_handling.c io.c partition.c main.c
OBJ = $(SRC:.c=.o)
EXEC = graph_partition

TEST_SRC = test.c io.c graph.c error_handling.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_EXEC = test

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_EXEC) $(TEST_OBJ)

clean:
	rm -f $(OBJ) $(EXEC) $(TEST_OBJ) $(TEST_EXEC)

.PHONY: all clean

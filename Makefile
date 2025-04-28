CC = gcc
CFLAGS = -Wall -Wextra -O2

SRC = graph.c error_handling.c io.c partition.c main.c
OBJ = $(SRC:.c=.o)
EXEC = graph_partition

# Nowe pliki do testów
TEST_SRC = test.c graph.c error_handling.c io.c partition.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TEST_EXEC = test_graph_partition

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

# Cel do kompilacji testu
test: $(TEST_EXEC)

$(TEST_EXEC): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -g

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(EXEC) $(TEST_EXEC)

.PHONY: all clean test
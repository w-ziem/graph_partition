#ifndef IO_H
#define IO_H

#include "graph.h"
#include "partition.h"

// Funkcja wczytująca graf z pliku
csrrg_t* parse_csrrg(const char *filename);

// Funkcja zapisująca wyniki podziału grafu do pliku
void save_partition_to_file(const char *filename, partition_t partition, int vertices);

#endif // IO_H

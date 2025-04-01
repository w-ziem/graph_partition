#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>
#include <stdlib.h>

// Funkcja do obsługi błędów związanych z alokacją pamięci
void handle_memory_error(void *ptr, const char *msg);

// Funkcja do obsługi błędów związanych z otwieraniem pliku
void handle_file_error(FILE *file, const char *msg);

// Funkcja do obsługi błędów przy wczytywaniu danych
void handle_data_read_error(void *data, const char *msg);

// Funkcja do obsługi błędów związanych z błędnymi argumentami wejściowymi
void handle_argument_error(const char *msg);

#endif // ERROR_HANDLING_H

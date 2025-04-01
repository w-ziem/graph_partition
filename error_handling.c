#include "error_handling.h"

// Funkcja do obsługi błędów związanych z alokacją pamięci
void handle_memory_error(void *ptr, const char *msg) {
    if (ptr != NULL) {
        free(ptr);
    }
    fprintf(stderr, "Błąd alokacji pamięci: %s\n", msg);
    exit(EXIT_FAILURE);
}

// Funkcja do obsługi błędów związanych z otwieraniem pliku
void handle_file_error(FILE *file, const char *msg) {
    if (file != NULL) {
        fclose(file);
    }
    perror(msg);
    exit(EXIT_FAILURE);
}

// Funkcja do obsługi błędów przy wczytywaniu danych
void handle_data_read_error(void *data, const char *msg) {
    if (data != NULL) {
        free(data);
    }
    fprintf(stderr, "Błąd przy wczytywaniu danych: %s\n", msg);
    exit(EXIT_FAILURE);
}

// Funkcja do obsługi błędów związanych z błędnymi argumentami wejściowymi
void handle_argument_error(const char *msg) {
    fprintf(stderr, "Błąd argumentów: %s\n", msg);
    exit(EXIT_FAILURE);
}

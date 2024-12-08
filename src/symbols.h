#ifndef SYMBOLS_H
#define SYMBOLS_H

typedef struct symbol_t {
    char *raw;
    struct symbol_t *next;
} symbol_t;

typedef struct symbols_t {
    symbol_t *head;
    symbol_t *tail;
} symbols_t;

/* Create a new symbol table */
symbols_t *symbols_new();

/* Cleanup symbol table's memory */
void symbols_free(symbols_t *symbols);

/* Add a symbol to the symbol table and return a reference to its value. */
char *symbols_add(symbols_t *symbols, const char *raw);

/* Remove all existing symbols.  */
void symbols_reset(symbols_t *symbols);
#endif

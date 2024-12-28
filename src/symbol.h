#ifndef SYMBOL_H
#define SYMBOL_H

typedef struct symbol_t {
    char *raw;
    struct symbol_t *next;
} symbol_t;

typedef struct symboltable_t {
    symbol_t *head;
    symbol_t *tail;
} symboltable_t;

/* Create a new symbol table */
symboltable_t *symboltable_new(void);

/* Cleanup symbol table's memory */
void symboltable_free(symboltable_t *symbols);

/* Add a symbol to the symbol table and return a reference to its value. */
char *symboltable_add(symboltable_t *symbols, const char *raw);
#endif

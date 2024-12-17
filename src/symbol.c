#include "symbol.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

symboltable_t *symboltable_new(void) {
    symboltable_t *symbols = malloc(sizeof(symboltable_t));

    symbols->head = NULL;
    symbols->tail = NULL;

    return symbols;
}

void symbol_free(symbol_t *symbol) {
    while (symbol != NULL) {
        symbol_t *next = symbol->next;

        free(symbol->raw);
        free(symbol);

        symbol = next;
    }
}

void symboltable_free(symboltable_t *symbols) {
    if (symbols == NULL) {
        return;
    }

    symbol_free(symbols->head);

    free(symbols);
}

const char *symboltable_add(symboltable_t *symbols, const char *raw) {
    symbol_t *symbol = malloc(sizeof(symbol_t));
    symbol->raw = strdup(raw);
    symbol->next = NULL;

    if (symbols->head == NULL) {
        assert(symbols->tail == NULL);

        symbols->head = symbol;
        symbols->tail = symbol;
    } else {
        symbols->tail->next = symbol;
        symbols->tail = symbols->tail->next;
    }

    return symbol->raw;
}

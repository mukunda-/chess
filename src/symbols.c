#include "symbols.h"

#include <stdlib.h>
#include <string.h>

symbols_t *symbols_new() {
    symbols_t *symbols = malloc(sizeof(symbols_t));

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

void symbols_free(symbols_t *symbols) {
    if (symbols == NULL) {
        return;
    }

    symbol_free(symbols->head);

    free(symbols);
}

char *symbols_add(symbols_t *symbols, const char *raw) {
    symbol_t *symbol = malloc(sizeof(symbol_t));
    symbol->raw = strdup(raw);

    if (symbols->tail == NULL) {
        symbols->tail = symbol;
        symbols->head = symbol;
    } else {
        symbols->tail->next = symbol;
        symbols->tail = symbols->tail->next;
    }

    return symbol->raw;
}

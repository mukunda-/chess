#ifndef CLIARGS_H
#define CLIARGS_H

typedef enum cliargs_option_t { CLIARGS_OPTION_MIN_PLY } cliargs_option_t;

typedef struct cliargs_t {
    cliargs_option_t option;
    char* error;
    struct cliargs_t* next;
};

cliargs_t* cliargs_new();

void cliargs_free(cliargs_t* args);

#endif

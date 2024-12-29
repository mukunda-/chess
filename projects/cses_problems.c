#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SUBMIT increasing_array

/************
 * Utilities
 ************/

typedef struct list_t {
    struct number_t* head;
} list_t;

typedef struct number_t {
    long val;
    struct number_t* next;
} number_t;

number_t* number_new(long val) {
    number_t* num = malloc(sizeof(number_t));
    num->val = val;
    num->next = NULL;

    return num;
}

void number_free(number_t* num) {
    while (num != NULL) {
        number_t* next = num->next;

        free(num);
        num = next;
    }
}

list_t* list_new(void) {
    list_t* list = malloc(sizeof(list_t));

    list->head = NULL;

    return list;
}

void list_free(list_t* list) {
    number_free(list->head);
    free(list);
}

void list_print(list_t* list) {
    const char* prefix = "";
    for (number_t* num = list->head; num != NULL; num = num->next) {
        printf("%s%ld", prefix, num->val);
        prefix = " ";
    }

    printf("\n");
}

void list_insert_head(list_t* list, long val) {
    number_t* num = number_new(val);
    num->next = list->head;
    list->head = num;
}

void list_insert_after(list_t* list, long val, number_t* after) {
    if (after == NULL) {
        list_insert_head(list, val);
    } else {
        number_t* num = number_new(val);
        num->next = after->next;
        after->next = num;
    }
}

void list_insert(list_t* list, long val) { list_insert_after(list, val, NULL); }

number_t* list_find_last_lt(list_t* list, long val) {
    number_t* num = list->head;
    while (num != NULL) {
        if (num->val < val && (num->next == NULL || num->next->val >= val)) {
            return num;
        }

        num = num->next;
    }

    return NULL;
}

void list_sorted_insert(list_t* list, long val) {
    number_t* last_lt = list_find_last_lt(list, val);
    if (last_lt != NULL) {
        list_insert_after(list, val, last_lt);
    } else {
        list_insert_head(list, val);
    }
}

void test_list(void) {
    list_t* list = list_new();
    /*
    list_insert_head(list, 3);
    list_insert_head(list, 2);
    list_insert_head(list, 1);

    assert(list_find_last_lt(list, 1) == NULL);
    assert(list_find_last_lt(list, 2)->val == 1);
    assert(list_find_last_lt(list, 3)->val == 2);
    assert(list_find_last_lt(list, 4)->val == 3);
    assert(list_find_last_lt(list, 5)->val == 3);

    list_sorted_insert(list, 4);
    list_sorted_insert(list, 5);

    assert(list->head->val == 1);
    assert(list->head->next->val == 2);
    assert(list->head->next->next->val == 3);
    assert(list->head->next->next->next->val == 4);
    assert(list->head->next->next->next->next->val == 5);
    */
    list_free(list);
}

/************
 * Solutions
 ************/
void increasing_array(void) {}

/************
 * Submission
 ************/

int main(void) {
    SUBMIT();
    test_list();
    printf("\n");
}

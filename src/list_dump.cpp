#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../my_libs/sassert.h"
#include "../my_libs/error_manage.h"
#include "../my_libs/better_output.h"
#include "sys/stat.h"
#include "list.h"
#include "list_dump.h"

extern error_t error;
static size_t DumpedTimes = 0;

int verify_list(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    int free = list->free;
    int head = list->prev[0];
    int tail = list->next[0];
    int current = 0;
    int capacity = list->capacity;
    int size = list->size;
    int previous = 0;
    int count_els = 0;
    if (tail < 0 || head < 0 || free < 0 || size < 0 || tail > capacity || free > capacity || head > capacity || size > capacity) {
        add_error(ERR_INVALID_ARGUMENTS, "tail: %d, head: %d, free: %d, size: %d", tail, head, free, size);
        return ERR_INVALID_ARGUMENTS;
    }

    while (current != head) {
        if (count_els++ > size + 1) {
            add_error(ERR_INVALID_SIZE, "size: %d, count_els: %d", size, count_els);
            return ERR_INVALID_SIZE;
        }
        if (current < -1 || current > size + 1 || list->next[current] > size + 1 || list->next[current] < -1) {
            add_error(ERR_INVALID_NEXT, "%d", current);
            return ERR_INVALID_NEXT;
        }
        if (list->prev[current] > size + 1 || list->prev[current] < -1) {
            add_error(ERR_INVALID_PREV, "%d", current);
            return ERR_INVALID_PREV;
        }
        previous = current;
        current = list->next[current];
    }
    if (count_els > size + 1) {
        add_error(ERR_INVALID_SIZE, "size: %d, count_els: %d", size, count_els);
        return ERR_INVALID_SIZE;
    }
    return 0;
}

void DumplistCsv(list_t *list, FILE *fp) {
    sassert(list, ERR_PTR_NULL);

    int current = list->next[list->tail];
    for (size_t i = 0; i < list->size; i++) {
        char * Data = list->data[current];
        if (Data == POISON)
            fputs("PSN\n", fp);
        else
            fprintf(fp, "%s\n", Data);
        current = list->next[current];
    }
}
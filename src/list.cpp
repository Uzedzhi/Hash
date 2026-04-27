#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../my_libs/sassert.h"
#include "../my_libs/error_manage.h"
#include "../my_libs/better_output.h"
#include "sys/stat.h"
#include "list_dump.h"
#include "list.h"

error_t error = {};

void print_order_of_data(FILE * fp, list_t *list) {
    sassert(fp, ERR_PTR_NULL);

    int current = list->next[0];
    for (size_t i = 0; i < list->size; i++) {
        fprintf(fp, "[%-4s]", list->data[current]);
        current = list->next[current];
    }
}

error_t listCtor_internal(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    list->free = 1;
    list->size = 0;
    list->capacity = START_LIST_SIZE;
    list->data = (data_t *) calloc(START_LIST_SIZE, sizeof(data_t));
    sassert(list->data, ERR_PTR_NULL);

    list->next = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->next, ERR_PTR_NULL);

    list->prev = (int *) calloc(START_LIST_SIZE, sizeof(int));
    sassert(list->prev, ERR_PTR_NULL);

    list->data[0] = POISON;
    list->prev[0] = 0;

    for (size_t i = 1; i < list->capacity; i++) {
        list->next[i] = i+1;
        list->prev[i] = i-1;
        list->data[i] = POISON;
    }
    list->next[START_LIST_SIZE - 1] = 0;

    list->next[0] = 0;
    list->prev[0] = 0;

    return error;
}

void * safe_realloc(void ** memory, size_t new_size) {
    sassert(memory, ERR_PTR_NULL);

    // temporary array to not lose actual
    void *temp = realloc(*memory, new_size);
    if (temp == NULL) {
        add_error(ERR_REALLOC_FAIL, "couldn't enlarge your array");
        return NULL;
    }

    *memory = temp;
    return *memory;
}

error_t add_element_before_internal(list_t *list, size_t index, data_t value) {
    sassert(list, ERR_PTR_NULL);

#ifndef NDEBUG
    if (verify_list(list) != 0)
        return error;
#endif
    
    if (index > list->size || index < 1) {
        add_error(ERR_INVALID_INDEX, "index=%zu size=%zu", index, list->size);
        return error;
    }
    if (list->size >= list->capacity - 1) {
        reallocate_up_list(list);
    }

    int prev_index = list->prev[index];
    int free_space = list->free;
    list->free = get_next_element(list, list->free);

    list->data[free_space]  = value;
    list->next[prev_index]  = free_space;
    list->prev[index]       = free_space;

    list->prev[free_space]  = prev_index;
    list->next[free_space]  = index;

    list->size++;

    return error;
}

void add_in_head(list_t *list, data_t value) {
    sassert(list, ERR_PTR_NULL);

    add_element_after_internal(list, list->prev[list->next[0]], value);
    return;
}

error_t reallocate_up_list(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    list->data      = (data_t *) safe_realloc((void **) &list->data, list->capacity * REALLOC_STD_MUL * sizeof(data_t));
    list->next      = (int *)    safe_realloc((void **) &list->next, list->capacity * REALLOC_STD_MUL * sizeof(int));
    list->prev      = (int *)    safe_realloc((void **) &list->prev, list->capacity * REALLOC_STD_MUL * sizeof(int));

    for (size_t i = list->capacity; i < list->capacity * REALLOC_STD_MUL; i++) {
        list->data[i] = POISON;
        list->next[i] = i + 1;
        list->prev[i] = i - 1;
    }
    list->next[list->capacity * REALLOC_STD_MUL - 1] = 0;
    list->free = max(list->capacity, list->free);
    list->capacity *= REALLOC_STD_MUL;
    return error;
}

size_t get_next_element(list_t *list, size_t index) {
    if (index < list->capacity)
        return list->next[index];
    return 0;
}

error_t reallocate_down_list(list_t *list, size_t factor, bool by_order) {
    sassert(list, ERR_PTR_NULL);

    size_t size = list->size;
    size_t capacity = list->capacity;
#ifndef NDEBUG
    int err = verify_list(list);
    if (err != 0) {
        add_error((lstErrors) err, "fatal error");
        return error;
    }
#endif

    if (size * factor >= capacity) {
        add_error(ERR_REALLOC_DOWN_FAIL, "factor: %zu, exp_size: %lf", factor, (double) capacity / factor)
        return error;
    }

    if (by_order)
        linearize_list_by_order(list);
    else
        linearize_list(list);

    size_t new_size = 0;
    if (factor == 0) {
        new_size = (size + 1) * sizeof(int);
    }
    else if (size * factor < capacity) {
        new_size = capacity / factor * sizeof(int);
    }

    list->data = (data_t *) safe_realloc((void **) &list->data, new_size);
    list->next = (int *)    safe_realloc((void **) &list->next, new_size);
    list->prev = (int *)    safe_realloc((void **) &list->prev, new_size);
    list->capacity = new_size / sizeof(int);
    list->next[list->capacity - 1] = 0;
    if (list->free > list->capacity)
        list->free = 0;
    return error;
}

void linearize_list(list_t *list) {
    sassert(list, ERR_PTR_NULL);

    size_t index_behind = 0;
    size_t index_ahead = 1;
    size_t count = 0;
    while (count != list->size) {
        if (list->data[index_ahead] == POISON) {
            index_ahead++;
            list->free = get_next_element(list, list->free);
            for (size_t i = index_behind; i < list->capacity; i++) {
                if (list->next[i] != 0 && list->data[i] != POISON)
                    list->next[i]--;
            }
            for (size_t i = index_behind; i < list->capacity; i++) {
                if (list->prev[i] > index_behind)
                    list->prev[i]--;
            }
            continue;
        }
        else {
            index_behind++;
            list->data[index_behind] = list->data[index_ahead];
            list->next[index_behind] = list->next[index_ahead];
            list->prev[index_behind] = list->prev[index_ahead];
            index_ahead++;
        }
        count++;
    }

    int current = list->next[0];
    count = 0;
    while (count++ < list->size) {
        if (list->next[current] != 0)
            current = list->next[current];
    }
    list->prev[0] = current;

}

int max(int a, int b) {
    return (a > b) ? a : b;
}

extern "C" int MyStrcmp(const char *str1, const char *str2) noexcept {
    int cmp = 1;
    __asm (
        ".intel_syntax noprefix\n"
        
        "vpxor ymm2, ymm2, ymm2\n"      // ymm2 = 0
        "vmovdqu ymm0, [%[str1]]\n"     // ymm0 = str1
        "vmovdqu ymm1, [%[str2]]\n"     // ymm1 = str2
        
        "vpcmpeqb ymm1, ymm0, ymm1\n"   // ymm1: 0xFF if str1[i] == str2[i]
        "vpcmpeqb ymm2, ymm0, ymm2\n"   // ymm2: 0xFF if str1[i] == 0

        "vpmovmskb ecx, ymm1\n"         // ecx[i] = 1 if str1[i] == str2[i]
        "vpmovmskb eax, ymm2\n"         // eax[i] = 1 if str1[i] == 0

        "not ecx\n"                     // ecx[i] = 1 if str1[i] != str2[i]
        
        "test ecx, ecx\n"
        "jz .Equal\n"

        "bsf ecx, ecx\n"
        "bsf eax, eax\n"
        
        "cmp ecx, eax\n"
        "ja .Equal\n"

        // Not Equal
        "mov %[result], 1\n"
        "jmp .Exit\n"

        // Equal
        ".Equal:\n"
        "mov %[result], 0\n"
        "jmp .Exit\n"

        ".Exit:\n"
        "vzeroupper\n\t"
        ".att_syntax prefix\n\t"
        : [result] "=r" (cmp)
        : [str1]   "r"  (str1), [str2] "r" (str2)
        : "rax", "rcx", "ymm0", "ymm1", "ymm2", "cc"
    );

    return cmp;
}

void linearize_list_by_order(list_t *list) {
    sassert(list, ERR_PTR_NULL);
    int current = list->next[0];
    int last_element_index = 0;
    int count = 1;
    data_t order_of_values[list->size] = {};
    while (count <= list->size) {
        order_of_values[count] = list->data[current];
        last_element_index = max(last_element_index, current);
        current = list->next[current];
        last_element_index = max(last_element_index, current);
        count++;
    }
    
    for (size_t i = 1; i <= list->size; i++) {
        list->data[i] = order_of_values[i];
        list->next[i] = i + 1;
        list->prev[i] = i - 1;
    }

    list->next[count - 1] = 0;

    list->next[0] = 1;
    list->prev[0] = list->size;
}

void add_element_after_internal(list_t * list, size_t index, data_t value) {
    sassert(list, ERR_PTR_NULL);

#ifndef NDEBUG
    if (verify_list(list) != 0)
        return;
#endif

    if (index > list->size || index < 0 || (list->data[index] == POISON && index != 0)) {
        add_error(ERR_INVALID_INDEX, " index=%zu size=%zu", index, list->size);
        return;
    }
    if (list->size >= list->capacity - 1) {
        reallocate_up_list(list);
    }
    size_t next_index = get_next_element(list, index);
    int free_space = list->free;
    list->free = get_next_element(list, list->free);

    list->data[free_space] = value;

    list->prev[free_space] = index;
    list->next[free_space] = next_index;

    list->next[index]      = free_space;
    list->prev[next_index] = free_space;

    list->size++;

    return;
}

error_t remove_element_internal(list_t *list, int index) {
    sassert(list, ERR_PTR_NULL);

#ifndef NDEBUG
    if (verify_list(list) != 0)
        return error;
#endif
    
    if (index < 1 || index >= list->capacity || list->data[index] == POISON) {
        add_error(ERR_OUT_OF_BOUNDS, "index:%d", index);
        return error;
    }

    if (list->size == 0) {
        add_error(ERR_NOTHING_TO_POP, "size = 0");
        return error;
    }
    int prev_index = list->prev[index];
    int next_index = list->next[index];

    list->prev[list->free] = index;
    list->next[index] = list->free;
    list->free = index;

    list->next[prev_index] = next_index;
    list->prev[next_index] = prev_index;

    free(list->data[index]);
    list->data[index] = POISON;
    list->size--;
    return error;
}

void list_iterator_next(list_iterator_t *it) {
    sassert(it,         ERR_PTR_NULL);
    sassert(it->list,   ERR_PTR_NULL);
    it->current = it->list->next[it->current];
}

bool list_iterator_end(list_iterator_t *it) {
    sassert(it, ERR_PTR_NULL);
    return (it->current == it->list->next[it->list->prev[0]]);
}

data_t list_iterator_value(list_iterator_t *it) {
    sassert(it,         ERR_PTR_NULL);
    sassert(it->list,   ERR_PTR_NULL);
    return it->list->data[it->current];
}

size_t getListSize(const list_t *list) {
    sassert(list, ERR_PTR_NULL);
    return list->size;
}

error_t listDtor_internal(list_t *list) {
    if (list == NULL)
        return error;
    
    if (list->data != NULL)
        free(list->data);
    if (list->prev != NULL)
        free(list->prev);
    if (list->next != NULL)
        free(list->next);
    free(list);
    return error;
}
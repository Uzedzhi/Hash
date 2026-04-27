#ifndef LIST_H
#define LIST_H

#include "stdio.h"
#include "stdlib.h"
#include "../my_libs/error_manage.h"

extern error_t error;
typedef const char * const string;
typedef char * data_t;
const  size_t START_LIST_SIZE   = 32;
static size_t count             = 0;
const  size_t MAX_STR_SIZE      = 200;
const  data_t POISON            = (data_t) -1;
const  size_t INITIAL_HEAD_VAL  = 1;
const  size_t INITIAL_TAIL_VAL  = 1;
const  size_t INITIAL_FREE_VAL  = 1;
const  size_t MAX_TESTING_SIZE  = 100;
const size_t REALLOC_STD_MUL    = 3;
string unitest_file_name    = "unitest.txt";
string dump_graph_file_name = "graph.txt";
string dump_site_file_name  = "dump.html";
string operations_descriptions[]    = {"add_after ", "before operation happened ", "remove ", "add_before "};
string operations_classes[]         = {"add_after", "start", "remove", "add_before"};

typedef struct OneWayArray_t {
    int * next;
} OneWayArray_t;

typedef struct {
    data_t *data;
    int *next;
    int *prev;
    int tail;
    int head;
    int free;
    size_t size;
    size_t capacity;
} list_t;

#define x(n) \
    n(ERR_PTR_NULL                      ,   0)  \
    n(ERR_CMD_INVALID                   ,   1)  \
    n(ERR_INVALID_INDEX                 ,   2)  \
    n(ERR_OUT_OF_BOUNDS                 ,   3)  \
    n(ERR_NOTHING_TO_POP                ,   4)  \
    n(ERR_INVALID_OPERATION             ,   5)  \
    n(ERR_REALLOC_FAIL                  ,   6)  \
    n(ERR_INCORRECT_LIST                ,   7)  \
    n(ERR_CYCLING_LIST                  ,   8)  \
    n(ERR_INVALID_ARGUMENTS             ,   9)  \
    n(ERR_INVALID_PREV                  ,   10) \
    n(ERR_INVALID_NEXT                  ,   11) \
    n(ERR_INVALID_SIZE                  ,   12) \
    n(ERR_INVALID_RELATION_WITH_PREV    ,   13) \
    n(ERR_INVALID_RELATION_WITH_NEXT    ,   14) \
    n(ERR_REALLOC_DOWN_FAIL             ,   15)
#define init_enum(val, ind) \
    val = ind,
#define init_str(val, ...) \
    #val, 

enum lstErrors {
    x(init_enum)
};
string error_text[] = {x(init_str)};

#undef x
#undef init_enum
#undef init_str

enum operations {
    ADD_AFTER       = 0,
    START           = 1,
    REMOVE          = 2,
    ADD_BEFORE      = 3,
    UNDEF_OPERATION = 4
};

typedef struct {
    list_t *list;
    int current; 
} list_iterator_t;

#define list_iterator_ctor(name, list) \
    list_iterator_t name = {list, list->next[0]}

#ifndef NDEBUG
#define listCtor(list_name) \
    list_t *list_name = (list_t *) calloc(1, sizeof(list_t));\
    listCtor_internal(list_name);\

#define listDtor(list) \
    listDtor_internal(list);\

#define add_element_after(list, index, value) {\
    create_dot_image_dump(list);\
    print_to_html(list, START, index, value);\
\
    add_element_after_internal(list, index, value);\
    create_dot_image_dump(list);\
    print_to_html(list, ADD_AFTER, index, value);\
\
}
    
    

#define add_element_before(list, index, value) {\
    create_dot_image_dump(list);\
    print_to_html(list, START, index, value);\
\
    add_element_before_internal(list, index, value);\
    create_dot_image_dump(list);\
    print_to_html(list, ADD_BEFORE, index, value);\
\
   }


#define remove_element(list, index) {\
    create_dot_image_dump(list);\
    print_to_html(list, START, index, POISON);\
\
    remove_element_internal(list, index);\
    create_dot_image_dump(list);\
    print_to_html(list, REMOVE, index, POISON);\
}

#else
#define listCtor(list_name) \
    list_t *list_name = (list_t *) calloc(1, sizeof(list_t));\
    listCtor_internal(list_name);
#define listDtor(list) \
    listDtor_internal(list);
#define remove_element(list, index) \
    remove_element_internal(list, index);

#define add_element_before(list, index, value) \
    add_element_before_internal(list, index, value);

#define add_element_after(list, index, value) \
    add_element_after_internal(list, index, value);
#endif 

void fill_with_poison(int * array, int start, int end);
error_t reallocate_up_list(list_t *list);
void list_iterator_next(list_iterator_t *it);
data_t list_iterator_value(list_iterator_t *it);
bool list_iterator_end(list_iterator_t *it);
size_t getListSize(const list_t *list);
void * safe_realloc(void ** memory, size_t new_size);
error_t listDtor_internal(list_t *list);
void add_in_head(list_t *list, data_t value);
size_t get_next_element(list_t *list, size_t index);
error_t remove_element_internal(list_t *list, int index);
void add_element_after_internal(list_t * list, size_t index, data_t value);
error_t add_element_before_internal(list_t *list, size_t index, data_t value);
void print_site_headers();
void linearize_list(list_t *list);
int max(int a, int b);
void linearize_list_by_order(list_t *list_t);
error_t reallocate_down_list(list_t *list, size_t factor, bool by_order);
error_t listCtor_internal(list_t *list);
void print_order_of_data(FILE * fp, list_t *list);
void print_command_execution_failed(const char * file_name);
extern "C" int MyStrcmp(const char *s1, const char *s2) noexcept;
#ifdef USE_MYSTRCMP
#define strcmp(str1, str2) MyStrcmp(str1, str2)
#endif
#endif // LIST_H
#ifndef LIST_DUMP_H
#define LIST_DUMP_H

#include "list.h"

error_t open_live_server(const char * site_name);
error_t print_to_html(list_t *list, operations operation, size_t index, data_t value);
error_t create_dot_image_next_array_dump(list_t *list);
error_t create_dot_image_prev_array_dump(list_t *list);
error_t create_dot_image_dump(list_t *list);
void add_error_to_html();
error_t create_dot_main_array_dump(list_t *list, FILE * fp);
void print_site_toes();
void DumplistCsv(list_t *list, FILE *fp);
void print_divider(FILE * fp);
int verify_list(list_t *list);
void print_info(list_t *list);
void print_errorss(list_t *list);

#endif // LIST_DUMP_H
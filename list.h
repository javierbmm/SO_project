//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#ifndef SO_PROJECT_LIST_H
#define SO_PROJECT_LIST_H

#include <stdlib.h>
#include <unistd.h>

typedef struct node {
    void* data;
    struct node * next;
} node_t;


void* remove_by_index(node_t ** head, int n);
void* remove_last(node_t * head);
void* pop(node_t ** head);
void push_init(node_t ** head, void* val);
void push_end(node_t * head, void* val);

#endif //SO_PROJECT_LIST_H

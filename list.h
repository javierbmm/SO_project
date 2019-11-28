//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#ifndef SO_PROJECT_LIST_H
#define SO_PROJECT_LIST_H
typedef struct node {
    void* data;
    struct node * next;
} node_t;


int remove_by_index(node_t ** head, int n);
int remove_last(node_t * head);
int pop(node_t ** head);
void push_init(node_t ** head, int val);
void push_end(node_t * head, int val);

#endif //SO_PROJECT_LIST_H

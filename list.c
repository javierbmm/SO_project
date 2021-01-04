//
// Created by Olívia Rey Pellicer on 28/11/2019.
//

#include "list.h"

/*
 * List push. It creates a new node and stores the new information there
 */
void push_end(node_t * head, void* val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = malloc(sizeof(node_t));
    current->next->data = val;
    current->next->next = NULL;
}
/*
 * It stores the new information initialized
 */
void push_init(node_t ** head, void* val) {
    node_t * new_node;
    new_node = malloc(sizeof(node_t));

    new_node->data = val;
    new_node->next = *head;
    *head = new_node;
}

/*
 * It deletes the node and information from the list
 */
void* pop(node_t ** head) {
    void* retval = NULL;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return NULL;
    }

    next_node = (*head)->next;
    retval = (*head)->data;
    free(*head);
    *head = next_node;

    return retval;
}
/*
 * It removes the last added node
 */
void* remove_last(node_t * head) {
    void* retval = NULL;
    /* if there is only one item in the list, remove it */
    if (head->next == NULL) {
        retval = head->data;
        free(head);
        return retval;
    }

    /* get to the second to last node in the list */
    node_t * current = head;
    while (current->next->next != NULL) {
        current = current->next;
    }

    /* now current points to the second to last item of the list, so let's remove current->next */
    retval = current->next->data;
    free(current->next);
    current->next = NULL;
    return retval;
}
/*
 * It removes the information located in the node (number passed by parameter)
 */
void* remove_by_index(node_t ** head, int n) {
    int i = 0;
    void* retval = NULL;
    node_t * current = *head;
    node_t * temp_node = NULL;

    if (n == 0) {
        return pop(head);
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return NULL;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->data;
    current->next = temp_node->next;
    free(temp_node);

    return retval;

}
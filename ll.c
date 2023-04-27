#include <stdio.h>
#include <stdlib.h>

#include "ll.h"

struct LinkedList {
    uint32_t length;
    Node *head;
    Node *tail;
    bool mtf;
};

uint64_t seeks = 0;
uint64_t links = 0;

//test check: success
static bool my_strcmp(char* s1, char* s2) {
    while (*s1 || *s2) {
        if (*s1 == *s2) {
            s1 += 1;
            s2 += 1;
        } else if ((!*s1 && *s2) || (*s1 && !*s2) || *s1 != *s2) {
            return false;
        }
    }
    return true;
}

//test check: success
LinkedList *ll_create(bool mtf) {
    LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
    ll->length = 0;
    ll->head = node_create("sentinel_head_old", "sentinel_head_new");
    ll->tail = node_create("sentinel_tail_old", "sentinel_tail_new");
    ll->head->next = ll->tail;
    ll->tail->prev = ll->head;
    ll->mtf = mtf;
    return ll;
}

//test check: success
void ll_delete(LinkedList **ll) {
    Node *tmp;
    while ((*ll)->head) {
        tmp = (*ll)->head;
        (*ll)->head = (*ll)->head->next;
        node_delete(&tmp);
    }
    free(*ll);
    *ll = NULL;
    return;
}

//test check: success
uint32_t ll_length(LinkedList *ll) {
    return ll->length;
}

//test check: success
Node *ll_lookup(LinkedList *ll, char *oldspeak) {
    Node *pointer = ll->head;
    seeks += 1;
    while (pointer != ll->tail) {                       //returns NULL pointer if no matching oldspeak found
        if (my_strcmp(oldspeak, pointer->oldspeak)) {
            if (ll->mtf) {
                pointer->next->prev = pointer->prev;    //remove pointer node from list
                pointer->prev->next = pointer->next;
                pointer->prev = ll->head;
                pointer->next = ll->head->next;
                ll->head->next->prev = pointer;
                ll->head->next = pointer;
            }
            break;
        }
        if (pointer->next != ll->tail) {
            links += 1;
        }
        pointer = pointer->next;
    }
    if (pointer == ll->tail) {
        pointer = NULL;
    }
    return pointer;
}    

//test check: success
void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
    Node *ptr = ll_lookup(ll, oldspeak);
    if (!ptr) {
        ll->length += 1;
        ptr = node_create(oldspeak, newspeak);
        ptr->prev = ll->head;                   //fix oldspeak node
        ptr->next = ll->head->next;
        ll->head->next->prev = ptr;             //fix head->next and head
        ll->head->next = ptr;
    } else if (ll->mtf) {
        ptr->prev->next = ptr->next;            //disassociate oldspeak node
        ptr->next->prev = ptr->prev;
        ptr->prev = ll->head;                   //fix oldspeak node
        ptr->next = ll->head->next;
        ll->head->next->prev = ptr;             //fix head->next and head
        ll->head->next = ptr;
    }
    return;
}
        
//test check: success
void ll_print(LinkedList *ll) {
    Node *ptr = ll->head->next;
    while (ptr != ll->tail) {
        node_print(ptr);
        ptr = ptr->next;
    }
    return;
}

//test check: success
void ll_stats(uint32_t * n_seeks, uint32_t * n_links) {
    *n_seeks = seeks;
    *n_links = links;
    return;
}

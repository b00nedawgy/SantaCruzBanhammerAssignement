#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "node.h"

//my strdup function since the strdup() function in the standard C library has been deprecated
static char *my_strdup(const char *s) {
    char *res, *ptr;
    int max = 0;
    while (s[max]) {
        max+=1;
    }
    res = (char *) malloc((max + 1)*sizeof(char));
    ptr = res;
    int i;
    for (i = 0; s[i]; i += 1) {
        ptr[i] = s[i];
    }
    ptr[i] = '\0';
    return res;
}

static int my_scmp_equal(char *w1, char *w2) {
    int i = 0;
    for (i = 0; w1[i] || w2[i]; i += 1) {
        if (w1[i] != w2[i]) {
            return 1;
        }
    }
    /**
    if (w1[i+1] || w2[i + 1]) {
        printf("?");
        return 1;
    }
    **/
    return 0;
}

Node *node_create(char *oldspeak, char *newspeak) {
    Node *n =  (Node *) malloc(sizeof(Node));

    if (n != NULL) {
        do {
            n->oldspeak = my_strdup(oldspeak);
            n->newspeak = my_strdup(newspeak);
        } while (my_scmp_equal(n->oldspeak, oldspeak) == 1);
    }
    return n;
}

void node_delete(Node **n) {
    /**
    if ((*n)->next && (*n)->next->prev && (*n)->prev) {
        (*n)->next->prev = (*n)->prev;
    }
    if ((*n)->prev && (*n)->prev->next && (*n)->next) {
        (*n)->prev->next = (*n)->next;
    }
    **/
    if ((*n)->next && (*n)->prev) {
        if (!(*n)->next->prev || !(*n)->prev->next) {
            fprintf(stderr, "uinitialized value used\n");
        }
        (*n)->next->prev = (*n)->prev;
        (*n)->prev->next = (*n)->next;
    } else if ((*n)->next) {
        if (!(*n)->next->prev) {
            fprintf(stderr, "uinitialized value used\n");
        }
        (*n)->next->prev = NULL;
    } else if ((*n)->prev) {
        if (!(*n)->prev->next) {
            fprintf(stderr, "uinitialized value used\n");
        }
        (*n)->prev->next = NULL;
    }

    free((*n)->oldspeak);
    free((*n)->newspeak);
    free(*n);
    *n = NULL;
    return;
}


void node_print(Node *n) {
    if (my_scmp_equal(n->newspeak, "NulL string") == 0) {
        printf("%s\n", n->oldspeak);
    } else {
        printf("%s -> %s\n", n->oldspeak, n->newspeak);
    }
    return;
}

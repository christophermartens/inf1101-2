#include <stdlib.h>
#include <stdio.h>

#include "set.h"

typedef struct node node_t;

struct node {
    void *item;
    node_t *next;
};

struct set {
    cmpfunc_t cmpfunc;
    node_t *head;
    int size;
};


/*
 * Creates a new set using the given comparison function
 * to compare elements of the set.
 */
set_t *set_create(cmpfunc_t cmpfunc) {
    set_t *set = malloc(sizeof(set_t));

    if (set == NULL)
        return NULL;

    set->cmpfunc = cmpfunc;
    set->head = NULL;
    set->size = 0;

    return set;
}

/*
 * Destroys the given set.  Subsequently accessing the set
 * will lead to undefined behavior.
 */
void set_destroy(set_t *set) {
    node_t *tmp;
    node_t *cur;

    cur = set->head;

    while(cur != NULL) {
        tmp = cur;
        cur = cur->next;
        free(tmp);
    }

    free(set);
}

/*
 * Returns the size (cardinality) of the given set.
 */
int set_size(set_t *set) {
    return set->size;
}

/*
 * Adds the given element to the given set.
 */
void set_add(set_t *set, void *elem) {
    node_t *new = malloc(sizeof(node_t));

    if (new == NULL) {
        return;
    }
    new->item = elem;

    node_t *tmp = set->head;

    /* Empty set */
    if (tmp == NULL) {
        set->head = new;
        new->next = NULL;
        set->size++;
        return;
    }

    /* Set with one item */
    if (tmp->next == NULL) {
        if (set->cmpfunc(tmp->item, new->item) < 0) {
            tmp->next = new;
            new->next = NULL;
            set->size++;
            return;
        } else if (set->cmpfunc(tmp->item, new->item) > 0) {
            set->head = new;
            new->next = tmp;
            set->size++;
            return;
        }
        else {
            return;
        }
    }

    /* Set with more than one item */
    node_t *prev = tmp;
    tmp = tmp->next;

    // This can probably be simplified.
    if (set->cmpfunc(set->head->item, elem) == 0) {
        return;
    }
    else if (set->cmpfunc(set->head->item, elem) > 0) {
        new->next = set->head;
        set->head = new;
        set->size++;
        return;
    }

    while (tmp != NULL) {
        if (set->cmpfunc(tmp->item, elem) == 0)
            return;
        if (set->cmpfunc(tmp->item, elem) > 0) {
            prev->next = new;
            new->next = tmp;
            set->size++;
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    prev->next = new;
    new->next = NULL;
    set->size++;
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem) {
    node_t *tmp = set->head;

    while (tmp != NULL) {
        if (set->cmpfunc(tmp->item, elem) == 0)
            return 1;
        tmp = tmp->next;
    }

    return 0;
}

/*
 * Returns the union of the two given sets; the returned
 * set contains all elements that are contained in either
 * a or b.
 */
set_t *set_union(set_t *a, set_t *b) {
    set_t *set = set_create(a->cmpfunc);

    if (set == NULL)
        return NULL;

    set_t *aa = set_copy(a);
    set_t *bb = set_copy(b);
    if (aa == NULL || bb == NULL)
        return NULL;

    node_t *tmp_a, *tmp_b;

    tmp_a = aa->head;
    tmp_b = bb->head;

    /* If one set is empty, return the other. */
    if (tmp_a == NULL) {
        return bb;
    }
    if (tmp_b == NULL) {
        return aa;
    }

    /* Find head node for union set. */
    if (set->cmpfunc(tmp_a->item, tmp_b->item) > 0) {
        set->head = tmp_b;
        set->size++;
        tmp_b = tmp_b->next;
    } else if (set->cmpfunc(a->head->item, b->head->item) == 0) {
        set->head = tmp_b;
        set->size++;
        tmp_b = tmp_b->next;
        tmp_a = tmp_a->next;
    } else {
        set->head = tmp_a;
        set->size++;
        tmp_a = tmp_a->next;
    }

    node_t *tmp_u = set->head;

    /* Insert elements in ascending order. */
    while (tmp_a != NULL && tmp_b != NULL) {
        if (set->cmpfunc(tmp_a->item, tmp_b->item) > 0) {
            tmp_u->next = tmp_b;
            tmp_b = tmp_b->next;
            set->size++;
        } else if (set->cmpfunc(tmp_a->item, tmp_b->item) == 0) {
            tmp_u->next = tmp_b;
            tmp_b = tmp_b->next;
            tmp_a = tmp_a->next;
            set->size++;
        } else {
            tmp_u->next = tmp_a;
            tmp_a = tmp_a->next;
            set->size++;
        }
        tmp_u = tmp_u->next;
    }

    /* Add remaining elements to union set. */
    if (tmp_a != NULL) {
        while (tmp_a != NULL) {
            tmp_u->next = tmp_a;
            tmp_a = tmp_a->next;
            set->size++;
            tmp_u = tmp_u->next;
        }
    }
    if (tmp_b != NULL) {
        while (tmp_b != NULL) {
            tmp_u->next = tmp_b;
            tmp_b = tmp_b->next;
            set->size++;
            tmp_u = tmp_u->next;
        }
    }

    return set;
}

/*
 * Returns the intersection of the two given sets; the
 * returned set contains all elements that are contained
 * in both a and b.
 */
set_t *set_intersection(set_t *a, set_t *b) {
    set_t *set = set_create(a->cmpfunc);
    if (set == NULL)
        return NULL;

    set_t *aa = set_copy(a);
    set_t *bb = set_copy(b);
    if (aa == NULL || bb == NULL)
        return NULL;

    node_t *tmp_a, *tmp_b, *tmp_i;

    tmp_a = aa->head;
    tmp_b = bb->head;
    tmp_i = NULL;

    /* If one of the sets is empty, return an empty set (no intersection.) */
    if (tmp_a == NULL)
        return aa;
    if (tmp_b == NULL)
        return bb;

    /* Add all elements with equal value. */
    while (tmp_a != NULL && tmp_b != NULL) {
        if (set->cmpfunc(tmp_a->item, tmp_b->item) > 0) {
            tmp_b = tmp_b->next;
        } else if (set->cmpfunc(tmp_a->item, tmp_b->item) == 0) {
            if (tmp_i != NULL) {
                tmp_i->next = tmp_b;
                tmp_b = tmp_b->next;
                tmp_a = tmp_a->next;
                set->size++;
                tmp_i = tmp_i->next;
            } else {
                set->head = tmp_b;
                tmp_b = tmp_b->next;
                tmp_a = tmp_a->next;
                set->size++;
                tmp_i = set->head;
            }
        } else {
            tmp_a = tmp_a->next;
        }
    }

    if (tmp_i != NULL)
        tmp_i->next = NULL;

    return set;
}

/*
 * Returns the set difference of the two given sets; the
 * returned set contains all elements that are contained
 * in a and not in b.
 */
set_t *set_difference(set_t *a, set_t *b) {
    set_t *set = set_create(a->cmpfunc);

    if (set == NULL)
        return NULL;

    set_t *aa = set_copy(a);
    set_t *bb = set_copy(b);
    if (aa == NULL || bb == NULL)
        return NULL;

    node_t *tmp_a, *tmp_b, *tmp_d;
    tmp_a = aa->head;
    tmp_b = bb->head;
    tmp_d = NULL;

    /* If a is empty, return an empty set. If b is empty, return a. */
    if (tmp_a == NULL)
        return set;
    if (tmp_b == NULL)
        return aa;

    /* Insert elements from A that are not contained in B. */
    while (tmp_a != NULL && tmp_b != NULL) {
        if (set->cmpfunc(tmp_a->item, tmp_b->item) > 0) {
            tmp_b = tmp_b->next;
        } else if (set->cmpfunc(tmp_a->item, tmp_b->item) == 0) {
            tmp_b = tmp_b->next;
            tmp_a = tmp_a->next;
        } else {
            if (tmp_d != NULL) {
                tmp_d->next = tmp_a;
                set->size++;
                tmp_a = tmp_a->next;
                tmp_d = tmp_d->next;
            } else {
                set->head = tmp_a;
                tmp_a = tmp_a->next;
                set->size++;
                tmp_d = set->head;
            }
        }
    }

    /* Add remaining elements. */
    if (tmp_a != NULL) {
        while (tmp_a != NULL) {
            tmp_d->next = tmp_a;
            tmp_a = tmp_a->next;
            set->size++;
            tmp_d = tmp_d->next;
        }
    } else {
        if (tmp_d != NULL)
            tmp_d->next = NULL;
    }

    return set;
}

/*
 * Returns a copy of the given set.
 */
set_t *set_copy(set_t *set) {
    set_t *copy = set_create(set->cmpfunc);

    if (copy == NULL)
        return NULL;

    if (set->head == NULL)
        return copy;

    node_t *tmp = set->head;

    node_t *node = malloc(sizeof(node_t));

    if (node == NULL) {
        set_destroy(copy);
        return NULL;
    }

    node->item = tmp->item;
    node->next = NULL;
    copy->head = node;
    copy->size++;
    tmp = tmp->next;

    node_t *tmp_c = copy->head;

    while (tmp != NULL && tmp_c != NULL) {
        node_t *new = malloc(sizeof(node_t));
        if (node == NULL) {
            set_destroy(copy);
            return NULL;
        }

        new->item = tmp->item;
        new->next = NULL;
        copy->size++;
        tmp_c->next = new;


        tmp_c = tmp_c->next;
        tmp = tmp->next;
    }

    return copy;
}

/*
 * The type of set iterators.
 */
struct set_iter {
    node_t *node;
};

/*
 * Creates a new set iterator for iterating over the given set.
 */
set_iter_t *set_createiter(set_t *set) {
    set_iter_t *iter = malloc(sizeof(set_iter_t));
    if (iter == NULL) {
        return NULL;
    }

    iter->node = set->head;

    return iter;
}

/*
 * Destroys the given set iterator.
 */
void set_destroyiter(set_iter_t *iter) {
    free(iter);
}

/*
 * Returns 0 if the given set iterator has reached the end of the
 * set, or 1 otherwise.
 */
int set_hasnext(set_iter_t *iter) {
    if (iter->node == NULL) {
        return 0;
    }

    return 1;
}

/*
 * Returns the next element in the sequence represented by the given
 * set iterator.
 */
void *set_next(set_iter_t *iter) {
    if (iter->node == NULL) {
        return NULL;
    }
    void *elem = iter->node->item;
    iter->node = iter->node->next;
    return elem;
}

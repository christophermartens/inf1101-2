#include <stdlib.h>
#include <stdio.h>

#include "set.h"

#define MAX_ITEMS 16

struct set {
    cmpfunc_t cmpfunc;
    void **items;
    int size;
    int max_size;
};

/*
 * Creates a new set using the given comparison function
 * to compare elements of the set.
 */
set_t *set_create(cmpfunc_t cmpfunc) {
    set_t *set = malloc(sizeof(set_t));

    if (set == NULL)
        return NULL;

    set->items = calloc(MAX_ITEMS, sizeof(void *) * MAX_ITEMS);
    if (set->items == NULL) {
        free(set);
        return NULL;
    }

    set->size = 0;
    set->max_size = MAX_ITEMS;
    set->cmpfunc = cmpfunc;

    return set;
}

/*
 * Destroys the given set. Subsequently accessing the set
 * will lead to undefined behavior.
 */
void set_destroy(set_t *set) {
    free(set->items);
    free(set);
}

/*
 * Returns the size (cardinality) of the given set.
 */
int set_size(set_t *set) {
    return set->size;
}

void set_resize(set_t *set) {
    void **items_copy;
    int new_size;

    new_size = set->max_size * 2;

    items_copy = realloc(set->items, sizeof(void*) * new_size);
    if (items_copy == NULL)
        return;

    set->max_size = new_size;
    set->items = items_copy;
}

/*
 * Adds the given element to the given set.
 */
void set_add(set_t *set, void *elem) {
    // If necessary, double maximum set size.
    if (set->size >= set->max_size)
        set_resize(set);

    // Add item if set is empty.
    if (set->size == 0) {
        set->items[0] = elem;
        set->size++;
        return;
    }

    // Iterate through set, return if element already exists, else add element at right position.
    for (int i = 0; i < set->size; i++) {
        if (set->cmpfunc(elem, set->items[i]) < 0) {
            for (int j = set->size; j > i; j--) {
                set->items[j] = set->items[j-1];
            }
            set->items[i] = elem;
            set->size++;
            return;
        } else if (set->cmpfunc(elem, set->items[i]) == 0) {
            return;
        }
    }
    set->items[set->size] = elem;
    set->size++;
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem) {
    for (int i = 0; i < set->size; i++) {
        if (set->cmpfunc(set->items[i], elem) == 0)
            return 1;
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

    if (a->size == 0)
        return b;
    if (b->size == 0)
        return a;

    int pos = 0;
    int a_pos = 0;
    int b_pos = 0;

    /* Add elements until one set is empty. */
    while (a_pos < a->size && b_pos < b->size) {
        if (set->size >= set->max_size)
            set_resize(set);

        if (set->cmpfunc(a->items[a_pos], b->items[b_pos]) > 0) {
            set->items[pos] = b->items[b_pos];
            b_pos++;
            set->size++;
            pos++;
        } else if (set->cmpfunc(a->items[a_pos], b->items[b_pos]) == 0) {
            set->items[pos] = b->items[b_pos];
            b_pos++;
            a_pos++;
            set->size++;
            pos++;
        } else {
            set->items[pos] = a->items[a_pos];
            a_pos++;
            set->size++;
            pos++;
        }
    }

    /* Add remaining elements from non-empty set. */
    while (a_pos < a->size) {
        if (set->size >= set->max_size){
            set_resize(set);
        }
        set->items[pos] = a->items[a_pos];
        a_pos++;
        pos++;
        set->size++;
    }
    while (b_pos < b->size) {
        if (set->size >= set->max_size) {
            set_resize(set);
        }
        set->items[pos] = b->items[b_pos];
        b_pos++;
        pos++;
        set->size++;
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

    // If either set is empty, return the empty set.
    if (a->size == 0)
        return a;
    if (b->size == 0)
        return b;

    int pos, a_pos, b_pos;
    pos = 0;
    a_pos = 0;
    b_pos = 0;

    /* Add all elements with equal value. */
    while (a_pos < a->size && b_pos < b->size) {
        if (set->cmpfunc(a->items[a_pos], b->items[b_pos]) > 0) {
            b_pos++;
        } else if (set->cmpfunc(a->items[a_pos], b->items[b_pos]) == 0) {
            if (set->size >= set->max_size)
                set_resize(set);

            set->items[pos] = a->items[a_pos];
            set->size++;
            a_pos++;
            b_pos++;
            pos++;
        } else {
            a_pos++;
        }
    }

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

    /* If a is empty, return an empty set. If b is empty, return a. */
    if (a->size == 0)
        return set;
    if (b->size == 0)
        return a;

    int pos, a_pos, b_pos;
    pos = 0;
    a_pos = 0;
    b_pos = 0;

    /* Insert elements from a that are not contained in b. */
    while (a_pos < a->size && b_pos < b->size) {
        if (set->cmpfunc(a->items[a_pos], b->items[b_pos]) > 0) {
            b_pos++;
        } else if (set->cmpfunc(a->items[a_pos], b->items[b_pos]) == 0) {
            a_pos++;
            b_pos++;
        } else {
            if (set->size >= set->max_size)
                set_resize(set);

            set->items[pos] = a->items[a_pos];
            set->size++;
            a_pos++;
            pos++;
        }
    }

    /* Add remaining elements */
    while (a_pos < a->size) {
        if (set->size >= set->max_size){
            set_resize(set);
        }
        set->items[pos] = a->items[a_pos];
        a_pos++;
        pos++;
        set->size++;
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

    int pos = 0;

    for (int i = 0; i < set->size; i++) {
        if (set->size >= set->max_size)
            set_resize(set);

        copy->items[pos] = set->items[i];
        copy->size++;
        pos++;
    }

    return copy;
}

/*
 * The type of set iterators.
 */
struct set_iter {
    set_t *set;
    int index;
};

/*
 * Creates a new set iterator for iterating over the given set.
 */
set_iter_t *set_createiter(set_t *set) {
    set_iter_t *iter = malloc(sizeof(set_iter_t));
    if (iter == NULL) {
        return NULL;
    }

    iter->set = set;
    iter->index = 0;

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
    if (iter->index == iter->set->size) {
        return 0;
    }
    return 1;
}

/*
 * Returns the next element in the sequence represented by the given
 * set iterator.
 */
void *set_next(set_iter_t *iter) {
    if (iter->index >= iter->set->size)
        return NULL;

    void *elem = iter->set->items[iter->index];
    iter->index++;
    return elem;
}

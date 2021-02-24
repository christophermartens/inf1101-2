#include <stdlib.h>
#include <stdio.h>

#include "set.h"
#include "list.h"

struct set {
    list_t *list;
    cmpfunc_t cmpfunc;
};

/*
 * Creates a new set using the given comparison function
 * to compare elements of the set.
 */
set_t *set_create(cmpfunc_t cmpfunc) {
    set_t *set = malloc(sizeof(set_t));

    if (set == NULL) {
        return NULL;
    }

    list_t *list = list_create(cmpfunc);
    if (list == NULL) {
        return NULL;
    }

    set->list = list;
    set->cmpfunc = cmpfunc;

    return set;
}

/*
 * Destroys the given set. Subsequently accessing the set
 * will lead to undefined behavior.
 */
void set_destroy(set_t *set) {
    list_destroy(set->list);
    free(set);
}

/*
 * Returns the size (cardinality) of the given set.
 */
int set_size(set_t *set) {
    return list_size(set->list);
}

/*
 * Adds the given element to the given set.
 */
void set_add(set_t *set, void *elem) {
    list_iter_t *iter = list_createiter(set->list);

    if (iter == NULL) {
        printf("Error\n");
        return;
    }

    while (list_hasnext(iter)) {
        if (set->cmpfunc(elem, list_next(iter)) == 0)
            return;
    }

    list_addfirst(set->list, elem);
    list_sort(set->list);
    list_destroyiter(iter);
}

/*
 * Returns 1 if the given element is contained in
 * the given set, 0 otherwise.
 */
int set_contains(set_t *set, void *elem) {
    list_iter_t *iter = list_createiter(set->list);

    if (iter == NULL) {
        printf("Error\n");
        return -1;
    }

    while (list_hasnext(iter)) {
        if (set->cmpfunc(elem, list_next(iter)) == 0) {
            list_destroyiter(iter);
            return 1;
        }
    }

    list_destroyiter(iter);
    return 0;
}

/*
 * Returns the union of the two given sets; the returned
 * set contains all elements that are contained in either
 * a or b.
 */
set_t *set_union(set_t *a, set_t *b) {
    set_t *set_union = set_create(a->cmpfunc);

    if (set_union == NULL) {
        return NULL;
    }

    list_iter_t *iter_a = list_createiter(a->list);
    list_iter_t *iter_b = list_createiter(b->list);
    if (iter_a == NULL || iter_b == NULL) {
        return NULL;
    }

    while (list_hasnext(iter_a)) {
        set_add(set_union, list_next(iter_a));
    }
    while (list_hasnext(iter_b)) {
        set_add(set_union, list_next(iter_b));
    }

    list_destroyiter(iter_a);
    list_destroyiter(iter_b);

    return set_union;
}

/*
 * Returns the intersection of the two given sets; the
 * returned set contains all elements that are contained
 * in both a and b.
 */
set_t *set_intersection(set_t *a, set_t *b) {
    set_t *set_intersection = set_create(a->cmpfunc);

    if (set_intersection == NULL) {
        return NULL;
    }

    list_iter_t *iter_a = list_createiter(a->list);
    if (iter_a == NULL) {
        return NULL;
    }

    while (list_hasnext(iter_a)) {
        list_iter_t *iter_b = list_createiter(b->list);
        if (iter_b == NULL) {
            return NULL;
        }

        void *elem_a = list_next(iter_a);

        while (list_hasnext(iter_b)) {
            void *elem_b = list_next(iter_b);

            if (set_intersection->cmpfunc(elem_a, elem_b) == 0) {
                set_add(set_intersection, elem_b);
            }
        }

        list_destroyiter(iter_b);
    }

    list_destroyiter(iter_a);
    return set_intersection;
}

/*
 * Returns the set difference of the two given sets; the
 * returned set contains all elements that are contained
 * in a and not in b.
 */
set_t *set_difference(set_t *a, set_t *b) {
    set_t *set_difference = set_create(a->cmpfunc);

    if (set_difference == NULL) {
        return NULL;
    }

    list_iter_t *iter_a = list_createiter(a->list);
    if (iter_a == NULL) {
        return NULL;
    }

    while (list_hasnext(iter_a)) {
        list_iter_t *iter_b = list_createiter(b->list);
        if (iter_b == NULL) {
            return NULL;
        }

        void *elem_a = list_next(iter_a);

        int add = 1;
        while (list_hasnext(iter_b)) {
            void *elem_b = list_next(iter_b);

            if (set_difference->cmpfunc(elem_a, elem_b) == 0) {
                add = 0;
                break;
            }
        }
        if (add == 1)
            set_add(set_difference, elem_a);

        list_destroyiter(iter_b);
    }

    list_destroyiter(iter_a);
    return set_difference;
}

/*
 * Returns a copy of the given set.
 */
set_t *set_copy(set_t *set) {
    set_t *set_copy = set_create(set->cmpfunc);

    if (set_copy == NULL) {
        return NULL;
    }

    list_iter_t *iter = list_createiter(set->list);

    if (iter == NULL) {
        return NULL;
    }

    while (list_hasnext(iter)) {
        set_add(set_copy, list_next(iter));
    }

    list_destroyiter(iter);

    return set_copy;
}

/*
 * The type of set iterators.
 */
struct set_iter {
    list_iter_t *list_iter;
};

/*
 * Creates a new set iterator for iterating over the given set.
 */
set_iter_t *set_createiter(set_t *set) {
    set_iter_t *set_iter = malloc(sizeof(set_iter_t));
    if (set_iter == NULL)
        return NULL;

    set_iter->list_iter = list_createiter(set->list);
    if (set_iter->list_iter == NULL) {
        return NULL;
    }

    return set_iter;
}

/*
 * Destroys the given set iterator.
 */
void set_destroyiter(set_iter_t *iter) {
    list_destroyiter(iter->list_iter);
    free(iter);
}

/*
 * Returns 0 if the given set iterator has reached the end of the
 * set, or 1 otherwise.
 */
int set_hasnext(set_iter_t *iter) {
    if (list_hasnext(iter->list_iter)) {
        return 1;
    }

    return 0;
}

/*
 * Returns the next element in the sequence represented by the given
 * set iterator.
 */
void *set_next(set_iter_t *iter) {
    return list_next(iter->list_iter);
}

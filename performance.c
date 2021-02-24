#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "set.h"
#include "list.h"

#define MAX_INT 20000

/* Taken from lecture notes for lecture 2. */
/* TODO: Rewrite */
unsigned long long gettime()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1)
    {
        fprintf(stderr, "Could not get time\n");
        return -1;
    }

    unsigned long long micros = 1000000 * tv.tv_sec + tv.tv_usec;

    return micros;
}

/* Compares two integers. */
int compare(void *a, void *b) {
    return *((int *) (a)) - *((int *) (b));
}

/* Generates a list of integers and adds them to a set. */
set_t *generate(int n, int order) {
    list_t *list = list_create(compare);
    if (list == NULL)
        return NULL;

    if (order == 1) {
        for (int i = 0; i < n; i++) {
            int *a = malloc(sizeof(int *));
            *a = i;
            list_addfirst(list, a);
        }
    } else if (order == 0) {
        srand(gettime());
        for (int i = 0; i < n; i++)
        {
            int *a = malloc(sizeof(int *));
            *a = rand() % MAX_INT;
            list_addfirst(list, a);
        }
    } else {
        for (int i = n; i > 0; i--) {
            int *a = malloc(sizeof(int *));
            *a = i;
            list_addfirst(list, a);
        }
    }

    set_t *set = set_create(compare);

    list_iter_t *listIter = list_createiter(list);
    if (listIter == NULL)
        return NULL;

    unsigned long long t1 = gettime();
    while(list_hasnext(listIter)) {
        set_add(set, list_next(listIter));
    }
    unsigned long long t2 = gettime();
    fprintf(stdout, "%d %lld ", set_size(set), t2 - t1);

    list_destroyiter(listIter);
    list_destroy(list);

    return set;
}

/* Creates the union between two sets. */
void test_union(set_t *a, set_t *b) {
    unsigned long long t1 = gettime();
    set_t *set = set_union(a, b);
    unsigned long long t2 = gettime();
    fprintf(stdout, "%lld", t2 - t1);
    set_destroy(set);
}

/* Intersection between two sets. */
void test_intersection(set_t *a, set_t *b) {
    unsigned long long t1 = gettime();
    set_t *set = set_intersection(a, b);
    unsigned long long t2 = gettime();
    fprintf(stdout, " %lld", t2 - t1);
    set_destroy(set);
}

/* Difference between two sets */
void test_difference(set_t *a, set_t *b) {
    unsigned long long t1 = gettime();
    set_t *set = set_difference(a, b);
    unsigned long long t2 = gettime();
    fprintf(stdout, " %lld\n", t2 - t1);
    set_destroy(set);
}

int main(int argc, char **argv) {
    set_t *a, *b;

    /* Gets the order of the generated integer list. */
    int order = atoi(argv[1]);

    for (int j = 0; j < 10; j++) {
        int n = 16;
        for (int i = 0; i < 10; i++) {
            a = generate(n, order);
            b = generate(n, order);

            test_union(a, b);
            test_intersection(a, b);
            test_difference(a, b);

            n *= 2;
        }
    }

    /* Cleanup */
    set_destroy(a);
    set_destroy(b);

    return 0;
}

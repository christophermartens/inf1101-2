/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#include "list.h"
#include "set.h"
#include "common.h"

#include <sys/time.h>

/*
 * Case-insensitive comparison function for strings.
 */
static int compare_words(void *a, void *b)
{
    return strcasecmp(a, b);
}

/*
 * Returns the set of (unique) words found in the given file.
 */
static set_t *tokenize(char *filename)
{
	set_t *wordset = set_create(compare_words);
	list_t *wordlist = list_create(compare_words);
	list_iter_t *it;
	FILE *f;
	
	f = fopen(filename, "r");
	if (f == NULL) {
		perror("fopen");
		fatal_error("fopen() failed");
	}
	tokenize_file(f, wordlist);
	
	it = list_createiter(wordlist);
	while (list_hasnext(it)) {
		set_add(wordset, list_next(it));		
	}
	list_destroyiter(it);
	list_destroy(wordlist);
	return wordset;
}

/*
 * Prints a set of words.
 */
static void printwords(char *prefix, set_t *words)
{
	set_iter_t *it;
	
	it = set_createiter(words);
	printf("%s: ", prefix);
	while (set_hasnext(it)) {
		printf(" %s", set_next(it));
	}
	printf("\n");
	set_destroyiter(it);
}

/*
 * Main entry point.
 */
int main(int argc, char **argv)
{
	char *spamdir, *nonspamdir, *maildir;
	
	if (argc != 4) {
		fprintf(stderr, "usage: %s <spamdir> <nonspamdir> <maildir>\n",
				argv[0]);
		return 1;
	}
	spamdir = argv[1];
	nonspamdir = argv[2];
	maildir = argv[3];

	set_t *spam_set = set_create(compare_words);
	set_t *non_spam_set = set_create(compare_words);
	set_t *mail_set = set_create(compare_words);

	if (spam_set == NULL || non_spam_set == NULL || mail_set == NULL)
	    return -1;

	list_t *spam_files = find_files(spamdir);
	list_t *non_spam_files = find_files(nonspamdir);
	list_t *mail_files = find_files(maildir);
	list_sort(mail_files);

	list_iter_t *spam_iter = list_createiter(spam_files);
	list_iter_t *non_spam_iter = list_createiter(non_spam_files);
	list_iter_t *mail_iter = list_createiter(mail_files);

	set_t *spam_prev = tokenize(list_next(spam_iter));

    // add all spam words to a set
	while (list_hasnext(spam_iter)) {
	    set_t *spam = tokenize(list_next(spam_iter));
	    spam_set = set_intersection(spam_prev, spam);
	    spam_prev = spam_set;
	}
	list_destroyiter(spam_iter);
	list_destroy(spam_files);

    // add all non spam words to a set
    while (list_hasnext(non_spam_iter)) {
	    non_spam_set = set_union(non_spam_set, tokenize(list_next(non_spam_iter)));
	}
    list_destroyiter(non_spam_iter);
    list_destroy(non_spam_files);

	// create one set per email
    // compare email set with spam and non spam set
    while (list_hasnext(mail_iter)) {
	    char *filename = list_next(mail_iter);
	    char *spam = "SPAM";
	    char *not_spam = "Not spam";
	    char *message;

	    mail_set = tokenize(filename);

	    set_t *filter = set_intersection(mail_set, set_difference(spam_set, non_spam_set));

	    if (set_size(filter) == 0) {
	        message = not_spam;
	    }
	    else {
	        message = spam;
	    }

	    printf("%s: %d spam word(s) -> %s\n", filename, set_size(filter), message);
	    set_destroy(filter);
	}

    // cleanup
    list_destroyiter(mail_iter);
    list_destroy(mail_files);

    return 0;
}
